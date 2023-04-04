#include "N64Controller.h"

#include "Controller.pio.h"
#include "pico/stdlib.h"

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
  busy_wait_us(500);
}

void N64Controller::getSwitchUsbReport(SwitchUsbReport *switchUsbReport) {
  updateState();
  switchUsbReport->hat = SWITCH_USB_HAT_NOTHING;
  switchUsbReport->rx = SWITCH_USB_JOYSTICK_MID;
  switchUsbReport->ry = SWITCH_USB_JOYSTICK_MID;

  switchUsbReport->buttons =
      (N64_MASK_B & _controllerState[0] ? SWITCH_USB_MASK_B : 0) |
      (N64_MASK_A & _controllerState[0] ? SWITCH_USB_MASK_A : 0) |
      (N64_MASK_RESET & _controllerState[1] ? SWITCH_USB_MASK_HOME : 0) |
      (N64_MASK_L & _controllerState[1] ? SWITCH_USB_MASK_L : 0) |
      (N64_MASK_R & _controllerState[1] ? SWITCH_USB_MASK_R : 0) |
      (N64_MASK_Z & _controllerState[0] ? SWITCH_USB_MASK_ZL : 0) |
      (N64_MASK_START & _controllerState[0] ? SWITCH_USB_MASK_PLUS : 0);

  switch (N64_MASK_DPAD & _controllerState[0]) {
    case N64_MASK_DPAD_UP:
      switchUsbReport->hat = SWITCH_USB_HAT_UP;
      break;
    case N64_MASK_DPAD_UPRIGHT:
      switchUsbReport->hat = SWITCH_USB_HAT_UPRIGHT;
      break;
    case N64_MASK_DPAD_RIGHT:
      switchUsbReport->hat = SWITCH_USB_HAT_RIGHT;
      break;
    case N64_MASK_DPAD_DOWNRIGHT:
      switchUsbReport->hat = SWITCH_USB_HAT_DOWNRIGHT;
      break;
    case N64_MASK_DPAD_DOWN:
      switchUsbReport->hat = SWITCH_USB_HAT_DOWN;
      break;
    case N64_MASK_DPAD_DOWNLEFT:
      switchUsbReport->hat = SWITCH_USB_HAT_DOWNLEFT;
      break;
    case N64_MASK_DPAD_LEFT:
      switchUsbReport->hat = SWITCH_USB_HAT_LEFT;
      break;
    case N64_MASK_DPAD_UPLEFT:
      switchUsbReport->hat = SWITCH_USB_HAT_UPLEFT;
      break;
  }

  switch (N64_MASK_C & _controllerState[1]) {
    case N64_MASK_C_UP:
      switchUsbReport->ry = SWITCH_USB_JOYSTICK_MIN;
      break;
    case N64_MASK_C_DOWN:
      switchUsbReport->ry = SWITCH_USB_JOYSTICK_MAX;
      break;
    case N64_MASK_C_LEFT:
      switchUsbReport->rx = SWITCH_USB_JOYSTICK_MIN;
      break;
    case N64_MASK_C_RIGHT:
      switchUsbReport->rx = SWITCH_USB_JOYSTICK_MAX;
      break;
  }

  // scale for joystick insensitivity if needed
  // https://n64squid.com/n64-joystick-360-degrees/
  switchUsbReport->lx =
      convertToSwitchUsbJoystick(_controllerState[2], &_minX, &_maxX);
  switchUsbReport->ly =
      convertToSwitchUsbJoystick(-_controllerState[3], &_minY, &_maxY);
  return;
}

uint16_t N64Controller::convertToSwitchUsbJoystick(int8_t axisPos,
                                                   double *minAxis,
                                                   double *maxAxis) {
  double unscaledAxisPos = axisPos / (double)N64_JOYSTICK_MAX;
  double scaledAxisPos = getScaledAnalogAxis(unscaledAxisPos, minAxis, maxAxis);
  return scaledAxisPos * SWITCH_USB_JOYSTICK_MID + SWITCH_USB_JOYSTICK_MID - 1;
}

void N64Controller::getSwitchBtReport(SwitchBtReport *switchBtReport) {
  updateState();

  switchBtReport->buttons[0] =
      (N64_MASK_R & _controllerState[1] ? SWITCH_BT_MASK_R : 0) |
      (N64_MASK_A & _controllerState[0] ? SWITCH_BT_MASK_A : 0) |
      (N64_MASK_B & _controllerState[0] ? SWITCH_BT_MASK_B : 0);

  switchBtReport->buttons[1] =
      (N64_MASK_RESET & _controllerState[1] ? SWITCH_BT_MASK_HOME : 0) |
      (N64_MASK_START & _controllerState[0] ? SWITCH_BT_MASK_PLUS : 0);

  switchBtReport->buttons[2] = 0x00;
  switch (N64_MASK_DPAD & _controllerState[0]) {
    case N64_MASK_DPAD_UP:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_UP;
      break;
    case N64_MASK_DPAD_UPRIGHT:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_UPRIGHT;
      break;
    case N64_MASK_DPAD_RIGHT:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_RIGHT;
      break;
    case N64_MASK_DPAD_DOWNRIGHT:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_DOWNRIGHT;
      break;
    case N64_MASK_DPAD_DOWN:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_DOWN;
      break;
    case N64_MASK_DPAD_DOWNLEFT:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_DOWNLEFT;
      break;
    case N64_MASK_DPAD_LEFT:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_LEFT;
      break;
    case N64_MASK_DPAD_UPLEFT:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_UPLEFT;
      break;
  }

  switchBtReport->buttons[2] |=
      (N64_MASK_L & _controllerState[1] ? SWITCH_BT_MASK_L : 0) |
      (N64_MASK_Z & _controllerState[0] ? SWITCH_BT_MASK_ZL : 0);

  // Scale for joystick insensitivity if needed
  uint16_t lx = convertToSwitchBtJoystick(_controllerState[2], &_minX, &_maxX);
  uint16_t ly = convertToSwitchBtJoystick(_controllerState[3], &_minY, &_maxY);
  switchBtReport->l[0] = lx & 0xff;
  switchBtReport->l[1] = ((ly & 0xf) << 4) | (lx >> 8);
  switchBtReport->l[2] = ly >> 4;

  uint16_t rx = SWITCH_BT_JOYSTICK_MID;
  uint16_t ry = SWITCH_BT_JOYSTICK_MID;
  switch (N64_MASK_C & _controllerState[1]) {
    case N64_MASK_C_UP:
      ry = SWITCH_BT_JOYSTICK_MAX;
      break;
    case N64_MASK_C_DOWN:
      ry = SWITCH_BT_JOYSTICK_MIN;
      break;
    case N64_MASK_C_LEFT:
      rx = SWITCH_BT_JOYSTICK_MIN;
      break;
    case N64_MASK_C_RIGHT:
      rx = SWITCH_BT_JOYSTICK_MAX;
      break;
  }
  switchBtReport->r[0] = rx & 0xff;
  switchBtReport->r[1] = ((ry & 0xf) << 4) | (rx >> 8);
  switchBtReport->r[2] = ry >> 4;

  return;
}

uint16_t N64Controller::convertToSwitchBtJoystick(int8_t axisPos,
                                                  double *minAxis,
                                                  double *maxAxis) {
  double unscaledAxisPos = axisPos / (double)N64_JOYSTICK_MAX;
  double scaledAxisPos = getScaledAnalogAxis(unscaledAxisPos, minAxis, maxAxis);
  return scaledAxisPos * SWITCH_BT_JOYSTICK_MID + SWITCH_BT_JOYSTICK_MID - 1;
}
