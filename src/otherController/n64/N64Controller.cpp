#include "pico/stdlib.h"
#include "N64Controller.h"
#include "Controller.pio.h"

void N64Controller::init() {
    _pio = pio0;
    _offset = pio_add_program(_pio, &controller_program);
    _sm = pio_claim_unused_sm(_pio, true);
    pio_sm_config tmpConfig = controller_program_get_default_config(_offset);
    _c = &tmpConfig;
    controller_program_init(_pio, _sm, _offset, _pin, _c);
    unsigned char data[1] = {0x00};
    uint8_t response[3];
    sendData(data, 1, response, 1);
    sleep_us(200);
}

void N64Controller::updateState() {
    uint8_t data[1] = {0x01};
    sendData(data, 1, _controllerState, 4);
    sleep_us(500);
}

SwitchReport *N64Controller::getSwitchReport() {
    _switchReport.hat = SWITCH_HAT_NOTHING;
    _switchReport.rx = SWITCH_JOYSTICK_MID;
    _switchReport.ry = SWITCH_JOYSTICK_MID;
    
    _switchReport.buttons = 
      (N64_MASK_B & _controllerState[0] ? SWITCH_MASK_B : 0) |
      (N64_MASK_A & _controllerState[0] ? SWITCH_MASK_A : 0) |
      (N64_MASK_RESET & _controllerState[1] ? SWITCH_MASK_HOME : 0) |
      (N64_MASK_L & _controllerState[1] ? SWITCH_MASK_L : 0) |
      (N64_MASK_R & _controllerState[1] ? SWITCH_MASK_R : 0) |
      (N64_MASK_Z & _controllerState[0] ? SWITCH_MASK_ZL : 0) |
      (N64_MASK_START & _controllerState[0] ? SWITCH_MASK_PLUS : 0);

    switch (N64_MASK_DPAD & _controllerState[0]) {
      case N64_MASK_DPAD_UP:
        _switchReport.hat = SWITCH_HAT_UP;
        break;
      case N64_MASK_DPAD_UPRIGHT:
        _switchReport.hat = SWITCH_HAT_UPRIGHT;
        break;
      case N64_MASK_DPAD_RIGHT:
        _switchReport.hat = SWITCH_HAT_RIGHT;
        break;
      case N64_MASK_DPAD_DOWNRIGHT:
        _switchReport.hat = SWITCH_HAT_DOWNRIGHT;
        break;
      case N64_MASK_DPAD_DOWN:
        _switchReport.hat = SWITCH_HAT_DOWN;
        break;
      case N64_MASK_DPAD_DOWNLEFT:
        _switchReport.hat = SWITCH_HAT_DOWNLEFT;
        break;
      case N64_MASK_DPAD_LEFT:
        _switchReport.hat = SWITCH_HAT_LEFT;
        break;
      case N64_MASK_DPAD_UPLEFT:
        _switchReport.hat = SWITCH_HAT_UPLEFT;
        break;
    }

    switch(N64_MASK_C & _controllerState[1]) {
      case N64_MASK_C_UP:
        _switchReport.ry = SWITCH_JOYSTICK_MIN;
        break;
      case N64_MASK_C_DOWN:
        _switchReport.ry = SWITCH_JOYSTICK_MAX;
        break;
      case N64_MASK_C_LEFT:
        _switchReport.rx = SWITCH_JOYSTICK_MIN;
        break;
      case N64_MASK_C_RIGHT:
        _switchReport.rx = SWITCH_JOYSTICK_MAX;
        break;
    }

    // scale for joystick insensitivity if needed https://n64squid.com/n64-joystick-360-degrees/
    int8_t xPos = _controllerState[2];
    _maxX = xPos > _maxX ? xPos : _maxX;
    _minX = xPos < _minX ? xPos : _minX;
    uint8_t scaledX = xPos > 0 ? xPos * (SWITCH_JOYSTICK_MID - 1) / _maxX : xPos * -SWITCH_JOYSTICK_MID / _minX;
    _switchReport.lx = SWITCH_JOYSTICK_MID + scaledX;

    int8_t yPos = -_controllerState[3];
    _maxY = yPos > _maxY ? yPos : _maxY;
    _minY = yPos < _minY ? yPos : _minY;
    uint8_t scaledY = yPos > 0 ? yPos * (SWITCH_JOYSTICK_MID - 1) / _maxY : yPos * -SWITCH_JOYSTICK_MID / _minY;
    _switchReport.ly = SWITCH_JOYSTICK_MID + scaledY;
    
    return &_switchReport;
}
