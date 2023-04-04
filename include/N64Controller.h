#ifndef N64Controller_h
#define N64Controller_h

#include "Controller.h"

class N64Controller : public Controller {
 public:
  N64Controller(uint8_t pin) : Controller(pin, 4){};
  void init();
  void getSwitchUsbReport(SwitchUsbReport *switchUsbReport);
  uint16_t convertToSwitchUsbJoystick(int8_t axisPos, double *minAxis,
                                      double *maxAxis);
  void getSwitchBtReport(SwitchBtReport *switchBtReport);
  uint16_t convertToSwitchBtJoystick(int8_t axisPos, double *minAxis,
                                     double *maxAxis);

 private:
  void updateState();
  double _maxX = 0.5;
  double _minX = -0.5;
  double _maxY = 0.5;
  double _minY = -0.5;
};

#define N64_JOYSTICK_MAX 0x80
#define N64_C_OFFSET 16

// First byte
#define N64_MASK_A (0x1 << 7)
#define N64_MASK_B (0x1 << 6)
#define N64_MASK_Z (0x1 << 5)
#define N64_MASK_START (0x1 << 4)
#define N64_MASK_DPAD 0xF
#define N64_MASK_RESET (0x1 << 7)
#define N64_MASK_L (0x1 << 5)
#define N64_MASK_R (0x1 << 4)
#define N64_MASK_C 0xF

#define N64_MASK_DPAD_UP 0x8
#define N64_MASK_DPAD_UPRIGHT 0x9
#define N64_MASK_DPAD_RIGHT 0x1
#define N64_MASK_DPAD_DOWNRIGHT 0x5
#define N64_MASK_DPAD_DOWN 0x4
#define N64_MASK_DPAD_DOWNLEFT 0x6
#define N64_MASK_DPAD_LEFT 0x2
#define N64_MASK_DPAD_UPLEFT 0xA

#define N64_MASK_C_UP 0x8
#define N64_MASK_C_DOWN 0x4
#define N64_MASK_C_LEFT 0x2
#define N64_MASK_C_RIGHT 0x1

#endif
