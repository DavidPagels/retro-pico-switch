
#define __BTSTACK_FILE__ "SwitchBluetooth.cpp"

#include "SwitchBluetooth.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "BtStackUtils.h"
#include "btstack.h"
#include "btstack_event.h"
#include "btstack_run_loop.h"
#include "pico/cyw43_arch.h"
#include "pico/rand.h"
#include "pico/stdlib.h"

Controller *globalController;
static uint8_t hid_service_buffer[700];
static uint8_t pnp_service_buffer[200];
static const char hid_device_name[] = "Wireless Gamepad";
static btstack_packet_callback_registration_t hci_event_callback_registration;
static uint16_t hid_cid;
bool SwitchBluetooth::_imu_enabled = false;
bool SwitchBluetooth::_vibration_enabled = false;
uint8_t SwitchBluetooth::_vibration_report = 0x00;
uint8_t SwitchBluetooth::_player_number = 0x00;
uint32_t SwitchBluetooth::_timer = 0;
uint32_t SwitchBluetooth::_timestamp = 0;
bool SwitchBluetooth::_device_info_queried = false;
uint8_t SwitchBluetooth::_report[100] = {0x00};
uint8_t SwitchBluetooth::_switchRequestReport[100] = {0x00};
bd_addr_t SwitchBluetooth::_addr = {0x7c,
                                    0xbb,
                                    0x8a,
                                    (uint8_t)(get_rand_32() % 0xff),
                                    (uint8_t)(get_rand_32() % 0xff),
                                    (uint8_t)(get_rand_32() % 0xff)};
SwitchBtReport SwitchBluetooth::_switchBtReport = {
    .batteryConnection = 0x90, .buttons = {0x0}, .l = {0x0}, .r = {0x0}};

void SwitchBluetooth::init(Controller *controller) {
  globalController = controller;
  if (cyw43_arch_init()) {
    printf("failed to initialise cyw43_arch\n");
    return;
  }

  gap_discoverable_control(1);
  gap_set_class_of_device(0x2508);
  gap_set_local_name("Pro Controller");
  gap_set_default_link_policy_settings(LM_LINK_POLICY_ENABLE_ROLE_SWITCH |
                                       LM_LINK_POLICY_ENABLE_SNIFF_MODE);
  gap_set_allow_role_switch(true);

  hci_set_chipset(btstack_chipset_cyw43_instance());
  hci_set_bd_addr(_addr);

  // L2CAP
  l2cap_init();
  sm_init();
  // SDP Server
  sdp_init();

  hid_sdp_record_t hid_sdp_record = {0x2508,
                                     33,
                                     1,
                                     1,
                                     1,
                                     0,
                                     0,
                                     0xFFFF,
                                     0xFFFF,
                                     3200,
                                     switch_bt_report_descriptor,
                                     sizeof(switch_bt_report_descriptor),
                                     hid_device_name};

  // Register SDP services
  memset(hid_service_buffer, 0, sizeof(hid_service_buffer));
  create_sdp_hid_record(hid_service_buffer, 0x10000, &hid_sdp_record);
  sdp_register_service(hid_service_buffer);

  memset(pnp_service_buffer, 0, sizeof(pnp_service_buffer));
  create_sdp_pnp_record(pnp_service_buffer, 0x10001,
                        DEVICE_ID_VENDOR_ID_SOURCE_USB, 0x057E, 0x2009, 0x0001);
  sdp_register_service(pnp_service_buffer);

  // HID Device
  hid_device_init(1, sizeof(switch_bt_report_descriptor),
                  switch_bt_report_descriptor);

  // Register callbacks
  hci_event_callback_registration.callback = &packet_handler;
  hci_add_event_handler(&hci_event_callback_registration);

  hid_device_register_packet_handler(&packet_handler);
  hid_device_register_report_data_callback(&hid_report_data_callback);

  // turn on!
  hci_power_control(HCI_POWER_ON);
  btstack_run_loop_execute();
}

void SwitchBluetooth::hid_report_data_callback(uint16_t cid,
                                               hid_report_type_t report_type,
                                               uint16_t report_id,
                                               int report_size,
                                               uint8_t *report) {
  memcpy(_switchRequestReport, report, report_size);
}

