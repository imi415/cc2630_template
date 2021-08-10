#include <stdint.h>

void pm_enter_sleep(uint32_t ticks_to_sleep) {
    // Timer IRQ wakeup
}

void pm_enter_deepsleep(uint32_t ticks_to_sleep) {
    // RTC AON event wakeup
}

void pm_enter_standby(uint32_t ticks_to_sleep) {

}