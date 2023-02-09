#ifndef GamecubeController_h
#define GamecubeController_h
#include <string.h>
#include "Controller.h"

#ifdef GC_CONTROLLER_TYPE
  #define gcControllerType GC_CONTROLLER_TYPE
#else
  #define gcControllerType "oneToOne"
#endif

class GamecubeController: public Controller {
  public:
    GamecubeController(uint8_t pin): Controller(pin, 10) {};
    void init();
    void updateState();
    SwitchReport *getSwitchReport();
  private:
    uint8_t getScaledAnalogAxis(uint8_t axisPos, uint8_t *minAxis, uint8_t *maxAxis);
    uint8_t _maxAnalogX = 0xC0;
		uint8_t _minAnalogX = 0x40;
		uint8_t _maxAnalogY = 0xC0;
		uint8_t _minAnalogY = 0x40;
    uint8_t _maxCX = 0xC0;
		uint8_t _minCX = 0x40;
		uint8_t _maxCY = 0xC0;
		uint8_t _minCY = 0x40;
    bool _isOneToOne = strcmp(gcControllerType, "oneToOne") == 0;
};

// GC First Byte
#define GC_MASK_A      (0x1)
#define GC_MASK_B      (0x1 << 1)
#define GC_MASK_X      (0x1 << 2)
#define GC_MASK_Y      (0x1 << 3)
#define GC_MASK_START  (0x1 << 4)

// GC Second Byte
#define GC_MASK_DPAD   (0xF)
#define GC_MASK_Z      (0x1 << 4)
#define GC_MASK_R      (0x1 << 5)
#define GC_MASK_L      (0x1 << 6)
#define GC_MASK_DPAD_UP        0x8
#define GC_MASK_DPAD_UPRIGHT   0xA
#define GC_MASK_DPAD_RIGHT     0x2
#define GC_MASK_DPAD_DOWNRIGHT 0x6
#define GC_MASK_DPAD_DOWN      0x4
#define GC_MASK_DPAD_DOWNLEFT  0x5
#define GC_MASK_DPAD_LEFT      0x1
#define GC_MASK_DPAD_UPLEFT    0x9

#endif