void SwitchBluetooth::packet_handler(uint8_t packet_type, uint16_t channel,
                                     uint8_t *packet, uint16_t packet_size) {
  UNUSED(channel);
  UNUSED(packet_size);
  uint8_t status;
  if (packet_type != HCI_EVENT_PACKET) {
    return;
  }
  switch (packet[0]) {
    case HCI_EVENT_HID_META:
      switch (hci_event_hid_meta_get_subevent_code(packet)) {
        case HID_SUBEVENT_CONNECTION_OPENED:
          status = hid_subevent_connection_opened_get_status(packet);
          if (status) {
            // outgoing connection failed
            hid_cid = 0;
            return;
          }
          hid_cid = hid_subevent_connection_opened_get_hid_cid(packet);
          hid_device_request_can_send_now_event(hid_cid);
          break;
        case HID_SUBEVENT_CONNECTION_CLOSED:
          hid_cid = 0;
          break;
        case HID_SUBEVENT_GET_PROTOCOL_RESPONSE:
          break;
        case HID_SUBEVENT_CAN_SEND_NOW:
          generate_report();
          hid_device_request_can_send_now_event(hid_cid);
          break;
      }
      break;
  }
}

void SwitchBluetooth::generate_report() {
  set_empty_report();
  switch (_switchRequestReport[9]) {
    case REQUEST_DEVICE_INFO:
      _device_info_queried = true;
      set_subcommand_reply();
      set_device_info();
      break;
    case SET_SHIPMENT:
      set_subcommand_reply();
      set_shipment();
      break;
    case SPI_READ:
      set_subcommand_reply();
      spi_read();
      break;
    case SET_MODE:
      set_subcommand_reply();
      set_mode();
      break;
    case TRIGGER_BUTTONS:
      set_subcommand_reply();
      set_trigger_buttons();
      break;
    case TOGGLE_IMU:
      set_subcommand_reply();
      toggle_imu();
      break;
    case ENABLE_VIBRATION:
      set_subcommand_reply();
      enable_vibration();
      break;
    case SET_PLAYER:
      set_subcommand_reply();
      set_player_lights();
      break;
    case SET_NFC_IR_STATE:
      set_subcommand_reply();
      set_nfc_ir_state();
      break;
    case SET_NFC_IR_CONFIG:
      set_subcommand_reply();
      set_nfc_ir_config();
      break;
    default:
      set_full_input_report();
      break;
  }

  globalController->getSwitchBtReport(&_switchBtReport);
  memcpy(_report + 3, (uint8_t *)&_switchBtReport, sizeof(SwitchBtReport));

  hid_device_send_interrupt_message(hid_cid, (uint8_t *)&_report, 50);
  memset(_switchRequestReport, 0x00, sizeof(_switchRequestReport));
}

void SwitchBluetooth::set_empty_report() {
  memset(_report, 0x00, sizeof(_report));
  _report[0] = 0xa1;
}

void SwitchBluetooth::set_subcommand_reply() {
  // Input Report ID
  _report[1] = 0x21;

  // TODO: Find out what the vibrator byte is doing.
  // This is a hack in an attempt to semi-emulate
  // actions of the vibrator byte as it seems to change
  // when a subcommand reply is sent.
  _vibration_report = VIB_OPTS[rand() % 4];

  set_standard_input_report();
}

void SwitchBluetooth::set_unknown_subcommand(uint8_t subcommand_id) {
  // Set NACK
  _report[14];

  // Set unknown subcommand ID
  _report[15] = subcommand_id;
}

void SwitchBluetooth::set_timer() {
  // If the timer hasn't been set before
  if (_timestamp == 0) {
    _timestamp = to_ms_since_boot(get_absolute_time());
    _report[2] = 0x00;
    return;
  }

  // Get the time that has passed since the last timestamp
  // in milliseconds
  uint32_t now = to_ms_since_boot(get_absolute_time());
  uint32_t delta_t = (now - _timestamp);

  // Get how many ticks have passed in hex with overflow at 255
  // Joy-Con uses 4.96ms as the timer tick rate
  uint32_t elapsed_ticks = int(delta_t * 4);
  _timer = (_timer + elapsed_ticks) & 0xFF;

  _report[2] = _timer;
  _timestamp = now;
}

