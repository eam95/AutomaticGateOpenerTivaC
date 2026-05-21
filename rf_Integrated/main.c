#include <stdint.h>
#include "tm4c123gh6pm.h"   // Header file for the TM4C123 microcontroller
#include "Timer3A.h"       // Timer 3A library
#include "Timer0A.h"       // Timer 0A library
#include "LCD.h"           // LCD driver library
#include "SSI2.h"          // SSI2 (SPI) module library
#include "UART.h"          // UART communication library

// LED color definitions for controlling Port F LEDs
#define OFF_LED   0x00       // All LEDs off
#define RED       0x02       // Red LED
#define BLUE      0x04       // Blue LED
#define GREEN     0x08       // Green LED
#define YELLOW    0x0A       // Red + Green = Yellow

// Switch definitions for LaunchPad buttons on Port F
#define SW1       0x10       // Left-side button (PF4)
#define SW2       0x01       // Right-side button (PF0)

// Sensor input pin on Port E
#define SensorDetect 0x01    // Sensor connected to PE0

// LCD command definitions for specific functionalities
#define clearLCD 0x01                       // Clear the LCD screen
#define returnHome 0x02                     // Return cursor to home position
#define decrementCursor 0x04                // Move cursor to the left
#define incrementCursor 0x06                // Move cursor to the right
#define shiftDisplayRight 0x05              // Shift display content to the right
#define shiftDisplayLeft 0x07               // Shift display content to the left
#define DispOffCurOff 0x08                  // Display and cursor off
#define DispOffCurOn 0x0A                   // Display off, cursor on
#define DispOnCurOff 0x0C                   // Display on, cursor off
#define DispOnCurOn 0x0E                    // Display and cursor on
#define shiftLeftCur 0x10                   // Move cursor to the left
#define shiftRightCur 0x14                  // Move cursor to the right
#define displayLeft 0x18                    // Shift entire display to the left
#define displayRight 0x1C                   // Shift entire display to the right
#define moveFirstLine 0x80                  // Move cursor to the first line
#define moveSecondLine 0xC0                 // Move cursor to the second line

// Function prototypes
void Delay_MicroSecond(int time);
void GPIOF_Init(void);
void GPIOE_Init(void);
uint32_t getDistanceAvg(void);
uint32_t getSingleDistance(void);
void LCD_Message(uint8_t display[]);
uint32_t getDisplacement(uint32_t xf, uint32_t xi);

// Global variable for internal logic
int j = 0;

int main() {
    // Initialize peripherals and modules
    GPIOF_Init();               // Initialize Port F for LED and button control
    GPIOE_Init();               // Initialize Port E for sensor input
    Timer0ACapture_init();      // Configure Timer0A for edge-time capture mode
    SSI2_init();                // Initialize SSI2 for SPI communication
    LCD_init();                 // Initialize LCD module
    UART_Init();                // Initialize UART module

    // LCD messages used in the program
    uint8_t distanceLabel[] = "x=\0";         // Label for distance display
    uint8_t bothLabel[] = "x,dx=\0";          // Label for position and displacement
    uint8_t unit[] = " [mm]\0";              // Unit for distance
    uint8_t closing[] = "Gate Closing... \0"; // Gate closing message
    uint8_t opening[] = "Gate Opening... \0"; // Gate opening message
    uint8_t closed[] = "Gate Closed \0";     // Gate closed message
    uint8_t open[] = "Gate Open \0";         // Gate open message
    uint8_t obstacle[] = "Obst. Detected! \0"; // Obstacle detected message
    uint8_t comma[] = ",\0";                // Comma for separating values

    // Variables for gate state and distance measurements
    uint32_t position;
    uint32_t initialPosition = 0;
    uint32_t finalPosition;
    uint32_t deltaPosition;
    uint8_t ButtonPressed;
    uint8_t flag = 1; // Logic flag for state transitions

    // Enum for gate states
    enum State {CHECK, OPENING, CLOSING, OPEN, CLOSED, OBSTACLE};
    enum State gateState = CHECK; // Initial state

    while (1) {
        // Get current distance from the sensor
        position = getSingleDistance();
        finalPosition = position; // Store final position for calculations
        deltaPosition = getDisplacement(finalPosition, initialPosition); // Calculate displacement
        initialPosition = position; // Update initial position

        // Clear LCD for new display
        LCD_command(clearLCD);

        // State machine to control gate behavior
        switch (gateState) {
        case OPENING:
            GPIO_PORTF_DATA_R = GREEN;      // Turn on green LED
            LCD_command(clearLCD);          // Clear display
            LCD_Message(opening);           // Display "Gate Opening..."
            LCD_command(moveSecondLine);    // Move to second line
            LCD_Message(distanceLabel);     // Display distance label
            LCD_OutUDec(position);          // Display position value
            LCD_Message(unit);              // Display unit
            gateState = CHECK;              // Transition to CHECK state
            break;

        case OBSTACLE:
            GPIO_PORTF_DATA_R = GREEN;      // Turn on green LED
            flag = 2;                       // Set obstacle flag
            LCD_command(clearLCD);          // Clear display
            LCD_Message(obstacle);          // Display "Obstacle Detected"
            LCD_command(moveSecondLine);    // Move to second line
            LCD_Message(opening);           // Display "Gate Opening..."
            gateState = CHECK;              // Transition to CHECK state
            break;

        case CLOSING:
            GPIO_PORTF_DATA_R = RED;        // Turn on red LED
            LCD_command(clearLCD);          // Clear display
            LCD_Message(closing);           // Display "Gate Closing..."
            LCD_command(moveSecondLine);    // Move to second line
            LCD_Message(bothLabel);         // Display both position and displacement
            LCD_OutUDec(position);          // Display position value
            LCD_Message(comma);             // Display comma
            LCD_OutUDec(deltaPosition);     // Display displacement value
            LCD_Message(unit);              // Display unit

            if (position > 50 && deltaPosition > 20) {
                gateState = OBSTACLE;       // Detect obstacle if criteria met
            } else {
                gateState = CHECK;          // Transition to CHECK state
            }
            break;

        case CLOSED:
            LCD_command(clearLCD);          // Clear display
            LCD_Message(closed);            // Display "Gate Closed"
            gateState = CHECK;              // Transition to CHECK state
            break;

        case OPEN:
            LCD_command(clearLCD);          // Clear display
            LCD_Message(open);              // Display "Gate Open"
            flag = 1;                       // Reset flag
            gateState = CHECK;              // Transition to CHECK state
            break;

        case CHECK:
            ButtonPressed = GPIO_PORTE_DATA_R & SensorDetect; // Read button state

            if (flag == 2) {
                if (position < 270) {
                    gateState = OBSTACLE;   // Detect obstacle if position < threshold
                } else {
                    gateState = OPEN;       // Transition to OPEN state
                }
                break;
            } else if (ButtonPressed) {
                flag = 1;                   // Set flag if button pressed
            } else {
                flag = 0;                   // Clear flag if button not pressed
            }

            if (flag == 1) {
                if (position < 270) {
                    gateState = OPENING;    // Transition to OPENING state
                } else {
                    gateState = OPEN;       // Transition to OPEN state
                }
            } else {
                if (position > 50) {
                    gateState = CLOSING;    // Transition to CLOSING state
                } else {
                    gateState = CLOSED;     // Transition to CLOSED state
                }
            }
            break;
        }
        Timer3A_Wait1ms(50); // Add a delay to prevent rapid state transitions
    }
}

