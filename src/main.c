#include <stdio.h>
#include <stdint.h>

#include "driverlib/ioc.h"
#include "driverlib/gpio.h"
#include "driverlib/pwr_ctrl.h"
#include "driverlib/prcm.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "driverlib/aux_adc.h"
#include "driverlib/aux_wuc.h"
#include "driverlib/aon_wuc.h"
#include "driverlib/uart.h"
#include "driverlib/osc.h"

#include "FreeRTOS.h"
#include "task.h"

// Private functions
void u_InitBootPeripherals(void);
void u_InitBootInterruptHandlers(void);
void u_CreateUserTasks(void);

/* Interrupt Handlers */
void BusFaultHandler(void);

void freertos_systick_handler(void);
void freertos_pendsv_handler(void);
void freertos_svc_handler(void);

int main(int argc, char *argv[]) {
    u_InitBootInterruptHandlers();
    u_InitBootPeripherals();

    printf("Hello world from a CC2630.\r\n");

    u_CreateUserTasks();

    vTaskStartScheduler();

    for(;;) {
        // We should never get here.
    }
}

void u_InitBootPeripherals(void) {
    // Switch to HF XOSC
#ifndef DRIVERLIB_NOROM
    OSCClockSourceSet(OSC_SRC_CLK_HF, OSC_XOSC_HF);
    while(!OSCHfSourceReady()) {
        //
    }
    OSCHfSourceSwitch();
#endif

    // Enable power domains
    uint32_t prcm_domain_enabled = PRCM_DOMAIN_PERIPH \
                                 | PRCM_DOMAIN_SERIAL;
    PRCMPowerDomainOn(prcm_domain_enabled);

    // Enable clock gating
    PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
    PRCMPeripheralRunEnable(PRCM_PERIPH_UART0);
    
    // Sync changes
    PRCMLoadSet();

    // Wait for all power domains powered up
    while(PRCMPowerDomainStatus(prcm_domain_enabled) != PRCM_DOMAIN_POWER_ON) {
        //
    }

    // Enable UART
    IOCPortConfigureSet(IOID_18, IOC_PORT_MCU_UART0_TX, IOC_IOMODE_NORMAL);
    IOCPortConfigureSet(IOID_19, IOC_PORT_MCU_UART0_RX, IOC_IOMODE_NORMAL);

    uint32_t uart_config = UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8;
    UARTConfigSetExpClk(UART0_BASE, 48000000, 115200, uart_config);
    UARTFIFOEnable(UART0_BASE);
    UARTEnable(UART0_BASE);

    // Enable ADC
    IOCPortConfigureSet(IOID_30, IOC_PORT_AUX_IO, IOC_IOMODE_NORMAL);

    uint32_t aux_clocks_enabled = AUX_WUC_ADC_CLOCK \
                                | AUX_WUC_ADI_CLOCK \
                                | AUX_WUC_OSCCTRL_CLOCK \
                                | AUX_WUC_ANAIF_CLOCK;
    AUXWUCClockEnable(aux_clocks_enabled);
    while(AUXWUCClockStatus(aux_clocks_enabled) != AUX_WUC_CLOCK_READY) {
        //
    }

    AUXADCSelectInput(ADC_COMPB_IN_AUXIO0);
    AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_10P9_MS, AUXADC_TRIGGER_MANUAL);

    // Enable GPIO LED

    // NO pull, open drain, normal mode
    IOCPortConfigureSet(IOID_20, IOC_PORT_GPIO, IOC_IOMODE_OPEN_DRAIN_NORMAL);
    GPIO_setOutputEnableDio(20, GPIO_OUTPUT_ENABLE);
    GPIO_clearDio(20);
}

void u_InitBootInterruptHandlers(void) {
    IntRegister(INT_BUS_FAULT, BusFaultHandler);
    IntEnable(INT_BUS_FAULT);

    IntRegister(INT_PENDSV, freertos_pendsv_handler);
    IntRegister(INT_SVCALL, freertos_svc_handler);
    IntRegister(INT_SYSTICK, freertos_systick_handler);

}
