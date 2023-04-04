
#include "GamecubeController.h"

#include "Controller.pio.h"
#include "pico/stdlib.h"

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
  busy_wait_us(500);
}

void GamecubeController::getSwitchUsbReport(SwitchUsbReport *switchUsbReport) {
  updateState();
  switchUsbReport->hat = SWITCH_USB_HAT_NOTHING;
  switchUsbReport->rx = SWITCH_USB_JOYSTICK_MID;
  switchUsbReport->ry = SWITCH_USB_JOYSTICK_MID;
  switchUsbReport->lx = SWITCH_USB_JOYSTICK_MID;
  switchUsbReport->ly = SWITCH_USB_JOYSTICK_MID;

  switchUsbReport->buttons =
      (GC_MASK_START & _controllerState[0] ? SWITCH_USB_MASK_PLUS : 0) |
      (GC_MASK_Y & _controllerState[0] ? SWITCH_USB_MASK_Y : 0) |
      (GC_MASK_X & _controllerState[0] ? SWITCH_USB_MASK_X : 0) |
      (GC_MASK_B & _controllerState[0] ? SWITCH_USB_MASK_B : 0) |
      (GC_MASK_A & _controllerState[0] ? SWITCH_USB_MASK_A : 0) |
      (GC_MASK_L & _controllerState[1] ? SWITCH_USB_MASK_L : 0) |
      (GC_MASK_R & _controllerState[1] ? SWITCH_USB_MASK_R : 0) |
      (!(GC_MASK_R & _controllerState[1]) && _controllerState[7] > 0x30
           ? SWITCH_USB_MASK_ZR
           : 0) |
      (!(GC_MASK_L & _controllerState[1]) && _controllerState[6] > 0x30
           ? SWITCH_USB_MASK_ZL
           : 0) |
      (GC_MASK_Z & _controllerState[1] ? SWITCH_USB_MASK_L : 0) |
      ((GC_MASK_L & _controllerState[1]) && (GC_MASK_R & _controllerState[1]) &&
               (GC_MASK_START & _controllerState[0])
           ? SWITCH_USB_MASK_HOME
           : 0);

  switch (GC_MASK_DPAD & _controllerState[1]) {
    case GC_MASK_DPAD_UP:
      switchUsbReport->hat = SWITCH_USB_HAT_UP;
      break;
    case GC_MASK_DPAD_UPRIGHT:
      switchUsbReport->hat = SWITCH_USB_HAT_UPRIGHT;
      break;
    case GC_MASK_DPAD_RIGHT:
      switchUsbReport->hat = SWITCH_USB_HAT_RIGHT;
      break;
    case GC_MASK_DPAD_DOWNRIGHT:
      switchUsbReport->hat = SWITCH_USB_HAT_DOWNRIGHT;
      break;
    case GC_MASK_DPAD_DOWN:
      switchUsbReport->hat = SWITCH_USB_HAT_DOWN;
      break;
    case GC_MASK_DPAD_DOWNLEFT:
      switchUsbReport->hat = SWITCH_USB_HAT_DOWNLEFT;
      break;
    case GC_MASK_DPAD_LEFT:
      switchUsbReport->hat = SWITCH_USB_HAT_LEFT;
      break;
    case GC_MASK_DPAD_UPLEFT:
      switchUsbReport->hat = SWITCH_USB_HAT_UPLEFT;
      break;
  }

  // Scale for joystick insensitivity if needed
  // https://GCsquid.com/GC-joystick-360-degrees/ GC Y axis is inverted relative
  // to Switch
  switchUsbReport->lx = convertToSwitchUsbJoystick(_controllerState[2],
                                                   &_minAnalogX, &_maxAnalogX);
  switchUsbReport->ly = convertToSwitchUsbJoystick(
      GC_JOYSTICK_MAX - _controllerState[3], &_minAnalogY, &_maxAnalogY);
  switchUsbReport->rx =
      convertToSwitchUsbJoystick(_controllerState[4], &_minCX, &_maxCX);
  switchUsbReport->ry = convertToSwitchUsbJoystick(
      GC_JOYSTICK_MAX - _controllerState[5], &_minCY, &_maxCY);
  return;
}

