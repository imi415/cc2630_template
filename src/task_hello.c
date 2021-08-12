#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "driverlib/gpio.h"
#include "driverlib/aon_rtc.h"
#include "driverlib/prcm.h"
#include "driverlib/aon_wuc.h"
#include "driverlib/osc.h"
#include "driverlib/pwr_ctrl.h"
#include "driverlib/timer.h"

static void report_power_and_clocks(void);

TaskHandle_t xUserTaskHelloHandle = NULL;
void vUserTaskHello(void *pvParameters) {
    report_power_and_clocks();
    uint32_t second;
    uint32_t fraction;
    for(;;) {
        /*
        * These must be called in order,
        * a read operation to the second register 
        * will latch the fraction register until its read.
        */
        second = AONRTCSecGet();
        fraction = (AONRTCFractionGet() >> 24U) * 100 / 256; // Convert to decimal.
        printf("RTC second: %lu.%lu\r", second, fraction);
        fflush(stdout);
        vTaskDelay(100);
    }
}

static void report_power_and_clocks(void) {
    uint32_t ret;
    char *ptr;

    // Power source
    printf("=== Report power supply system status: ===\r\n");
    ret = PowerCtrlSourceGet();
    if(ret == PWRCTRL_PWRSRC_GLDO) ptr = "global LDO";
    else ptr = "internal DCDC converter";
    printf("Current power supply: %s\r\n", ptr);

    // PRCM MCU PD
    printf("=== Report PRCM/DDI status: ===\r\n");

    ret = PRCMInfClockConfigureGet(PRCM_RUN_MODE);
    printf("Infrastructure clock: PRCM_CLOCK_DIV_%d\r\n", 1U << ret);

    ret = PRCMPowerDomainStatus(PRCM_DOMAIN_RFCORE);
    printf("RF core power domain: %s\r\n", ret == PRCM_DOMAIN_POWER_ON ? "ON" : "OFF");

    ret = PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH);
    printf("Peripheral power domain: %s\r\n", ret == PRCM_DOMAIN_POWER_ON ? "ON" : "OFF");

    ret = PRCMPowerDomainStatus(PRCM_DOMAIN_SERIAL);
    printf("Serial power domain: %s\r\n", ret == PRCM_DOMAIN_POWER_ON ? "ON" : "OFF");

    ret = OSCClockSourceGet(OSC_SRC_CLK_HF);
    switch(ret) {
        case OSC_RCOSC_HF:
        ptr = "OSC_RCOSC_HF";
        break;
        case OSC_XOSC_HF:
        ptr = "OSC_XOSC_HF";
        break;
        default:
        ptr = "UNKNOWN";
        break;
    }
    printf("HF clock source: %s\r\n", ptr);

    ret = OSCClockSourceGet(OSC_SRC_CLK_LF);
    switch(ret) {
        case OSC_RCOSC_LF:
        ptr = "OSC_RCOSC_LF";
        break;
        case OSC_XOSC_LF:
        ptr = "OSC_XOSC_LF";
        break;
        default:
        ptr = "UNKNOWN";
        break;
    }
    printf("LF clock source: %s\r\n", ptr);
    printf("==============================\r\n\r\n");

    // AUX PD
    printf("=== Report AUX/AON status: ===\r\n");

    ret = AONWUCAuxClockConfigGet();
    printf("AUX clock: AUX_CLOCK_DIV_%d\r\n", 1U << ((ret >> 0x08U) + 1));

    ret = AONWUCPowerStatusGet();
    printf("AON power state: ");
    if(ret & AONWUC_AUX_POWER_ON)  printf("AONWUC_AUX_POWER_ON");
    if(ret & AONWUC_MCU_POWER_ON)  printf(" | AONWUC_MCU_POWER_ON");
    if(ret & AONWUC_JTAG_POWER_ON) printf(" | AONWUC_JTAG_POWER_ON");
    printf("\r\n");
    printf("==============================\r\n\r\n");
    printf("Report done.\r\n");
}