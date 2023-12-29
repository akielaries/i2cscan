#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

#ifdef __AVR_ATmega328__
  #define UART_BAUD_RATE 38400
  #define I2C_CLOCK 100000
  #define I2C_PRESCALER 1
  #define I2C_BIT_RATE_REG ((F_CPU / I2C_CLOCK) - 16) / (2 * I2C_PRESCALER)
#endif

#ifdef __AVR_ATmega2560__
  #define UART_BAUD_RATE 115200
  #define I2C_CLOCK 100000
  #define I2C_PRESCALER 1
  #define I2C_BIT_RATE_REG ((F_CPU / I2C_CLOCK) - 16) / (2 * I2C_PRESCALER)
#endif

static int uart_putchar(char c, FILE *stream) {
    // if character is newline, also send carriage return
    if (c == '\n') {
        uart_putchar('\r', stream);
    }
    // wait until the UART data register is empty
    loop_until_bit_is_set(UCSR0A, UDRE0);
    // send character to UART data register
    UDR0 = c;
    return 0;
}

// create a stream for stdout (printf)
static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);


static void setup_uart() {
#ifdef __AVR_ATmega2560__
    // disable USART during baud rate setting
    UCSR0B = 0;

#elif __AVR_ATmega328__
    // in case UART was put into power-reduction mode ...
    PRR &= ~ _BV(PRUSART0);

#endif

    // BAUD is defined via command line: -DBAUD=115200
    // config UART baud rate based on setbaud.h?
    //#include <util/setbaud.h>

    // UART baud HIGH BYTE
    UBRR0H = UBRRH_VALUE;
    // UART baud LOW BYTE
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    // if 2x is defined set the U2X0 bit in the UART status register A
    UCSR0A |= _BV(U2X0);
#else
    // default to clearing the U2x0 bit
    UCSR0A &= ~_BV(U2X0);
#endif

#ifdef __AVR_ATmega2560__
    // set UART frame format to 8 data bits, no parity, 1 stop bit
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);  // 8N1
    
    // enable UART TX and RX (transmit x recieve)
    UCSR0B = _BV(TXEN0) | _BV(RXEN0);  // Enable transmitter and receiver

#elif __AVR_ATmega328__
    // UART Control and Status Register B:
    // - Enable transmitter only
    UCSR0B = _BV(TXEN0);

    // 8N1
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

#endif

    // create redirect for stdout to UART stream
    stdout = &uart_stdout;
}

static void i2c_set_freq() {
#ifdef __AVR_ATmega2560__
    // for TWI (two-wire-interface) set prescaler to 1
    TWSR = 0;
    // set bit rate register for 100 kHz I2C, I2C default is 100 kHz
    TWBR = ((F_CPU / 100000UL) - 16) / 2;  // Set bit rate register

#elif __AVR_ATmega328__
    TWBR = 32;
#endif

}

static void i2c_start() {
    // stop condition on I2C bus
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    // wait until condition is complete
    loop_until_bit_is_set(TWCR, TWINT);
}

static void i2c_stop() {
    // stop condition on I2C bus
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
    // wait until condition is complete
    loop_until_bit_is_clear(TWCR, TWSTO);
}

static void i2c_set_address(uint8_t addr, uint8_t rw) {
    // load TWI data register with the device address and read/write flag
    TWDR = (addr << 1) | rw;
    // clear TWI interrupt flag and enable TWI
    TWCR = _BV(TWINT) | _BV(TWEN);
    // wait until TWI interrupt flag is set, indicating the address is sent
    loop_until_bit_is_set(TWCR, TWINT);
}

static void probe_address(uint8_t i) {
    // flag start condition on I2C bus
    i2c_start();

#ifdef __AVR_ATmega2560__
    // set I2C address in write mode
    i2c_set_address(i, 0);  // TW_WRITE
    // checks if address responds to ping
    if ((TWSR & 0xF8) == 0x18) {  // TW_MT_SLA_ACK
        printf("Found device at I2C address: 0x%02X (%d)\n", i, i);
    }

#elif __AVR_ATmega328__
    i2c_set_address(i, TW_WRITE);
    if ((TWSR & TW_STATUS_MASK) == TW_MT_SLA_ACK) {
        printf("Found device at I2C address: 0x%" PRIx8 " (%" PRIu8 ")\n", i, i);
    }
#endif

    // flag stop condition on I2C bus
    i2c_stop();
}

static void scan() {
    // for address 0-127, probe
    for (uint8_t i = 1; i < 128; ++i)
        probe_address(i);
}

int main() {
    // setup UART transmission
/*    setup_uart();
    // set I2C bus frequency
    i2c_set_freq();

    for (;;)
    {
        printf("Scanning I2C bus at 100 kHz\n");
        scan();

        for (uint16_t i = 0; i < 30000; ++i)
            _delay_ms(1);
    }

    return 0;
    */
    // Set Pin 7 (Arduino Mega Pin 13) as an output
    DDRB |= (1 << PB7);

    // Initialize UART
    setup_uart();

    // Print a welcome message
    printf("UART Example: Hello, AVR!\n");

    // Main loop
    while (1) {
        // Do some processing or tasks here
        // For example, blink an LED every second

        PORTB ^= (1 << PB7);
        _delay_ms(100);      // Wait for 500 milliseconds


        printf("We're looping... wait 1000ms\n");
        _delay_ms(100);

    }

    return 0;

}

