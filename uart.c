#include "uart.h"
#include <avr/io.h>
#include <stdio.h>

static int uart_putchar(char c, FILE *stream) {
    if(c == '\n') { uart0_write('\r'); }
    uart0_write(c);
    return 0;
}

static FILE _stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main() {
    uart0_init(115200);
    stdout = &_stdout;

    printf("Start setup.\n");

    while(1) {

    }
}



