#include "uart.h"
void UART1_Init(void) {
    // GPIOA & USART1 Clock
    RCC->APB2ENR |= (1 << 2);  // IOPAEN
    RCC->APB2ENR |= (1 << 14); // USART1EN

    // PA9 (TX) as Alternate Function Push-Pull
    GPIOA->CRH &= ~(0xF << 4);         // Clear bits for PA9
    GPIOA->CRH |=  (0xB << 4);         // 0b1011: Output 50MHz, AF Push Pull

    // PA10 (RX) as Input Floating
    GPIOA->CRH &= ~(0xF << 8);         // Clear bits for PA10
    GPIOA->CRH |=  (0x4 << 8);         // 0b0100: Input floating

    // Config Baudrate
    // USARTDIV = Fck / (16 * Baud)
    // Fck = 72MHz, Baud = 9600
    // USARTDIV = 72000000 / (16 * 9600) = 468.75
    // -> Mantissa = 468, Fraction = 0.75*16 = 12
    USART1->BRR = (468 << 4) | 12;

    // Enable USART1, Tx & Rx
    USART1->CR1 |= (1 << 13); // UE: USART Enable
    USART1->CR1 |= (1 << 3);  // TE: Transmit Enable
    USART1->CR1 |= (1 << 2);  // RE: Receive Enable
}

void UART1_SendChar(char c) {
    while (!(USART1->SR & (1 << 7))); // Wait TXE = 1
    USART1->DR = c;
}

void UART1_SendString(const char *str) {
    while (*str) {
        UART1_SendChar(*str++);
    }
}

void uart_send_hex32(uint32_t value) {
    UART1_SendString("0x");

    for (int i = 7; i >= 0; i--) {
        uint8_t nibble = (value >> (i * 4)) & 0xF;
        char c = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
        UART1_SendChar(c);
    }

    UART1_SendString("\r\n");
}
void log_stack(uint32_t *sp) {
    UART1_SendString("=== Stack Dump For Task ===\r\n");

    const char *labels[] = {
        " R4:  ", " R5:  ", " R6:  ", " R7:  ",
        " R8:  ", " R9:  ", "R10:  ", "R11:  ",
        " R0:  ", " R1:  ", " R2:  ", " R3:  ",
        "R12:  ", " LR:  ", " PC:  ", "xPSR: "
    };

    for (int i = 0; i < 16; i++) {
        UART1_SendString(labels[i]);
        uart_send_hex32(sp[i]);
    }

    UART1_SendString("===========================\r\n");
}