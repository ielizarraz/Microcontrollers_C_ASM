#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h> 

#pragma config OSC      =   INTIO67
#pragma config BOREN    =   OFF
#pragma config WDT      =   OFF
#pragma config LVP      =   OFF
#pragma config CCP2MX   =   PORTBE

#define _XTAL_FREQ      8000000                   // Set operation for 
#define TMR_CLOCK       _XTAL_FREQ/4              // Timer Clock 2 Mhz
#define COUNT_PER_MS    TMR_CLOCK/1000            // Count per ms = 2000
#define COUNT_SCALED    COUNT_PER_MS/32           // Divinding the count per ms to make the scaled count
#define Second_LED      PORTEbits.RE0

void interrupt high_priority T0ISR(void);         // Function prototyping
void Do_Init(void);

void Do_Init()                                    // Initialize the ports
{
 OSCCON=0x70;                                     // Set oscillator to 8 MHz
 ADCON1=0x0F;                                     // Configure all pins to digital
 TRISC=0x00;                                      // Configure PORT C to be all outputs
 TRISEbits.TRISE0 = 0; 
 T0CON=0x04;                                      // Timer0 off, increment on positive
                                                  // edge, 1:32 prescaler
 TMR0H=0x85;                                      // Set Timer High
 TMR0L=0xAA;                                      // Set Timer Low
 INTCONbits.TMR0IE=1;                             // Set interrupt enable
 INTCONbits.TMR0IF=0;                             // Clear interrupt flag
 INTCONbits.GIE=1;                                // Set the Global Interrupt Enable
 T0CONbits.TMR0ON=1;                              // Turn on Timer0
}
void interrupt high_priority T0ISR()
{
 INTCONbits.TMR0IF=0;                             // Clear the interrupt flag
 TMR0H=0x85;                                      // Reload Timer High and
 TMR0L=0xAA;                                      // Timer Low
 Second_LED = ~Second_LED;                        // flip logic state of
                                                  // Second_LED
}
void main()
{
char i;
OSCCON=0x070;                                     // Program oscillator to be at 8Mhz
 TRISC=0x00;                                      // Setup port C with output
 SSPSTAT=0x40;                                    // SMP:
                                                  // Input data sampled at middle of data
                                                  // output
                                                  // CKE:
                                                  // Transmit occurs on transition from active
 SSPCON1=0x20;                                    // SSPEN:
 while (1)
 {
 SSPBUF = 0x55;                                   // data 0x55 to be sent out
while (SSPSTATbits.BF == 0);                      // wait for status done
 SSPBUF = 0xaa;                                   // data 0xAA to be sent out
while (SSPSTATbits.BF == 0);                      // wait for status done
for (i=0;i<10;i++);                               // small delay
}
} 

