#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h> 

#pragma config OSC=INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN=OFF


#define DP PORTDbits.RD7                                // Set the decimal point of the seven segment deisplay to port D7

float volt, temp;                                       // Initialize local variables
int step;
char array1[10] = {0xC0, 0xF9, 0xA4,0xB0,0x99,
0x92, 0x82, 0xF8, 0x80, 0x90};                          // Ordered values of the numbers on the seven segment display

void Do_InIt(void) 
{
    TRISA = 0xFF;                                       // Set port A to an input
    TRISB = 0x00;                                       // Set port B to an output
    TRISC = 0x00;                                       // Set port C to an output
    TRISD = 0x00;                                       // Set port D to an output
    TRISE = 0x00;                                       // Set port E to an output
    ADCON1 = 0x17;                                      // Set ADCON1 to channel 0x17
    ADCON2 = 0xA9;                                      // Set ADCON2 to channel 0xA9
}
void putch (char c)
{
 while (!TRMT);                                         // Allow TeraTerm access
 TXREG = c;
}

void Do_LED(float v){
    int u,l;
    u = (int) v;                                        // Finding the value of the most significant number of the temperature and setting it to u
    l = (int)((v-u) *10);                               // Finding the value of the least significant number of the temperature and setting it to l
    PORTC = array1[u];                                  // Setting the values of port C to the value at u on the seven segment display
    PORTD = array1[l];                                  // Setting the values of port D to the value at l on the seven segment display
    DP = 0;
}
unsigned int Get_Full_ADC(void)
{
    int result;
    ADCON0bits.GO=1;                                    // Start Conversion
    while(ADCON0bits.DONE==1);                          // Wait for conversion to be completed (DONE=0)
    result = (ADRESH * 0x100) + ADRESL;                 // Combine result of upper byte and lower byte into
    return result;                                      // return the most significant 8- bits of the result.
}
void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & 
    USART_CONT_RX &USART_BRGH_HIGH, 25);                // Linking in peripheral library
    OSCCON = 0x60;                                      // Set address of internal oscillator
} 

void SEL_ANX(char ch){
    ADCON0 = ch * 4 + 1;                                // Equation for selecting the ADCON0 based on the pin resistance being used
}

void main(void)
{
    Do_InIt();                                          // Calls the function that defines the ports as inputs or outputs
    init_UART();                                        // Calls the function that allows TeraTerm to display the temperature
    while(1){                                           // Infinite loop
        SEL_ANX(0);                                     // Set ADCON1 for channel 0
        step = Get_Full_ADC();                          // Get the step by calling the ADC function
        volt = step * 4.0 / 1000;                       // Multiply the step by four and set it equal to volt
        printf("volt = %f\r\n", volt);                  // Output the voltage to TeraTerm
        Do_LED(volt);                                   // Call the function to display the voltage on the seven segment display
    }
}