// Function to measure single distance from the sensor
uint32_t getSingleDistance(void) {
    uint32_t time;          // Stores pulse duration
    uint32_t distance;      // Stores calculated distance
    uint32_t decimalplace = 10; // Decimal place scaling factor

    time = Measure_distance();                    // Measure pulse duration
    distance = (time * 10625 * decimalplace) / 10000000; // Convert duration to distance
    return(distance);
}

// Function to send a message to the LCD
void LCD_Message(uint8_t display[]) {
    int k = 0;
    for (k = 0; display[k]; k++) {
        LCD_data(display[k]); // Send each character to the LCD
    }
}

// Initialize GPIO Port F for LEDs and switches
void GPIOF_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;           // Enable clock for Port F
    while ((SYSCTL_PRGPIO_R & 0x20) == 0); // Wait for Port F to be ready
    GPIO_PORTF_CR_R = SW1;              // Allow changes to PF4 and PF0
    GPIO_PORTF_DIR_R |= 0x0E;           // Set PF1, PF2, PF3 as outputs (LEDs)
    GPIO_PORTF_DEN_R |= 0x1E;           // Enable digital functionality on PF1-PF4
    GPIO_PORTF_PUR_R = SW1;             // Enable pull-up resistors on PF0 and PF4
}

// Initialize GPIO Port E for sensor input
void GPIOE_Init(void) {
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4; // Enable clock for Port E
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R4) == 0); // Wait for Port E to be ready
    GPIO_PORTE_DIR_R &= ~SensorDetect;    // Set PE0 as input
    GPIO_PORTE_DEN_R |= SensorDetect;     // Enable digital functionality on PE0
}

// Delay function for microsecond timing
void Delay_MicroSecond(int time) {
    int i;
    SYSCTL_RCGCTIMER_R |= 2;            // Enable clock for Timer Block 1
    TIMER1_CTL_R = 0;                   // Disable Timer before configuration
    TIMER1_CFG_R = 0x04;                // Configure Timer1 as 16-bit
    TIMER1_TAMR_R = 0x02;               // Set to periodic mode, down-counter
    TIMER1_TAILR_R = 16 - 1;            // Load value for 1 microsecond delay
    TIMER1_ICR_R = 0x1;                 // Clear timeout flag
    TIMER1_CTL_R |= 0x01;               // Enable Timer A

    for (i = 0; i < time; i++) {
        while ((TIMER1_RIS_R & 0x1) == 0); // Wait for timeout flag
        TIMER1_ICR_R = 0x1;             // Clear timeout flag
    }
}

// Function to calculate displacement between two positions
uint32_t getDisplacement(uint32_t xf, uint32_t xi) {
    uint32_t delta_x = xf - xi;         // Calculate displacement
    if (delta_x > 0x10000000) {         // Handle unsigned integer overflow
        delta_x = ~delta_x + 1;         // Two's complement for negative result
    }
    return(delta_x);
}
