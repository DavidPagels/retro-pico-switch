#ifndef SwitchBluetooth_h
#define SwitchBluetooth_h

#include "SwitchCommon.h"
#include "SwitchConsts.h"
#include "btstack.h"

class SwitchBluetooth : public SwitchCommon {
 public:
  void init(Controller *controller);
  void setHidCid(uint16_t hid_cid) { _hid_cid = hid_cid; };
  uint16_t getHidCid() { return _hid_cid; };

 private:
  uint16_t _hid_cid = 0;
};

void packet_handler(SwitchBluetooth *inst, uint8_t packet_type,
                    uint8_t *packet);

#endif