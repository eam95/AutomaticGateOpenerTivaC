// UART functions
 
#include <stdint.h>
#include "UART.h"
#include "tm4c123gh6pm.h"

void UART_Init(void)
{
    SYSCTL_RCGCUART_R |= 0x01;  // activate UART0
    SYSCTL_RCGCGPIO_R |= 0x01;  // activate port A

    UART0_CTL_R &= ~UART_CTL_UARTEN;      // disable UART0
    UART0_IBRD_R = 8;                     // IBRD = int(16,000,000 / (16 * 115,200)) = int(8.6806)
    UART0_FBRD_R = 44;                    // FBRD = round(0.6806 * 64) = 44
    UART0_CC_R = 0x0;                     // use system clock
    UART0_LCRH_R = 0x60;                  // 8-bit, no parity, 1-stop bit, no FIFO
    UART0_CTL_R |= 0x301;                 // enable UART0, TXE, RXE

    GPIO_PORTA_AFSEL_R |= 0x03;           // enable alt funct on PA1-0
    GPIO_PORTA_DEN_R |= 0x03;             // enable digital I/O on PA1-0
    GPIO_PORTA_PCTL_R |= (GPIO_PCTL_PA0_U0RX | GPIO_PCTL_PA1_U0TX);  // configure PA1-0 as UART
    GPIO_PORTA_AMSEL_R &= ~0x03;          // disable analog on PA1-0
}

char readChar(void)
{
    while((UART0_FR_R & UART_FR_RXFE) != 0);  // wait until RXFE is 0 (receiver not empty)
    return UART0_DR_R & 0xFF;  // return received character
}

void printChar(char c)
{
    while((UART0_FR_R & UART_FR_TXFF) != 0);  // wait until TXFF is 0 (transmitter not full)
    UART0_DR_R = c;
}

void printString(char* string)
{
    while(*string)
    {
        printChar(*(string++));
    }
}
