
#include "Joystick.h"

static uint8_t PrevJoystickHIDReportBuffer[sizeof(USB_JoystickReport_Data_t)];

USB_ClassInfo_HID_Device_t Joystick_HID_Interface = {
  .Config = {
    .InterfaceNumber = INTERFACE_ID_Joystick,
    .ReportINEndpoint = {
      .Address              = JOYSTICK_EPADDR,
      .Size                 = JOYSTICK_EPSIZE,
      .Banks                = 1,
    },
    .PrevReportINBuffer           = PrevJoystickHIDReportBuffer,
    .PrevReportINBufferSize       = sizeof(PrevJoystickHIDReportBuffer),
  },
};

#define IO_PORT		PORTD
#define IO_DDR		DDRD
#define IO_PIN		PIND

#define DATA5_PIN	0
#define DATA3_PIN	1
#define DATA2_PIN	2
#define SELECT_PIN	3
#define DATA1_PIN	4
#define DATA4_PIN	5
#define DATA0_PIN	6

#define INPUT_PINS_MASK	(_BV(DATA0_PIN)|\
			 _BV(DATA1_PIN)|\
			 _BV(DATA2_PIN)|\
			 _BV(DATA3_PIN)|\
			 _BV(DATA4_PIN)|\
			 _BV(DATA5_PIN))
#define OUTPUT_PINS_MASK (_BV(SELECT_PIN))

typedef union {
  uint8_t raw;
  struct {
    unsigned int a:1;
    unsigned int right:1;
    unsigned int left:1;
    unsigned int :1;
    
    unsigned int down:1;
    unsigned int start:1;
    unsigned int up:1;
    unsigned int :1;
  };
  struct {
    unsigned int b:1;
    unsigned int mode:1;
    unsigned int x:1;
    unsigned int :1;
    
    unsigned int y:1;
    unsigned int c:1;
    unsigned int z:1;
    unsigned int :1;
  };
} input_defs;

inline void set_select_low(void) {
  IO_PORT &= ~(_BV(SELECT_PIN));
}

inline void set_select_high(void) {
  IO_PORT |= _BV(SELECT_PIN);
}

inline bool is_three_buttons_Frame(input_defs i) {
  return !i.left && !i.right;
}

inline bool is_six_buttons_Frame(input_defs i) {
  return !i.left && !i.right && !i.up && !i.down;
}

inline void set_directions(USB_JoystickReport_Data_t* JoystickReport, input_defs inputs) {
  if (!inputs.up)
    JoystickReport->Y = -127;
  else if (!inputs.down)
    JoystickReport->Y = 127;
  
  if (!inputs.left)
    JoystickReport->X = -127;
  else if (!inputs.right)
    JoystickReport->X = 127;
}

inline void read_two_buttons_frame(USB_JoystickReport_Data_t* JoystickReport, input_defs inputs) {
  set_directions(JoystickReport, inputs);
  
  if (!inputs.a)
    JoystickReport->button.square = 1;
  
  if (!inputs.start)
    JoystickReport->button.cross = 1;
}

inline void read_three_buttons_frames(USB_JoystickReport_Data_t* JoystickReport, input_defs inputs) {
  set_select_high();
  if (!inputs.a)
    JoystickReport->button.cross = 1;
  if (!inputs.start)
    JoystickReport->button.start = 1;
  inputs.raw = IO_PIN;
  set_select_low();
  
  set_directions(JoystickReport, inputs);
  if (!inputs.b)
    JoystickReport->button.circle = 1;
  if (!inputs.c)
    JoystickReport->button.square = 1;
}

inline void read_six_buttons_frames_internal(USB_JoystickReport_Data_t* JoystickReport) {
  input_defs inputs;
  inputs.raw = IO_PIN;
  set_select_high();
  if (!inputs.a)
    JoystickReport->button.cross = 1;
  if (!inputs.start)
    JoystickReport->button.start = 1;
  
  inputs.raw = PINB;
  set_select_low();
  set_directions(JoystickReport, inputs);
  if (!inputs.b)
    JoystickReport->button.square = 1;
  if (!inputs.c)
    JoystickReport->button.r1 = 1;
}

