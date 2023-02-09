
#include "pico/stdlib.h"
#include "GamecubeController.h"
#include "Controller.pio.h"

void GamecubeController::init() {
    _pio = pio0;
    _offset = pio_add_program(_pio, &controller_program);
    _sm = pio_claim_unused_sm(_pio, true);
    pio_sm_config tmpConfig = controller_program_get_default_config(_offset);
    _c = &tmpConfig;
    controller_program_init(_pio, _sm, _offset, _pin, _c);
    uint8_t data[1] = {0x00};
    uint8_t response[3];
    sendData(data, 1, response, 3);
    sleep_us(200);
    updatePioOutputSize(24);
}

void GamecubeController::updateState() {
    uint8_t data[3] = {0x40, 0x03, 0x0};
    sendData(data, 3, _controllerState, 8);
    sleep_us(500);
}

uint8_t GamecubeController::getScaledAnalogAxis(uint8_t axisPos, uint8_t *minAxis, uint8_t *maxAxis) {
  *maxAxis = axisPos > *maxAxis ? axisPos : *maxAxis;
  *minAxis = axisPos < *minAxis ? axisPos : *minAxis;
  return axisPos > 0x80 ? 
    SWITCH_JOYSTICK_MID - 1 + (axisPos - SWITCH_JOYSTICK_MID) / (float)(*maxAxis - SWITCH_JOYSTICK_MID) * (float)SWITCH_JOYSTICK_MID : 
    SWITCH_JOYSTICK_MID - (SWITCH_JOYSTICK_MID - axisPos) / (float)(SWITCH_JOYSTICK_MID - *minAxis) * (float)SWITCH_JOYSTICK_MID;
}

SwitchReport *GamecubeController::getSwitchReport() {
    _switchReport.hat = SWITCH_HAT_NOTHING;
    _switchReport.rx = SWITCH_JOYSTICK_MID;
    _switchReport.ry = SWITCH_JOYSTICK_MID;
    _switchReport.lx = SWITCH_JOYSTICK_MID;
    _switchReport.ly = SWITCH_JOYSTICK_MID;
    
    _switchReport.buttons =
        (GC_MASK_START & _controllerState[0] ? SWITCH_MASK_PLUS : 0) |
        (GC_MASK_Y & _controllerState[0] ? (_isOneToOne ? SWITCH_MASK_Y : SWITCH_MASK_R3) : 0) |
        (GC_MASK_X & _controllerState[0] ? SWITCH_MASK_X : 0) |
        (GC_MASK_B & _controllerState[0] ? (_isOneToOne ? SWITCH_MASK_B : SWITCH_MASK_Y) : 0) |
        (GC_MASK_A & _controllerState[0] ? SWITCH_MASK_A : 0) |
        (GC_MASK_L & _controllerState[1] ? SWITCH_MASK_ZL : 0) |
        (GC_MASK_R & _controllerState[1] ? SWITCH_MASK_R : 0) |
        (!(GC_MASK_R & _controllerState[1]) && _controllerState[7] > 0x30 ? SWITCH_MASK_ZR : 0) |
        (!(GC_MASK_L & _controllerState[1]) && _controllerState[6] > 0x30 ? SWITCH_MASK_ZL : 0) |
        (GC_MASK_Z & _controllerState[1] ? SWITCH_MASK_L : 0) |
        ((GC_MASK_L & _controllerState[1]) && (GC_MASK_R & _controllerState[1]) && (GC_MASK_START & _controllerState[0]) ? SWITCH_MASK_HOME : 0);

    switch (GC_MASK_DPAD & _controllerState[1]) {
      case GC_MASK_DPAD_UP:
        _switchReport.hat = SWITCH_HAT_UP;
        break;
      case GC_MASK_DPAD_UPRIGHT:
        _switchReport.hat = SWITCH_HAT_UPRIGHT;
        break;
      case GC_MASK_DPAD_RIGHT:
        _switchReport.hat = SWITCH_HAT_RIGHT;
        break;
      case GC_MASK_DPAD_DOWNRIGHT:
        _switchReport.hat = SWITCH_HAT_DOWNRIGHT;
        break;
      case GC_MASK_DPAD_DOWN:
        _switchReport.hat = SWITCH_HAT_DOWN;
        break;
      case GC_MASK_DPAD_DOWNLEFT:
        _switchReport.hat = SWITCH_HAT_DOWNLEFT;
        break;
      case GC_MASK_DPAD_LEFT:
        _switchReport.hat = SWITCH_HAT_LEFT;
        break;
      case GC_MASK_DPAD_UPLEFT:
        _switchReport.hat = SWITCH_HAT_UPLEFT;
        break;
    }

    // Scale for joystick insensitivity if needed https://GCsquid.com/GC-joystick-360-degrees/
    // GC Y axis is inverted relative to Switch
    _switchReport.lx = getScaledAnalogAxis(_controllerState[2], &_minAnalogX, &_maxAnalogX);
    _switchReport.ly = getScaledAnalogAxis((1 - _controllerState[3] / 255.) * 255., &_minAnalogY, &_maxAnalogY);
    _switchReport.rx = getScaledAnalogAxis(_controllerState[4], &_minCX, &_maxCX);
    _switchReport.ry = getScaledAnalogAxis((1 - _controllerState[5] / 255.) * 255., &_minCY, &_maxCY);
    
    return &_switchReport;
}
