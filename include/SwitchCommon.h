#ifndef SwitchCommon_h
#define SwitchCommon_h

#include "Controller.h"
#include "SwitchConsts.h"

typedef enum {
  BLUETOOTH_PAIR_REQUEST = 0x01,
  REQUEST_DEVICE_INFO = 0x02,
  SET_SHIPMENT = 0x08,
  SPI_READ = 0x10,
  SET_MODE = 0x03,
  TRIGGER_BUTTONS = 0x04,
  TOGGLE_IMU = 0x40,
  ENABLE_VIBRATION = 0x48,
  SET_PLAYER = 0x30,
  SET_NFC_IR_STATE = 0x22,
  SET_NFC_IR_CONFIG = 0x21,
  IMU_SENSITIVITY = 0x41
} SwitchRequest;

const uint8_t VIB_OPTS[4] = {0x0a, 0x0c, 0x0b, 0x09};

class SwitchCommon {
 public:
  virtual void init(Controller *controller) = 0;
  void setSwitchRequestReport(uint8_t *report, int report_size);
  void set_empty_switch_request_report();
  void set_controller_rumble(bool rumble);
  uint8_t *generate_report();

 protected:
  void set_empty_report();
  void set_subcommand_reply();
  void set_unknown_subcommand(uint8_t subcommand_id);
  void set_timer();
  void set_full_input_report();
  void set_standard_input_report();
  void set_bt();
  void set_device_info();
  void set_shipment();
  void toggle_imu();
  void imu_sensitivity();
  void set_imu_data();
  void spi_read();
  void set_mode();
  void set_trigger_buttons();
  void enable_vibration();
  void set_player_lights();
  void set_nfc_ir_state();
  void set_nfc_ir_config();
  SwitchReport _switchReport = {
      .batteryConnection = 0x91, .buttons = {0x0}, .l = {0x0}, .r = {0x0}};
  Controller *_controller;
  uint8_t _report[100] = {0x0};
  uint8_t _switchRequestReport[100] = {0x0};
  uint8_t _addr[6] = {0x0};
  bool _vibration_enabled = false;
  uint8_t _vibration_report = 0x00;
  uint8_t _vibration_idx = 0x00;
  bool _imu_enabled = false;
  uint8_t _player_number = 0x00;
  bool _device_info_queried = false;
  uint32_t _timer = 0;
  uint32_t _timestamp = 0;
};

void hid_report_data_callback(SwitchCommon *inst, uint16_t report_id,
                              uint8_t *report, int report_size);

#endif