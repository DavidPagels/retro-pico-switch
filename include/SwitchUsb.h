/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#ifndef SwitchUsb_h
#define SwitchUsb_h

#include <stdint.h>

#include "Controller.h"
#include "SwitchConsts.h"

class SwitchUsb {
 public:
  SwitchUsb(Controller *controller) { _controller = controller; };
  void init();

 private:
  Controller *_controller;
  SwitchUsbReport _switchUsbReport{.buttons = 0,
                                   .hat = SWITCH_USB_HAT_NOTHING,
                                   .lx = SWITCH_USB_JOYSTICK_MID,
                                   .ly = SWITCH_USB_JOYSTICK_MID,
                                   .rx = SWITCH_USB_JOYSTICK_MID,
                                   .ry = SWITCH_USB_JOYSTICK_MID,
                                   .vendor = 0};
};

#endif
