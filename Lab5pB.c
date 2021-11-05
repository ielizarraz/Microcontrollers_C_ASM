#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h> 

#pragma config OSC=INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN=OFF
#pragma config CCP2MX = PORTBE 

#define RED PORTBbits.RB0                               // Initialize portB bit 0 to D1
#define GREEN PORTBbits.RB1                             // Initialize portB bit 1 to D1
#define BLUE PORTBbits.RB2                              // Initialize portB bit 2 to D1
#define DP PORTDbits.RD7                                // Set the decimal point of the seven segment deisplay to port D7

float volt, temp, RL;                                   // Initialize local variables
int step;

char array1[10] = {0xC0, 0xF9, 0xA4,0xB0
,0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};                   // Ordered values of the numbers on the seven segment display

void Do_InIt(void) 
{
    TRISA = 0xFF;                                       // Set port A to an input
    TRISB = 0x00;                                       // Set port B to an output
    TRISC = 0x00;                                       // Set port C to an output
    TRISD = 0x00;                                       // Set port D to an output
    TRISE = 0x0F;                                       // Set port E to an output
    ADCON1 = 0x19;
    ADCON2 = 0xA9;
}
void putch (char c)
{
 while (!TRMT);                                         //TeraTerm access call
 TXREG = c;
}

void Do_LED1(float R){
   int u,l,D;
   if (R < 10.0)
    {
        
        u = (int) R;                                    // Finding the value of the most significant number of the temperature and setting it to u
        l = (int)((R-u) *10);                           // Finding the value of the least significant number of the temperature and setting it to l
        D = 0;                                          // Setting the decimal to zero because the number is greater than 10k
    }                                      
    else 
    {
        u = ((int) (R)) / 10;                           // Finding the value of the most significant number of the temperature and setting it to u
        l = ((int)(R)) %10;                             // Finding the value of the least significant number of the temperature and setting it to l
        D =1;                                           // Setting the decimal to one because the number is less than 10k
    }
    PORTC = array1[u];                                  // Setting the values of port C to the value at u on the seven segment display
    PORTD = array1[l];                                  // Setting the values of port D to the value at l on the seven segment display
    DP = D;                                             // Toggling the decimal point at D7 depending on the value of D
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
void SET_RED()                                          // Output the color red to D1 by turning on the first bit of port B
{
RED = 1;
GREEN = 0;
BLUE = 0;
}
void SET_YELLOW()                                       // Output the color yellow to D1 by turning on the first and second bit of port B
{
RED = 1;
GREEN = 1;
BLUE = 0;
} 

void SET_BLUE()                                         // Output the color blue to D1 by turning on the third bit of port B
{
RED = 0;
GREEN = 0;
BLUE = 1;
}
void SET_GREEN()                                        // Output the color green to D1 by turning on the second bit of port B
{
RED = 0;
GREEN = 1;
BLUE = 0;
} 

void SET_WHITE()                                        // Output the color white to D1 by turning on the first three bit of port B
{
RED = 1;
GREEN = 1;
BLUE = 1;
}
void SET_CYAN()                                         // Output the color cyan to D1 by turning on the second and third bit of port B
{
RED = 0;
GREEN = 1;
BLUE = 1;
} 

void SET_PURPLE()                                       // Output the color purple to D1 by turning on the first and third bit of port B
{
RED = 1;
GREEN = 0;
BLUE = 1;
}
void SET_NOLIGHT()                                      // Output no color to D1 by turning off the first three bits of port B
{
RED = 0;
GREEN = 0;
BLUE = 0;
} 

void SEL_ANX(char ch){
    ADCON0 = ch * 4 + 1;                                // Equation for selecting the ADCON0 based on the pin resistance being used
}
void gen_1Khz_beep (void)
{
PR2 = 0xF9;                                             // Values for 1Khz beep
CCPR2L = 0x52;
CCP2CON = 0x1C;
T2CON = 0x05;                                           // Turn T2 on here
}
void gen_2Khz_beep (void)
{
PR2 = 0x7C;                                             // Values for 2Khz beep
CCPR2L = 0x3E; 
CCP2CON = 0x1C;
T2CON = 0x05;                                           // Turn T2 on here
}

void turn_off_buzzer()
{
 CCP2CON = 0;                                           // Value for turning the buzzer off
 PORTBbits.RB3=0;
} 
void Do_RGB(float RL)
{
    float temp; 
    temp = RL*1000.0;                                   // Multiplying the RL by 1000 to get the resistance
    if (temp < 10000){
        SET_NOLIGHT();                                  // If the resistance is below 10000, turn the RGB off
    }
    else if (temp < 20000){
        SET_RED();                                      // If the resistance is below 20000 and above 10000, turn the RGB to red
    }
    else if (temp < 30000){
        SET_GREEN();                                    // If the resistance is below 30000 and above 20000, turn the RGB to green
    }
    else if (temp < 40000){
        SET_YELLOW();                                   // If the resistance is below 40000 and above 30000, turn the RGB to yellow
    }
    else if (temp < 50000){
        SET_BLUE();                                     // If the resistance is below 50000 and above 40000, turn the RGB to blue
    }
    else if (temp < 60000){
        SET_PURPLE();                                   // If the resistance is below 60000 and above 50000, turn the RGB to purple
    }
    else if (temp < 70000){
        SET_CYAN();                                     // If the resistance is below 70000 and above 60000, turn the RGB to cyan
    }
    else
    {
        SET_WHITE();                                    // If the resistance is above 70000, turn the RGB to white
    }
}
void main(void)
{
    Do_InIt();                                          // Calls the function that defines the ports as inputs or outputs
    init_UART();                                        // Calls the function that allows TeraTerm to display the temperature
    while(1){                                           // Infinite loop
        SEL_ANX(1);                                     // Set ADCON1 to selected pin by calling the function
        step = Get_Full_ADC();                          // Get the step by calling the ADC function
        volt = step * 4.0 / 1000.0;                     // Multiply the step by four and divide it by 1000 and set it equal to volt
        RL = (volt / (4.096-volt)) * 0.1;               // Using circuit analysis, find the equivalent load resistance
        Do_RGB(RL);                                     // Set the RGB LED according to the resistance by calling the function
        printf("RL = %f\r\n", RL);                      // Output the resistance to TeraTerm
        Do_LED1(RL);                                    // Output the resistance to the 7 segment display
        if(RL > 0.1)                                    // Buzz accordingly based on the resistance
        {
            turn_off_buzzer();
        }
        else if (RL>=0 && RL < 0.01){
            gen_2Khz_beep();
        }
        else{
            gen_1Khz_beep();
        }
    }
}