inline void read_six_buttons_frames(USB_JoystickReport_Data_t* JoystickReport, input_defs inputs) {
  set_select_high();
  if (!inputs.a)
    JoystickReport->button.cross = 1;
  if (!inputs.start)
    JoystickReport->button.start = 1;
  
  inputs.raw = PINB;
  set_select_low();
  if (!inputs.x)
    JoystickReport->button.circle = 1;
  if (!inputs.y)
    JoystickReport->button.triangle = 1;
  if (!inputs.z)
    JoystickReport->button.l1 = 1;
  //if (!inputs.mode)
  //  JoystickReport->button.select = 1;
  
  read_six_buttons_frames_internal(JoystickReport);
  read_six_buttons_frames_internal(JoystickReport);
  read_six_buttons_frames_internal(JoystickReport);
}


int main(void) {
  SetupHardware();
  
  GlobalInterruptEnable();
  
  for (;;) {
    HID_Device_USBTask(&Joystick_HID_Interface);
    USB_USBTask();
  }
}


void SetupHardware(void) {
  /* Disable watchdog if enabled by bootloader/fuses */
  MCUSR &= ~(1 << WDRF);
  wdt_disable();
  
  /* Disable clock division */
  clock_prescale_set(clock_div_1);
  
  /* Hardware Initialization */
  IO_DDR = OUTPUT_PINS_MASK;
  IO_PORT = INPUT_PINS_MASK;
  
  set_select_low();
  SerialDebug_init();
  USB_Init();
}

void EVENT_USB_Device_ConfigurationChanged(void) {
  bool ConfigSuccess = true;
  
  SerialDebug_printf("EVENT_USB_Device_ConfigurationChanged\r\n");
  ConfigSuccess &= HID_Device_ConfigureEndpoints(&Joystick_HID_Interface);
  
  USB_Device_EnableSOFEvents();
}

void EVENT_USB_Device_ControlRequest(void) {
  SerialDebug_printf("EVENT_USB_Device_ControlRequest\r\n");
  HID_Device_ProcessControlRequest(&Joystick_HID_Interface);
}

void EVENT_USB_Device_StartOfFrame(void) {
  //SerialDebug_printf("EVENT_USB_Device_StartOfFrame\r\n");
  HID_Device_MillisecondElapsed(&Joystick_HID_Interface);
}

bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
  USB_JoystickReport_Data_t* JoystickReport = (USB_JoystickReport_Data_t*)ReportData;
  input_defs inputs;
  
  set_select_low();
  JoystickReport->Y = 0;
  JoystickReport->X = 0;
  JoystickReport->button.raw[0] = 0;
  JoystickReport->button.raw[1] = 0;
  
  inputs.raw = PINB;
  if (is_six_buttons_Frame(inputs)) {
    // Most likely 6 buttons megadrive/genesis controller
    read_six_buttons_frames(JoystickReport, inputs);
  } else if (is_three_buttons_Frame(inputs)) {
    // Most likely 3 buttons megadrive/genesis controller
    read_three_buttons_frames(JoystickReport, inputs);
  } else {
    // Most likely MS controller.
    read_two_buttons_frame(JoystickReport, inputs);
  }
  
  *ReportSize = sizeof(USB_JoystickReport_Data_t);
  return false;
}

void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
  SerialDebug_printf("CALLBACK_HID_Device_ProcessHIDReport\r\n");
  SerialDebug_printf("%#0x, %#0x\r\n", ReportID, ReportType);
  SerialDebug_printByteArray(ReportData, ReportSize);
  
  // Unused (but mandatory for the HID class driver) in this demo, since there are no Host->Device reports
}
