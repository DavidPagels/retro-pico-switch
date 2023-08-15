
#define __BTSTACK_FILE__ "SwitchBluetooth.cpp"

#include "SwitchBluetooth.h"

#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "BtStackUtils.h"
#include "btstack.h"
#include "btstack_event.h"
#include "btstack_run_loop.h"
#include "pico/cyw43_arch.h"
#include "pico/rand.h"
#include "pico/stdlib.h"

static uint8_t hid_service_buffer[700];
static uint8_t pnp_service_buffer[200];
static const char hid_device_name[] = "Wireless Gamepad";

void SwitchBluetooth::init(Controller *controller) {
  _controller = controller;
  _switchReport.batteryConnection = 0x80;
  bd_addr_t newAddr;
  cyw43_hal_get_mac(0, (uint8_t*)&newAddr);
  newAddr[BD_ADDR_LEN - 1]++;
  memcpy(_addr, newAddr, 6);
  if (cyw43_arch_init()) {
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
  create_sdp_hid_record(hid_service_buffer, &hid_sdp_record);
  sdp_register_service(hid_service_buffer);

  memset(pnp_service_buffer, 0, sizeof(pnp_service_buffer));
  create_sdp_pnp_record(pnp_service_buffer, DEVICE_ID_VENDOR_ID_SOURCE_USB,
                        0x057E, 0x2009, 0x0001);
  sdp_register_service(pnp_service_buffer);

  // HID Device
  hid_device_init(1, sizeof(switch_bt_report_descriptor),
                  switch_bt_report_descriptor);
}

void packet_handler(SwitchBluetooth *inst, uint8_t packet_type,
                    uint8_t *packet) {
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
            inst->setHidCid(0);
            return;
          }
          inst->setHidCid(hid_subevent_connection_opened_get_hid_cid(packet));
          hid_device_request_can_send_now_event(inst->getHidCid());
          break;
        case HID_SUBEVENT_CONNECTION_CLOSED:
          inst->setHidCid(0);
          break;
        case HID_SUBEVENT_GET_PROTOCOL_RESPONSE:
          break;
        case HID_SUBEVENT_CAN_SEND_NOW:
          try {
            uint8_t *report = inst->generate_report();
            hid_device_send_interrupt_message(inst->getHidCid(), report, 50);
            inst->set_empty_switch_request_report();
            hid_device_request_can_send_now_event(inst->getHidCid());
          } catch (int e) {
            hid_device_request_can_send_now_event(inst->getHidCid());
          }
          break;
      }
      break;
  }
}