uint16_t GamecubeController::convertToSwitchUsbJoystick(uint8_t axisPos,
                                                        double *minAxis,
                                                        double *maxAxis) {
  double unscaledAxisPos =
      (axisPos - GC_JOYSTICK_MID) / (double)GC_JOYSTICK_MID;
  double scaledAxisPos = getScaledAnalogAxis(unscaledAxisPos, minAxis, maxAxis);
  return scaledAxisPos * SWITCH_USB_JOYSTICK_MID + SWITCH_USB_JOYSTICK_MID - 1;
}

void GamecubeController::getSwitchBtReport(SwitchBtReport *switchBtReport) {
  updateState();

  switchBtReport->buttons[0] =
      (!(GC_MASK_R & _controllerState[1]) && _controllerState[7] > 0x30
           ? SWITCH_BT_MASK_ZR
           : 0) |
      (GC_MASK_R & _controllerState[1] ? SWITCH_BT_MASK_R : 0) |
      (GC_MASK_A & _controllerState[0] ? SWITCH_BT_MASK_A : 0) |
      (GC_MASK_B & _controllerState[0] ? SWITCH_BT_MASK_B : 0) |
      (GC_MASK_X & _controllerState[0] ? SWITCH_BT_MASK_X : 0) |
      (GC_MASK_Y & _controllerState[0] ? SWITCH_BT_MASK_Y : 0);

  switchBtReport->buttons[1] =
      ((GC_MASK_L & _controllerState[1]) && (GC_MASK_R & _controllerState[1]) &&
               (GC_MASK_START & _controllerState[0])
           ? SWITCH_BT_MASK_HOME
           : 0) |
      (GC_MASK_START & _controllerState[1] ? SWITCH_BT_MASK_PLUS : 0);

  switchBtReport->buttons[2] = 0x00;
  switch (GC_MASK_DPAD & _controllerState[1]) {
    case GC_MASK_DPAD_UP:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_UP;
      break;
    case GC_MASK_DPAD_UPRIGHT:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_UPRIGHT;
      break;
    case GC_MASK_DPAD_RIGHT:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_RIGHT;
      break;
    case GC_MASK_DPAD_DOWNRIGHT:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_DOWNRIGHT;
      break;
    case GC_MASK_DPAD_DOWN:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_DOWN;
      break;
    case GC_MASK_DPAD_DOWNLEFT:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_DOWNLEFT;
      break;
    case GC_MASK_DPAD_LEFT:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_LEFT;
      break;
    case GC_MASK_DPAD_UPLEFT:
      switchBtReport->buttons[2] |= SWITCH_BT_HAT_UPLEFT;
      break;
  }

  switchBtReport->buttons[2] |=
      (GC_MASK_L & _controllerState[1] ? SWITCH_BT_MASK_L : 0) |
      (!(GC_MASK_L & _controllerState[1]) && _controllerState[6] > 0x30
           ? SWITCH_BT_MASK_ZL
           : 0) |
      (GC_MASK_Z & _controllerState[1] ? SWITCH_BT_MASK_L : 0);

  // Scale for joystick insensitivity if needed
  uint16_t lx = convertToSwitchBtJoystick(_controllerState[2], &_minAnalogX,
                                          &_maxAnalogX);
  uint16_t ly = convertToSwitchBtJoystick(_controllerState[3], &_minAnalogY,
                                          &_maxAnalogY);
  switchBtReport->l[0] = lx & 0xff;
  switchBtReport->l[1] = ((ly & 0xf) << 4) | (lx >> 8);
  switchBtReport->l[2] = ly >> 4;

  uint16_t rx =
      convertToSwitchBtJoystick(_controllerState[4], &_minCX, &_maxCX);
  uint16_t ry =
      convertToSwitchBtJoystick(_controllerState[5], &_minCY, &_maxCY);
  switchBtReport->r[0] = rx & 0xff;
  switchBtReport->r[1] = ((ry & 0xf) << 4) | (rx >> 8);
  switchBtReport->r[2] = ry >> 4;

  return;
}

uint16_t GamecubeController::convertToSwitchBtJoystick(uint8_t axisPos,
                                                       double *minAxis,
                                                       double *maxAxis) {
  double unscaledAxisPos =
      (axisPos - GC_JOYSTICK_MID) / (double)GC_JOYSTICK_MID;
  double scaledAxisPos = getScaledAnalogAxis(unscaledAxisPos, minAxis, maxAxis);
  return scaledAxisPos * SWITCH_BT_JOYSTICK_MID + SWITCH_BT_JOYSTICK_MID - 1;
}