void SwitchBluetooth::set_full_input_report() {
  // Setting Report ID to full standard input report ID
  _report[1] = 0x30;
  set_standard_input_report();
  set_imu_data();
}

void SwitchBluetooth::set_standard_input_report() {
  set_timer();

  if (_device_info_queried) {
    _report[3] = 0x90;
    _report[4] = 0x00;
    _report[5] = 0x00;
    _report[6] = 0x00;

    _report[7] = 0x00;
    _report[8] = 0x08;
    _report[9] = 0x80;

    _report[10] = 0x16;
    _report[11] = 0xd8;
    _report[12] = 0x7d;

    _report[13] = _vibration_report;
    _device_info_queried = false;
  }
}

void SwitchBluetooth::set_device_info() {
  // ACK Reply
  _report[14] = 0x82;

  // Subcommand Reply
  _report[15] = 0x02;

  // Firmware version
  _report[16] = 0x03;
  _report[17] = 0x8B;

  // Controller ID
  _report[18] = 0x03;

  // Unknown Byte, always 2
  _report[19] = 0x02;

  // Controller Bluetooth Address
  memcpy(_report + 20, _addr, 6);

  // Unknown byte, always 1
  _report[26] = 0x01;

  // Controller colours location (read from SPI)
  _report[27] = 0x01;
}

void SwitchBluetooth::set_shipment() {
  // ACK Reply
  _report[14] = 0x80;

  // Subcommand reply
  _report[15] = 0x08;
}

void SwitchBluetooth::toggle_imu() {
  _imu_enabled = _switchRequestReport[10] == 0x01;

  // ACK Reply
  _report[14] = 0x80;

  // Subcommand reply
  _report[15] = 0x40;
}

