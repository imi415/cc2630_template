#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "driverlib/systick.h"
#include "driverlib/prcm.h"
#include "driverlib/aon_rtc.h"
#include "driverlib/aon_wuc.h"
#include "driverlib/aon_event.h"
#include "driverlib/event.h"

uint32_t _rtc_to_ticks(uint32_t sec, uint32_t frac) {
    return (sec & 0xFFFFU) * configTICK_RATE_HZ + (frac >> 16U) * configTICK_RATE_HZ / 65536;
}

void vApplicationSleep(uint32_t xExpectedIdleTime) {
    uint32_t prev_sleep_ticks, post_sleep_ticks;
    if(xExpectedIdleTime > pdMS_TO_TICKS(1000)) {
        // Do standby operations
    } else if(xExpectedIdleTime > pdMS_TO_TICKS(100)) {
        // Do idle operations
    }
}

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize) {
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__((aligned(32)));
    
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize ) {
        static StaticTask_t xTimerTaskTCB;
        static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH] __attribute__((aligned(32)));

        *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
        *ppxTimerTaskStackBuffer = uxTimerTaskStack;

        *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
