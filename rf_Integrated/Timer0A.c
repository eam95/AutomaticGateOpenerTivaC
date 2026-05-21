// Timer0A.c
// Runs on Tiva-C

// Adapted from SysTick.c from the book:
/* "Embedded Systems: Introduction to MSP432 Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Volume 1, Program 4.7
*/

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Timer0A.h"

/* Timer0A initialization in input-edge time mode with up-count mode */
void Timer0ACapture_init(void)
{
    SYSCTL_RCGCTIMER_R |= 1;            /* Enable clock to Timer Block 0 */
    SYSCTL_RCGCGPIO_R |= 2;             /* Enable clock to PORTB */

    GPIO_PORTB_DIR_R &= ~0x40;          /* Make PB6 an input pin 0100 0000*/
    GPIO_PORTB_DEN_R |= 0x40;           /* Make PB6 a digital pin */
    GPIO_PORTB_AFSEL_R |= 0x40;         /* Use PB6 alternate function */
    GPIO_PORTB_PCTL_R &= ~0x0F000000;   /* Configure PB6 for T0CCP0 */
    GPIO_PORTB_PCTL_R |= 0x07000000;

    SYSCTL_RCGCGPIO_R |= 1;             /* Enable clock to PORTA */
    GPIO_PORTA_DIR_R |= (1 << 4);       /* Set PA4 as a digital output pin */
    GPIO_PORTA_DEN_R |= (1 << 4);       /* Make PA4 a digital pin */

    TIMER0_CTL_R &= ~1;                 /* Disable Timer0A during setup */
    TIMER0_CFG_R = 4;                   /* 16-bit timer mode */
    TIMER0_TAMR_R = 0x17;               /* Up-count, edge-time, capture mode */
    TIMER0_CTL_R |= 0x0C;               /* Capture the rising edge */
    TIMER0_CTL_R |= (1 << 0);           /* Enable Timer0A */
}

/* Measure pulse duration */
uint32_t Measure_distance(void)
{
    int lastEdge, thisEdge;

    /* Generate 10us trigger pulse */
    GPIO_PORTA_DATA_R &= ~(1 << 4);     /* Set trigger pin low 0000 0000*/
    Delay_MicroSecond(10);              /* 10 microseconds delay */
    GPIO_PORTA_DATA_R |= (1 << 4);      /* Set trigger pin high 0001 0000*/
    Delay_MicroSecond(10);              /* 10 microseconds delay */
    GPIO_PORTA_DATA_R &= ~(1 << 4);     /* Set trigger pin low 0000 0000*/

    while(1)
    {
        TIMER0_ICR_R = 4;               /* Clear Timer0A capture flag */
        while((TIMER0_RIS_R & 4) == 0); /* Wait till captured */

        if(GPIO_PORTB_DATA_R & (1 << 6)) /* Check if rising edge occurs */
        {
            lastEdge = TIMER0_TAR_R;    /* Save timestamp */
            TIMER0_ICR_R = 4;           /* Clear Timer0A capture flag */
            while((TIMER0_RIS_R & 4) == 0); /* Wait till captured */
            thisEdge = TIMER0_TAR_R;    /* Save timestamp */
            return (thisEdge - lastEdge); /* Return time difference */
        }
    }
}
