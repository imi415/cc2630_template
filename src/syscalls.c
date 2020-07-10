#include "driverlib/uart.h"

int _write(int file, char *buf, int nbytes) {
    for(int i = 0; i < nbytes; i++) {
        while(UARTSpaceAvail(UART0_BASE) == 0) {
            //
        }
        UARTCharPut(UART0_BASE, buf[i]);
    }
}
