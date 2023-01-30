#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "N64Controller.h"
#include "SwitchDescriptors.h"
#include "tusb.h"

int main() {
    stdio_init_all();
    N64Controller n64Controller(18);
    SwitchDescriptors switchController;
    n64Controller.init();
    tusb_init();
    while (true) {
        try {
            uint32_t n64State = n64Controller.getState();
            tud_task();
            if (tud_suspended()) {
                tud_remote_wakeup();
            }
            if(tud_hid_ready()) {
		        tud_hid_report(0, switchController.getSwitchReport(n64State), sizeof(SwitchReport));
            }
        } catch (int e) {
            tud_task();
            if (tud_suspended()) {
                tud_remote_wakeup();
            }
        }
    }
}
