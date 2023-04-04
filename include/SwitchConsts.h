#ifndef SwitchConsts_h
#define SwitchConsts_h

#include "pico/stdlib.h"

typedef struct __attribute((packed, aligned(1))) {
  uint16_t buttons;
  uint8_t hat;
  uint8_t lx;
  uint8_t ly;
  uint8_t rx;
  uint8_t ry;
  uint8_t vendor;
} SwitchUsbReport;

// HAT report (4 bits)
#define SWITCH_USB_HAT_UP 0x00
#define SWITCH_USB_HAT_UPRIGHT 0x01
#define SWITCH_USB_HAT_RIGHT 0x02
#define SWITCH_USB_HAT_DOWNRIGHT 0x03
#define SWITCH_USB_HAT_DOWN 0x04
#define SWITCH_USB_HAT_DOWNLEFT 0x05
#define SWITCH_USB_HAT_LEFT 0x06
#define SWITCH_USB_HAT_UPLEFT 0x07
#define SWITCH_USB_HAT_NOTHING 0x08

// Button report (16 bits)
#define SWITCH_USB_MASK_Y (1U << 0)
#define SWITCH_USB_MASK_B (1U << 1)
#define SWITCH_USB_MASK_A (1U << 2)
#define SWITCH_USB_MASK_X (1U << 3)
#define SWITCH_USB_MASK_L (1U << 4)
#define SWITCH_USB_MASK_R (1U << 5)
#define SWITCH_USB_MASK_ZL (1U << 6)
#define SWITCH_USB_MASK_ZR (1U << 7)
#define SWITCH_USB_MASK_MINUS (1U << 8)
#define SWITCH_USB_MASK_PLUS (1U << 9)
#define SWITCH_USB_MASK_L3 (1U << 10)
#define SWITCH_USB_MASK_R3 (1U << 11)
#define SWITCH_USB_MASK_HOME (1U << 12)
#define SWITCH_USB_MASK_CAPTURE (1U << 13)

// Switch analog sticks only report 8 bits
#define SWITCH_USB_JOYSTICK_MIN 0x00
#define SWITCH_USB_JOYSTICK_MID 0x80
#define SWITCH_USB_JOYSTICK_MAX 0xFF

static const uint8_t switch_usb_string_language[] = {0x09, 0x04};
static const uint8_t switch_usb_string_manufacturer[] = "HORI CO.,LTD.";
static const uint8_t switch_usb_string_product[] = "POKKEN CONTROLLER";
static const uint8_t switch_usb_string_version[] = "1.0";

static const uint8_t *switch_usb_string_descriptors[] = {
    switch_usb_string_language, switch_usb_string_manufacturer,
    switch_usb_string_product, switch_usb_string_version};

static const uint8_t switch_usb_device_descriptor[] = {
    0x12,        // bLength
    0x01,        // bDescriptorType (Device)
    0x00, 0x02,  // bcdUSB 2.00
    0x00,  // bDeviceClass (Use class information in the Interface Descriptors)
    0x00,  // bDeviceSubClass
    0x00,  // bDeviceProtocol
    0x40,  // bMaxPacketSize0 64
    0x0D, 0x0F,  // idVendor 0x0F0D
    0x92, 0x00,  // idProduct 0x92
    0x00, 0x01,  // bcdDevice 2.00
    0x01,        // iManufacturer (String Index)
    0x02,        // iProduct (String Index)
    0x00,        // iSerialNumber (String Index)
    0x01,        // bNumConfigurations 1
};

static const uint8_t switch_usb_hid_descriptor[] = {
    0x09,        // bLength
    0x21,        // bDescriptorType (HID)
    0x11, 0x01,  // bcdHID 1.11
    0x00,        // bCountryCode
    0x01,        // bNumDescriptors
    0x22,        // bDescriptorType[0] (HID)
    0x56, 0x00,  // wDescriptorLength[0] 86
};

