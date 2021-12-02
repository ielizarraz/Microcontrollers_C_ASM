#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>


#pragma config OSC      =   INTIO67
#pragma config BOREN    =   OFF
#pragma config WDT      =   OFF
#pragma config LVP      =   OFF
#pragma config CCP2MX   =   PORTBE
#define Chip_Select PORTDbits.RD0                       // Set the zeroth bit of PORT D to be the chip select
#define Second_LED PORTEbits.RE0                        // Set the zeroth bit of PORT E to be the second LED
void interrupt high_priority T0IRS();                   // Function prototyping
void SPI_out(unsigned char SPI_data); 
void Do_init(); 
char Array[80];
char count = 0;
float angle;
char I; 

void main()
{
 Do_init();                                             // Initialization
 while (1)
    {                                                   // Do nothing,
                                                        // wait for interrupt
    }
} 
void interrupt high_priority T0ISR()
{
     INTCONbits.TMR0IF=0;                               // Clear the interrupt flag
                                                        // reloaded with 60ms
                                                        // T0CON = 0X08; //scalar of
     TMR0H=0xFC;                                        // Reload Timer High and
     TMR0L=0x3D;                                        // Timer Low
     Second_LED = ~Second_LED;                          // flip logic state of
     SPI_out(Array[count]);                             // Call the SPI_out function in the interrupt
     count++;                                           // Increment count
     if(count == 80)                                    // Reset count when it reaches 60
     {
         count = 0; 
     }

}
void SPI_out(unsigned char SPI_data)
{
    char First_byte, Second_byte;                       // Initializing first and second byte variables
    First_byte = (SPI_data & 0xf0) >> 4;                // take the upper nibble of data and >> 4
                                                        // times
     First_byte = 0x30 | First_byte;                    // set the upper nibble with 0x30
     Second_byte = (SPI_data & 0x0f) <<4;               // take the lower nibble of data and << 4 times
    Chip_Select = 0;                                    // set the chip select of the D/A chip to be low
    SSPBUF = First_byte;                                // output the first byte to the SPI bus
    while (SSPSTATbits.BF == 0);                        // wait for status done
    for (I=0;I<1;I++);                                  // small delay
    SSPBUF = Second_byte;                               // output the second byte to the SPI bus
    while (SSPSTATbits.BF == 0);                        // wait for status done
    for (I=0;I<1;I++);                                  // small delay
    Chip_Select = 1;                                    // raise chip select high
} 
void Do_init()
{
     OSCCON=0x70;                                       // Program oscillator to be at 8Mhz
     ADCON1 = 0x0F;                                     // Set ADCON1 to 0F
     TRISC=0x00;                                        // Setup port C with output
     TRISDbits.TRISD0 = 0;                              // Set zeroth bit of PORTD as output
     TRISEbits.TRISE0 = 0;                              // Set zeroth bit of PORTE as output
     SSPSTAT=0x40;                                      // SMP:
                                                        // Input data sampled at middle of data output
                                                        // CKE:
                                                        // Transmit occurs on transition from active
     SSPCON1=0x20;                
     for(I = 0; I < 80; I++)                            // Loop through all values of the array
     {
         if (I <= 40)                                   // For each value in the first half of the array, multiply it by 1.6 (calculated from hex value) then store it as an integer
         {
            Array[I] = (int)(1.6 * I);
         }
         else
         {
             Array[I] = (int)(Array[80-I]);             // For each value in the second half of the array, mirror the first half of the array in order to make it symmetrical
         }
     }
                                                        // Enables serial port and configures SCK, ?
     T0CON=0x08;                                        // Set Timer 0 in Timer mode, no scalar
     TMR0H=0xF8;                                        // Program Timer High byte changed for a 60ms period
     TMR0L=0x2F;                                        // Program Timer Low byte
     INTCONbits.TMR0IE=1;                               // Enable Timer 0 interrupt
     INTCONbits.TMR0IF=0;                               // Clear Timer 0 Interrupt Flag
     INTCONbits.GIE=1;                                  // Enable Global Interrupt
     T0CONbits.TMR0ON=1;                                // Turn on Timer 0 

}


