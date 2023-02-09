#ifndef Controller_h
#define Controller_h

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "SwitchDescriptors.h"

class Controller {
  public:
    Controller(uint8_t pin, uint8_t stateBytes) {
      _pin = pin;
      _stateBytes = stateBytes;
      _controllerState = new uint8_t[_stateBytes];
    }
    void emptyRxFifo() {
      while(pio_sm_get_rx_fifo_level(_pio, _sm)) {
        pio_sm_get(_pio, _sm);
      }
    }
    void updatePioOutputSize(uint8_t autoPullLength) {
      // Pull mask = 0x3E000000
      // Push mask = 0x01F00000
      pio_sm_set_enabled(_pio, _sm, false);
      _pio->sm[_sm].shiftctrl = (_pio->sm[_sm].shiftctrl & 0xA00FFFFF) | (0x8 << 20) | (((autoPullLength + 5) & 0x1F) << 25);
      // Restart the state machine to avoid 16 0 bits being auto-pulled
      _pio->ctrl |= 1 << (4 + _sm);
      pio_sm_set_enabled(_pio, _sm, true);
    }
    void sendData(uint8_t *request, uint8_t dataLength, uint8_t *response, uint8_t responseLength) {
      uint32_t dataWithResponseLength = ((responseLength - 1) & 0x1F) << 27;
      for (int i = 0 ; i < dataLength ; i++) {
        dataWithResponseLength |= *(request + i) << (19 - i * 8);
      }
      pio_sm_put_blocking(_pio, _sm, dataWithResponseLength);

      int16_t remainingBytes = responseLength;
      while(remainingBytes > 0) {
        absolute_time_t timeout_us = make_timeout_time_us(600);
        bool timedOut = false;
        while(pio_sm_is_rx_fifo_empty(_pio, _sm) && !timedOut) {
          timedOut = time_reached(timeout_us);
        }
        if (timedOut) {
          throw 0;
        }
        uint32_t data = pio_sm_get(_pio, _sm);
        response[responseLength - remainingBytes] = (uint8_t)(data& 0xFF); // & (0xFF << i * 8);
        remainingBytes--;
      }
    }
    virtual void init() = 0;
    virtual void updateState() = 0;
    virtual SwitchReport *getSwitchReport() = 0;
  protected:
    uint8_t _pin;
    PIO _pio;
    uint _sm;
    pio_sm_config *_c;
    uint _offset;
    uint8_t _stateBytes;
    uint8_t *_controllerState;
    SwitchReport _switchReport {
    	.buttons = 0,
    	.hat = SWITCH_HAT_NOTHING,
    	.lx = SWITCH_JOYSTICK_MID,
    	.ly = SWITCH_JOYSTICK_MID,
    	.rx = SWITCH_JOYSTICK_MID,
    	.ry = SWITCH_JOYSTICK_MID,
    	.vendor = 0,
    };
};

#endif