static const uint8_t switch_usb_configuration_descriptor[] = {
    0x09,        // bLength
    0x02,        // bDescriptorType (Configuration)
    0x29, 0x00,  // wTotalLength 41
    0x01,        // bNumInterfaces 1
    0x01,        // bConfigurationValue
    0x00,        // iConfiguration (String Index)
    0x80,        // bmAttributes
    0xFA,        // bMaxPower 500mA

    0x09,  // bLength
    0x04,  // bDescriptorType (Interface)
    0x00,  // bInterfaceNumber 0
    0x00,  // bAlternateSetting
    0x02,  // bNumEndpoints 2
    0x03,  // bInterfaceClass
    0x00,  // bInterfaceSubClass
    0x00,  // bInterfaceProtocol
    0x00,  // iInterface (String Index)

    0x09,        // bLength
    0x21,        // bDescriptorType (HID)
    0x11, 0x01,  // bcdHID 1.11
    0x00,        // bCountryCode
    0x01,        // bNumDescriptors
    0x22,        // bDescriptorType[0] (HID)
    0x56, 0x00,  // wDescriptorLength[0] 86

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x02,        // bEndpointAddress (OUT/H2D)
    0x03,        // bmAttributes (Interrupt)
    0x40, 0x00,  // wMaxPacketSize 64
    0x01,        // bInterval 1 (unit depends on device speed)

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x81,        // bEndpointAddress (IN/D2H)
    0x03,        // bmAttributes (Interrupt)
    0x40, 0x00,  // wMaxPacketSize 64
    0x01,        // bInterval 1 (unit depends on device speed)
};

static const uint8_t switch_usb_report_descriptor[] = {
    0x05, 0x01,  // Usage Page (Generic Desktop Ctrls)
    0x09, 0x05,  // Usage (Game Pad)
    0xA1, 0x01,  // Collection (Application)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0x01,  //   Logical Maximum (1)
    0x35, 0x00,  //   Physical Minimum (0)
    0x45, 0x01,  //   Physical Maximum (1)
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x10,  //   Report Count (16)
    0x05, 0x09,  //   Usage Page (Button)
    0x19, 0x01,  //   Usage Minimum (0x01)
    0x29, 0x10,  //   Usage Maximum (0x10)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
                 //   Position)
    0x05, 0x01,  //   Usage Page (Generic Desktop Ctrls)
    0x25, 0x07,  //   Logical Maximum (7)
    0x46, 0x3B, 0x01,  //   Physical Maximum (315)
    0x75, 0x04,        //   Report Size (4)
    0x95, 0x01,        //   Report Count (1)
    0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
    0x09, 0x39,        //   Usage (Hat switch)
    0x81, 0x42,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null
                 //   State)
    0x65, 0x00,  //   Unit (None)
    0x95, 0x01,  //   Report Count (1)
    0x81, 0x01,  //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x46, 0xFF, 0x00,  //   Physical Maximum (255)
    0x09, 0x30,        //   Usage (X)
    0x09, 0x31,        //   Usage (Y)
    0x09, 0x32,        //   Usage (Z)
    0x09, 0x35,        //   Usage (Rz)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x04,        //   Report Count (4)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
                 //   Position)
    0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
    0x09, 0x20,        //   Usage (0x20)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
                 //   Position)
    0x0A, 0x21, 0x26,  //   Usage (0x2621)
    0x95, 0x08,        //   Report Count (8)
    0x91, 0x02,  //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position,Non-volatile)
    0xC0,        // End Collection
};

typedef struct __attribute((packed, aligned(1))) {
  uint8_t batteryConnection;
  uint8_t buttons[3];
  uint8_t l[3];
  uint8_t r[3];
} SwitchBtReport;

// HAT report (4 bits)
#define SWITCH_BT_HAT_UP 0x2
#define SWITCH_BT_HAT_UPRIGHT 0x6
#define SWITCH_BT_HAT_RIGHT 0x4
#define SWITCH_BT_HAT_DOWNRIGHT 0x5
#define SWITCH_BT_HAT_DOWN 0x1
#define SWITCH_BT_HAT_DOWNLEFT 0x9
#define SWITCH_BT_HAT_LEFT 0x8
#define SWITCH_BT_HAT_UPLEFT 0xa
#define SWITCH_BT_HAT_NOTHING 0x0

// Button report (3 bytes)
#define SWITCH_BT_MASK_ZR (1U << 7)
#define SWITCH_BT_MASK_R (1U << 6)
#define SWITCH_BT_MASK_A (1U << 3)
#define SWITCH_BT_MASK_B (1U << 2)
#define SWITCH_BT_MASK_X (1U << 1)
#define SWITCH_BT_MASK_Y 1U

#define SWITCH_BT_MASK_CAPTURE (1U << 5)
#define SWITCH_BT_MASK_HOME (1U << 4)
#define SWITCH_BT_MASK_L3 (1U << 3)
#define SWITCH_BT_MASK_R3 (1U << 2)
#define SWITCH_BT_MASK_PLUS (1U << 1)
#define SWITCH_BT_MASK_MINUS 1U

