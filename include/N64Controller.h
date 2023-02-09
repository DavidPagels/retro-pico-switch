#ifndef N64Controller_h
#define N64Controller_h

#include "Controller.h"

class N64Controller: public Controller {
  public:
    N64Controller(uint8_t pin): Controller(pin, 4) {};
    void init();
    void updateState();
    SwitchReport *getSwitchReport();
  private:
    int8_t _maxX = 60;
		int8_t _minX = -60;
		int8_t _maxY = 60;
		int8_t _minY = -60;

};

#define N64_C_OFFSET    16

// First byte
#define N64_MASK_A      (0x1 << 7)
#define N64_MASK_B      (0x1 << 6)
#define N64_MASK_Z      (0x1 << 5)
#define N64_MASK_START  (0x1 << 4)
#define N64_MASK_DPAD   0xF
#define N64_MASK_RESET  (0x1 << 7)
#define N64_MASK_L      (0x1 << 5)
#define N64_MASK_R      (0x1 << 4)
#define N64_MASK_C      0xF

#define N64_MASK_DPAD_UP        0x8
#define N64_MASK_DPAD_UPRIGHT   0x9
#define N64_MASK_DPAD_RIGHT     0x1
#define N64_MASK_DPAD_DOWNRIGHT 0x5
#define N64_MASK_DPAD_DOWN      0x4
#define N64_MASK_DPAD_DOWNLEFT  0x6
#define N64_MASK_DPAD_LEFT      0x2
#define N64_MASK_DPAD_UPLEFT    0xA

#define N64_MASK_C_UP     0x8
#define N64_MASK_C_DOWN   0x4
#define N64_MASK_C_LEFT   0x2
#define N64_MASK_C_RIGHT  0x1

#endif
