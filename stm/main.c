#include <stdint.h>

#define USART1_BASE_ADDR 0x40011000
#define RCC_BASE_ADDR    0x40023800

volatile uint32_t *const RCC_APB2ENR = (uint32_t *)(RCC_BASE_ADDR + 0x44);
volatile uint32_t *const GPIOA_MODER = (uint32_t *)(0x40020000 + 0x00);
volatile uint32_t *const GPIOA_AFRL = (uint32_t *)(0x40020000 + 0x20);
volatile uint32_t *const USART1_SR = (uint32_t *)(USART1_BASE_ADDR + 0x00);
volatile uint32_t *const USART1_DR = (uint32_t *)(USART1_BASE_ADDR + 0x04);
volatile uint32_t *const USART1_BRR = (uint32_t *)(USART1_BASE_ADDR + 0x08);
volatile uint32_t *const USART1_CR1 = (uint32_t *)(USART1_BASE_ADDR + 0x0C);

void delay(uint32_t count) {
    for (volatile uint32_t i = 0; i < count; ++i) {
        // Delay loop
    }
}

void uart_init(void) {
    // Enable GPIOA and USART1 peripheral clock
    *RCC_APB2ENR |= (1U << 0); // GPIOA
    *RCC_APB2ENR |= (1U << 4); // USART1

    // Configure PA9 (TX) as alternate function
    *GPIOA_MODER &= ~(3U << (2 * 9)); // Clear bits
    *GPIOA_MODER |= (2U << (2 * 9));  // Set as alternate function

    // Configure PA9 alternate function to USART1 TX (AF7)
    *GPIOA_AFRL &= ~(0xFU << (4 * 9)); // Clear bits
    *GPIOA_AFRL |= (7U << (4 * 9));    // Set to AF7 (USART1_TX)

    // Configure USART1
    *USART1_CR1 = 0;            // Disable USART1
    *USART1_BRR = 0x208D;       // 9600 baud rate @ 16MHz
    *USART1_CR1 |= (1U << 3);   // Enable transmitter
    *USART1_CR1 |= (1U << 13);  // Enable USART1
}

void uart_send_char(char c) {
    // Wait until the transmit data register is empty
    while (!(*USART1_SR & (1U << 7))) {}

    // Send the character
    *USART1_DR = c;
}

void uart_send_string(const char *str) {
    while (*str != '\0') {
        uart_send_char(*str++);
    }
}

int main(void) {
    uart_init();

    while (1) {
        uart_send_string("Hello, UART!\r\n");
        delay(10); // Delay for a while
    }
}