#define SWITCH_BT_MASK_ZL (1U << 7)
#define SWITCH_BT_MASK_L (1U << 6)

// Switch analog sticks only report 8 bits
#define SWITCH_BT_JOYSTICK_MIN 0x000
#define SWITCH_BT_JOYSTICK_MID 0x800
#define SWITCH_BT_JOYSTICK_MAX 0xFFF

static const uint8_t switch_bt_report_descriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x05,        // Usage (Game Pad)
    0xA1, 0x01,        // Collection (Application)
    0x06, 0x01, 0xFF,  //   Usage Page (Vendor Defined 0xFF01)

    0x85, 0x21,  //   Report ID (33)
    0x09, 0x21,  //   Usage (0x21)
    0x75, 0x08,  //   Report Size (8)
    0x95, 0x30,  //   Report Count (48)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
                 //   Position)

    0x85, 0x30,  //   Report ID (48)
    0x09, 0x30,  //   Usage (0x30)
    0x75, 0x08,  //   Report Size (8)
    0x95, 0x30,  //   Report Count (48)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
                 //   Position)

    0x85, 0x31,        //   Report ID (49)
    0x09, 0x31,        //   Usage (0x31)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0x69, 0x01,  //   Report Count (361)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
                 //   Position)

    0x85, 0x32,        //   Report ID (50)
    0x09, 0x32,        //   Usage (0x32)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0x69, 0x01,  //   Report Count (361)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
                 //   Position)

    0x85, 0x33,        //   Report ID (51)
    0x09, 0x33,        //   Usage (0x33)
    0x75, 0x08,        //   Report Size (8)
    0x96, 0x69, 0x01,  //   Report Count (361)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
                 //   Position)

    0x85, 0x3F,  //   Report ID (63)
    0x05, 0x09,  //   Usage Page (Button)
    0x19, 0x01,  //   Usage Minimum (0x01)
    0x29, 0x10,  //   Usage Maximum (0x10)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0x01,  //   Logical Maximum (1)
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x10,  //   Report Count (16)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
                 //   Position)
    0x05, 0x01,  //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x39,  //   Usage (Hat switch)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0x07,  //   Logical Maximum (7)
    0x75, 0x04,  //   Report Size (4)
    0x95, 0x01,  //   Report Count (1)
    0x81, 0x42,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null
                 //   State)
    0x05, 0x09,  //   Usage Page (Button)
    0x75, 0x04,  //   Report Size (4)
    0x95, 0x01,  //   Report Count (1)
    0x81, 0x01,  //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position)
    0x05, 0x01,  //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,  //   Usage (X)
    0x09, 0x31,  //   Usage (Y)
    0x09, 0x33,  //   Usage (Rx)
    0x09, 0x34,  //   Usage (Ry)
    0x16, 0x00, 0x00,              //   Logical Minimum (0)
    0x27, 0xFF, 0xFF, 0x00, 0x00,  //   Logical Maximum (65534)
    0x75, 0x10,                    //   Report Size (16)
    0x95, 0x04,                    //   Report Count (4)
    0x81, 0x02,  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
                 //   Position)
    0x06, 0x01, 0xFF,  //   Usage Page (Vendor Defined 0xFF01)

    0x85, 0x01,  //   Report ID (1)
    0x09, 0x01,  //   Usage (0x01)
    0x75, 0x08,  //   Report Size (8)
    0x95, 0x30,  //   Report Count (48)
    0x91, 0x02,  //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position,Non-volatile)

    0x85, 0x10,  //   Report ID (16)
    0x09, 0x10,  //   Usage (0x10)
    0x75, 0x08,  //   Report Size (8)
    0x95, 0x30,  //   Report Count (48)
    0x91, 0x02,  //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position,Non-volatile)

    0x85, 0x11,  //   Report ID (17)
    0x09, 0x11,  //   Usage (0x11)
    0x75, 0x08,  //   Report Size (8)
    0x95, 0x30,  //   Report Count (48)
    0x91, 0x02,  //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position,Non-volatile)

    0x85, 0x12,  //   Report ID (18)
    0x09, 0x12,  //   Usage (0x12)
    0x75, 0x08,  //   Report Size (8)
    0x95, 0x30,  //   Report Count (48)
    0x91, 0x02,  //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No
                 //   Null Position,Non-volatile)
    0xC0,        // End Collection
};
#endif