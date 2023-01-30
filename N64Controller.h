#ifndef N64Controller_h
#define N64Controller_h

class N64Controller {
  public:
    N64Controller(int n64Pin);
    void init();
    void sendByte(uint8_t byte);
    uint32_t getState();
  private:
    void emptyRxFifo();
    int _n64Pin;
    uint sm;
};

#define N64_DPAD_OFFSET 24
#define N64_C_OFFSET    16

// Button report (16 bits)
#define N64_MASK_A      (0x1 << 31)
#define N64_MASK_B      (0x1 << 30)
#define N64_MASK_Z      (0x1 << 29)
#define N64_MASK_START  (0x1 << 28)
#define N64_MASK_DPAD   (0xF << N64_DPAD_OFFSET)
#define N64_MASK_RESET  (0x1 << 23)
#define N64_MASK_L      (0x1 << 21)
#define N64_MASK_R      (0x1 << 20)
#define N64_MASK_C      (0xF << N64_C_OFFSET)

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

#define N64_X_OFFSET      8
#define N64_Y_OFFSET      0
#endif
