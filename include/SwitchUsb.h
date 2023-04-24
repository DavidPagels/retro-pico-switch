/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#ifndef SwitchUsb_h
#define SwitchUsb_h

#include "SwitchCommon.h"
#include "SwitchConsts.h"

class SwitchUsb : public SwitchCommon {
 public:
  void init(Controller *controller);

 private:
  uint8_t *generate_usb_report();
};

#endif
