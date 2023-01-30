/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "tusb.h"
#include "SwitchDescriptors.h"
#include <string.h>

static SwitchReport switchReport {
	.buttons = 0,
	.hat = SWITCH_HAT_NOTHING,
	.lx = SWITCH_JOYSTICK_MID,
	.ly = SWITCH_JOYSTICK_MID,
	.rx = SWITCH_JOYSTICK_MID,
	.ry = SWITCH_JOYSTICK_MID,
	.vendor = 0,
};

SwitchReport *SwitchDescriptors::getSwitchReport(uint32_t n64State) {
    switchReport.hat = SWITCH_HAT_NOTHING;
    switchReport.rx = SWITCH_JOYSTICK_MID;
    switchReport.ry = SWITCH_JOYSTICK_MID;
    
    switchReport.buttons = 
      (N64_MASK_B & n64State ? SWITCH_MASK_B : 0) |
      (N64_MASK_A & n64State ? SWITCH_MASK_A : 0) |
      (N64_MASK_RESET & n64State ? SWITCH_MASK_HOME : 0) |
      (N64_MASK_L & n64State ? SWITCH_MASK_L : 0) |
      (N64_MASK_R & n64State ? SWITCH_MASK_R : 0) |
      (N64_MASK_Z & n64State ? SWITCH_MASK_ZL : 0) |
      (N64_MASK_START & n64State ? SWITCH_MASK_PLUS : 0);

    switch ((N64_MASK_DPAD & n64State) >> N64_DPAD_OFFSET) {
      case N64_MASK_DPAD_UP:
        switchReport.hat = SWITCH_HAT_UP;
        break;
      case N64_MASK_DPAD_UPRIGHT:
        switchReport.hat = SWITCH_HAT_UPRIGHT;
        break;
      case N64_MASK_DPAD_RIGHT:
        switchReport.hat = SWITCH_HAT_RIGHT;
        break;
      case N64_MASK_DPAD_DOWNRIGHT:
        switchReport.hat = SWITCH_HAT_DOWNRIGHT;
        break;
      case N64_MASK_DPAD_DOWN:
        switchReport.hat = SWITCH_HAT_DOWN;
        break;
      case N64_MASK_DPAD_DOWNLEFT:
        switchReport.hat = SWITCH_HAT_DOWNLEFT;
        break;
      case N64_MASK_DPAD_LEFT:
        switchReport.hat = SWITCH_HAT_LEFT;
        break;
      case N64_MASK_DPAD_UPLEFT:
        switchReport.hat = SWITCH_HAT_UPLEFT;
        break;
    }

    switch((N64_MASK_C & n64State) >> N64_C_OFFSET) {
      case N64_MASK_C_UP:
        switchReport.ry = SWITCH_JOYSTICK_MIN;
        break;
      case N64_MASK_C_DOWN:
        switchReport.ry = SWITCH_JOYSTICK_MAX;
        break;
      case N64_MASK_C_LEFT:
        switchReport.rx = SWITCH_JOYSTICK_MIN;
        break;
      case N64_MASK_C_RIGHT:
        switchReport.rx = SWITCH_JOYSTICK_MAX;
        break;
    }

    // scale for joystick insensitivity if needed https://n64squid.com/n64-joystick-360-degrees/
    int8_t xPos = (n64State >> N64_X_OFFSET) & 0xFF;
    maxX = xPos > maxX ? xPos : maxX;
    minX = xPos < minX ? xPos : minX;
    uint8_t scaledX = xPos > 0 ? xPos * (SWITCH_JOYSTICK_MID - 1) / maxX : xPos * -SWITCH_JOYSTICK_MID / minX;
    switchReport.lx = SWITCH_JOYSTICK_MID + scaledX;

    int8_t yPos = -((n64State >> N64_Y_OFFSET) & 0xFF);
    maxY = yPos > maxY ? yPos : maxY;
    minY = yPos < minY ? yPos : minY;
    uint8_t scaledY = yPos > 0 ? yPos * (SWITCH_JOYSTICK_MID - 1) / maxY : yPos * -SWITCH_JOYSTICK_MID / minY;
    switchReport.ly = SWITCH_JOYSTICK_MID + scaledY;
    
    return &switchReport;
}

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void) {
  return switch_device_descriptor;
}

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void) langid;

  uint8_t chr_count;

  if ( index == 0) {
    memcpy(&_desc_str[1], switch_string_descriptors[0], 2);
    chr_count = 1;
  } else {
    // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

    if ( !(index < sizeof(switch_string_descriptors)/sizeof(switch_string_descriptors[0])) ) return NULL;

    const char* str = (char *)switch_string_descriptors[index];

    // Cap at max char
    chr_count = (uint8_t) strlen(str);
    if ( chr_count > 31 ) chr_count = 31;

    // Convert ASCII string into UTF-16
    for(uint8_t i=0; i<chr_count; i++) {
      _desc_str[1+i] = str[i];
    }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (uint16_t) ((0x03 << 8 ) | (2*chr_count + 2));

  return _desc_str;
}

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_hid_descriptor_report_cb(uint8_t itf) {
  (void) itf;
  return switch_report_descriptor;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
  (void) index; // for multiple configurations
  return switch_configuration_descriptor;
}

uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
	// TODO: Handle the correct report type, if required
	(void)itf;
	(void)report_id;
	(void)report_type;
	(void)reqlen;

	uint8_t report_size = 0;
	SwitchReport switch_report;
	report_size = sizeof(SwitchReport);
	memcpy(buffer, &switch_report, report_size);
	return report_size;
}

void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
	(void) itf;

	// echo back anything we received from host
	tud_hid_report(report_id, buffer, bufsize);
}


