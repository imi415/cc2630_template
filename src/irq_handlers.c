#include <stdint.h>

#include "driverlib/gpio.h"
#include "driverlib/timer.h"

void BusFaultHandler(void) {
    volatile uint32_t cfsr, hfsr, mmfar, bfar, afsr;
    cfsr  = *(uint32_t *)(0xE000E000 + 0xD28);
    hfsr  = *(uint32_t *)(0xE000E000 + 0xD2C);
    mmfar = *(uint32_t *)(0xE000E000 + 0xD34);
    bfar  = *(uint32_t *)(0xE000E000 + 0xD38);
    afsr  = *(uint32_t *)(0xE000E000 + 0xD3C);

    for(;;) {
        //
    }
}

void Timer3AIntHandler(void) {
    TimerIntClear(GPT3_BASE, TIMER_TIMA_TIMEOUT);
    GPIO_toggleDio(20);
}