#include <stdio.h>
#include "N64Controller.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "N64.pio.h"

N64Controller::N64Controller(int n64Pin) {
    _n64Pin = n64Pin;
}

void N64Controller::init() {
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &n64_program);
    sm = pio_claim_unused_sm(pio, true);
    pio_sm_config c = n64_program_get_default_config(offset);
    n64_program_init(pio, sm, offset, _n64Pin, &c);
    sendByte(0x00);
    sleep_us(200);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

uint32_t N64Controller::getState() {
    emptyRxFifo();
    sendByte(0x01);
    absolute_time_t timeout_us = make_timeout_time_us(200);
    // Todo: Add timeout and recovery
    bool timedOut = false;
    while(pio_sm_is_rx_fifo_empty(pio0, sm) && !timedOut) {
        timedOut = time_reached(timeout_us);
    }
    if (!timedOut) {
        return pio_sm_get(pio0, sm);
    }
    throw 0;
}

void N64Controller::sendByte(uint8_t byte) {
    pio_sm_put_blocking(pio0, 0, byte << 24);
}

void N64Controller::emptyRxFifo() {
    while(pio_sm_get_rx_fifo_level(pio0, sm)) {
        pio_sm_get(pio0, sm);
    }
}