void SwitchBluetooth::set_imu_data() {
  if (!_imu_enabled) {
    return;
  }

  uint8_t imu_data[49] = {0x75, 0xFD, 0xFD, 0xFF, 0x09, 0x10, 0x21, 0x00, 0xD5,
                          0xFF, 0xE0, 0xFF, 0x72, 0xFD, 0xF9, 0xFF, 0x0A, 0x10,
                          0x22, 0x00, 0xD5, 0xFF, 0xE0, 0xFF, 0x76, 0xFD, 0xFC,
                          0xFF, 0x09, 0x10, 0x23, 0x00, 0xD5, 0xFF, 0xE0, 0xFF};
  memcpy(_report + 14, imu_data, sizeof(imu_data));
}
void SwitchBluetooth::spi_read() {
  uint8_t addr_top = _switchRequestReport[11];
  uint8_t addr_bottom = _switchRequestReport[10];
  uint8_t read_length = _switchRequestReport[14];

  // ACK byte
  _report[14] = 0x90;

  // Subcommand reply
  _report[15] = 0x10;

  // Read address
  _report[16] = addr_bottom;
  _report[17] = addr_top;

  // Read length
  _report[20] = read_length;

  // Stick Parameters
  // Params are generally the same for all sticks
  // Notable difference is the deadzone (10%
  // Joy-Con vs 15% Pro Con)
  uint8_t params[18] = {0x0F, 0x30, 0x61,  // Unused
                        0x96, 0x30,
                        0xF3,               // Dead Zone/Range Ratio
                        0xD4, 0x14, 0x54,   // X/Y ?
                        0x41, 0x15, 0x54,   // X/Y ?
                        0xC7, 0x79, 0x9C,   // X/Y ?
                        0x33, 0x36, 0x63};  // X/Y ?

  // Serial Number read
  if (addr_top == 0x60 && addr_bottom == 0x00) {
    // Switch will take this as no serial number
    memset(_report + 21, 0xff, 16);
  } else if (addr_top == 0x60 && addr_bottom == 0x50) {
    // Body colour
    memset(_report + 21, 0x82, 3);
    // Buttons colour
    memset(_report + 24, 0x0f, 3);
    // Left/right grip colours (Pro controller)
    memset(_report + 27, 0xff, 7);
  } else if (addr_top == 0x60 && addr_bottom == 0x80) {
    // Six-Axis factory parameters
    _report[21] = 0x50;
    _report[22] = 0xFD;
    _report[23] = 0x00;
    _report[24] = 0x00;
    _report[25] = 0xC6;
    _report[26] = 0x0F;

    memcpy(_report + 27, params, sizeof(params));

    // Stick device parameters 2
  } else if (addr_top == 0x60 && addr_bottom == 0x98) {
    // Setting same params since controllers always
    // have duplicates of stick params 1 for stick params 2
    memcpy(_report + 21, params, sizeof(params));

    // User analog stick calibration
  } else if (addr_top == 0x80 && addr_bottom == 0x10) {
    // Fill report with null user calibration info
    memset(_report + 21, 0xff, 3);

    // Factory analog stick calibration
  } else if (addr_top == 0x60 && addr_bottom == 0x3D) {
    // Left/right stick calibration
    uint8_t l_calibration[9] = {0xBA, 0xF5, 0x62, 0x6F, 0xC8,
                                0x77, 0xED, 0x95, 0x5B};
    uint8_t r_calibration[9] = {0x16, 0xD8, 0x7D, 0xF2, 0xB5,
                                0x5F, 0x86, 0x65, 0x5E};

    // Left stick calibration

    memcpy(_report + 21, l_calibration, sizeof(l_calibration));

    // Right stick calibration
    memcpy(_report + 30, r_calibration, sizeof(r_calibration));

    // Spacer byte
    _report[39] = 0xFF;

    // Body colour
    memset(_report + 40, 0x82, 3);
    // Buttons colour
    memset(_report + 43, 0x0f, 3);

    // Six-Axis motion sensor factor
    // calibration
  } else if (addr_top == 0x60 && addr_bottom == 0x20) {
    // 1: Acceleration origin position
    // 2: Acceleration sensitivity coefficient
    // 3: Gyro origin when still
    // 4: Gyro sensitivity coefficient
    uint8_t sa_calibration[24] = {0xD3, 0xFF, 0xD5, 0xFF, 0x55, 0x01,   // 1
                                  0x00, 0x40, 0x00, 0x40, 0x00, 0x40,   // 2
                                  0x19, 0x00, 0xDD, 0xFF, 0xDC, 0xFF,   // 3
                                  0x3B, 0x34, 0x3B, 0x34, 0x3B, 0x34};  // 4

    memcpy(_report + 21, sa_calibration, sizeof(sa_calibration));
  }
}

void SwitchBluetooth::set_mode() {
  // ACK byte
  _report[14] = 0x80;

  // Subcommand reply
  _report[15] = 0x03;
}

void SwitchBluetooth::set_trigger_buttons() {
  // ACK byte
  _report[14] = 0x83;

  // Subcommand reply
  _report[15] = 0x04;
}

void SwitchBluetooth::enable_vibration() {
  // ACK Reply
  _report[14] = 0x82;

  // Subcommand reply
  _report[15] = 0x48;

  // Set class property
  _vibration_enabled = true;
}

void SwitchBluetooth::set_player_lights() {
  // ACK byte
  _report[14] = 0x80;

  // Subcommand reply
  _report[15] = 0x30;

  uint8_t bitfield = _switchRequestReport[10];

  if (bitfield == 0x01 || bitfield == 0x10) {
    _player_number = 1;
  } else if (bitfield == 0x03 || bitfield == 0x30) {
    _player_number = 2;
  } else if (bitfield == 0x07 || bitfield == 0x70) {
    _player_number = 3;
  } else if (bitfield == 0x0F || bitfield == 0xF0) {
    _player_number = 4;
  }
}

void SwitchBluetooth::set_nfc_ir_state() {
  // ACK byte
  _report[14] = 0x80;

  // Subcommand reply
  _report[15] = 0x22;
}

void SwitchBluetooth::set_nfc_ir_config() {
  // ACK byte
  _report[14] = 0xA0;

  // Subcommand reply
  _report[15] = 0x21;

  // NFC/IR state data
  uint8_t params[8] = {0x01, 0x00, 0xFF, 0x00, 0x08, 0x00, 0x1B, 0x01};
  memcpy(_report + 16, params, sizeof(params));
  _report[49] = 0xC8;
}
