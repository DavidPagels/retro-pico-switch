#ifndef SwitchBluetooth_h
#define SwitchBluetooth_h

#include "Controller.h"
#include "SwitchConsts.h"
#include "btstack.h"

extern Controller *globalController;

typedef enum {
  REQUEST_DEVICE_INFO = 0x02,
  SET_SHIPMENT = 0x08,
  SPI_READ = 0x10,
  SET_MODE = 0x03,
  TRIGGER_BUTTONS = 0x04,
  TOGGLE_IMU = 0x40,
  ENABLE_VIBRATION = 0x48,
  SET_PLAYER = 0x30,
  SET_NFC_IR_STATE = 0x22,
  SET_NFC_IR_CONFIG = 0x21
} SwitchRequest;

static const uint8_t VIB_OPTS[4] = {0xa0, 0xb0, 0xc0, 0x90};

class SwitchBluetooth {
 public:
  static void init(Controller *controller);

 private:
  static void generate_report();
  static void packet_handler(uint8_t packet_type, uint16_t channel,
                             uint8_t *packet, uint16_t packet_size);
  static void hid_report_data_callback(uint16_t cid,
                                       hid_report_type_t report_type,
                                       uint16_t report_id, int report_size,
                                       uint8_t *report);
  static void set_empty_report();
  static void set_subcommand_reply();
  static void set_unknown_subcommand(uint8_t subcommand_id);
  static void set_timer();
  static void set_full_input_report();
  static void set_standard_input_report();
  static void set_device_info();
  static void set_shipment();
  static void toggle_imu();
  static void set_imu_data();
  static void spi_read();
  static void set_mode();
  static void set_trigger_buttons();
  static void enable_vibration();
  static void set_player_lights();
  static void set_nfc_ir_state();
  static void set_nfc_ir_config();
  static bd_addr_t _addr;
  static uint8_t _report[100];
  static uint8_t _switchRequestReport[100];
  static SwitchBtReport _switchBtReport;
  static bool _vibration_enabled;
  static uint8_t _vibration_report;
  static bool _imu_enabled;
  static uint8_t _player_number;
  static bool _device_info_queried;
  static uint32_t _timer;
  static uint32_t _timestamp;
};

#define SWITCH_ENDPOINT_SIZE 64

#endif