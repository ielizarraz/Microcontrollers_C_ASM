#include <p18f4321.h>
#include <stdio.h>
#include <math.h>
#include <usart.h> 

#pragma config OSC = INTIO2
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOR =OFF

#define RED PORTBbits.RB0                               // Initialize portB bit 0 to D1
#define GREEN PORTBbits.RB1                             // Initialize portB bit 1 to D1
#define BLUE PORTBbits.RB2                              // Initialize portB bit 2 to D1
#define RED2 PORTBbits.RB3                              // Initialize portB bit 3 to D2
#define GREEN2 PORTBbits.RB4                            // Initialize portB bit 4 to D2
#define BLUE2 PORTBbits.RB5                             // Initialize portB bit 5 to D2

float volt, temp;
int step;
char array1[10] = {0xC0, 0xF9, 0xA4,0xB0,
0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};                    // Ordered values of the numbers on the seven segment display

void Do_InIt(void) 
{
    TRISA = 0xFF;                                       // Set port A to an input
    TRISB = 0x00;                                       // Set port B to an output
    TRISC = 0x00;                                       // Set port C to an output
    TRISD = 0x00;                                       // Set port D to an output
    TRISE = 0x00;                                       // Set port E to an output
}
void Do_LED(float temp){
    int u, l, itemp;                                    // Initializing local variables for the function
    itemp = (int)temp;                                  // Making a copy of the temperature as an integer
    u = itemp/10;                                       // Finding the value of the most significant number of the temperature and setting it to u
    l = itemp %10;                                      // Finding the value of the least significant number of the temperature and setting it to l
    PORTC = array1[u];                                  // Setting the values of port C(the tens place of the temperature on the seven segment display) to the value at u
    PORTD = array1[l];                                  // Setting the values of port D(the ones place of the temperature on the seven segment display) to the value at l
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

void SET_RED2()                                         // Output the color red to D2 by turning on the fourth bit of port B
{
RED2 = 1;
GREEN2 = 0;
BLUE2 = 0;
}
void SET_YELLOW2()                                      // Output the color yellow to D2 by turning on the fourth and fifth bit of port B
{
RED2 = 1;
GREEN2 = 1;
BLUE2 = 0;
} 

void SET_BLUE2()                                        // Output the color blue to D2 by turning on the sixth bit of port B
{
RED2 = 0;
GREEN2 = 0;
BLUE2 = 1;
}
void SET_GREEN2()                                       // Output the color green to D2 by turning on the fifth bit of port B
{
RED2 = 0;
GREEN2 = 1;
BLUE2 = 0;
} 

void SET_WHITE2()                                       // Output the color white to D2 by turning on the fourth, fifth and sixth bit of port B
{
RED2 = 1;
GREEN2 = 1;
BLUE2 = 1;
}
void SET_CYAN2()                                        // Output the color cyan to D2 by turning on the fifth and sixth bit of port B
{
RED2 = 0;
GREEN2 = 1;
BLUE2 = 1;
} 

void SET_PURPLE2()                                      // Output the color purple to D2 by turning on the fourth and sixth bit of port B
{
RED2 = 1;
GREEN2 = 0;
BLUE2 = 1;
}
void SET_NOLIGHT2()                                     // Output no color to D2 by turning off the fourth, fifth and sixth bit of port B
{
RED2 = 0;
GREEN2 = 0;
BLUE2 = 0;
} 
void Do_RGB(float temp)
{
    switch ((int)temp/10){                              // Setting up a switch-case of D2 where it changes depending on the value of the tens place in the temperature
            case 0:                                     // At a temperature that is less than ten, D2 is turned off
                SET_NOLIGHT2();
                break;
            case 1:                                     // At a temperature equal to ten and less than twenty, D2 is set to red
                SET_RED2();
                break;
            case 2:                                     // At a temperature greater than or equal to twenty and less than thirty, D2 is set to green
                SET_GREEN2();
                break;
            case 3:                                     // At a temperature greater than or equal to thirty and less than forty, D2 is set to yellow
                SET_YELLOW2();
                break;
            case 4:                                     // At a temperature greater than or equal to forty and less than fifty, D2 is set to blue
                SET_BLUE2();
                break;
            case 5:                                     // At a temperature greater than or equal to fifty and less than sixty, D2 is set to purple
                SET_PURPLE2();
                break;
            case 6:                                     // At a temperature greater than or equal to sixty and less than seventy, D2 is set to cyan
                SET_CYAN2();
                break;
            default:                                    // At a temperature greater than or equal to seventy, D2 is set to white
                SET_WHITE2();
                break;
            
    }
    if (temp < 70){SET_NOLIGHT();}                      // At a temperature that is less than seventy, D1 is turned off
    else if (temp < 75 && temp > 70){SET_RED();}        // At a temperature greater than or equal to seventy and less than seventy-five, D1 is set to red
    else if (temp < 79 && temp > 74){SET_GREEN();}      // At a temperature greater than or equal to seventy-five and less than seventy-nine, D1 is set to green
    else if (temp < 84 && temp > 78){SET_BLUE();}       // At a temperature greater than or equal to seventy-eight and less than eighty-four, D1 is set to blue
    else {SET_WHITE();}                                 // At a temperature greater than or equal to eighty-five, D1 is set to white
}
void main(void)
{
    Do_InIt();                                          // Calls the function that defines the ports as inputs or outputs
    init_UART();                                        // Calls the function that allows TeraTerm to display the temperature
    while(1){                                           // Infinite loop
        ADCON0 = 0x01;                                  // Select channel AN0, and turn on the ADC subsystem 
        ADCON1 = 0x1B;                                  // Select pins AN0 through AN3 as analog signal, VDD-VSS as reference voltage
        ADCON2 = 0xA9;                                  // Set the bit conversion time (TAD) and acquisition time
        step = Get_Full_ADC();                          // Set step to the analog voltage
        volt = step * 4.0;                              // Multiply the step by four and set it equal to volt
        temp = volt/10.0;                               // Temperature is set to the the two most significant digits of the voltage
        printf("temp = %f\r\n", temp);                  // Output the temperature to TeraTerm
        Do_LED(temp);                                   // Turn on the seven segment display with the temperature as the input
        Do_RGB(temp);                                   // Turn on D1 and D2 with the temperature at the input
        ADCON0 = 0x09;                                  // Select channel and turn on the ADC subsystem 
        step = Get_Full_ADC();                          // Set step to analog voltage
        volt = (step* 4.0)/1000.0;                      // Set voltage equal to step divided by 250
        if (volt < 1.9)                                 // If the voltage is less than 1.9 volts, set port E equal to 0x06
            PORTE = 0x06;                               // Set port E to red
        else if (volt < 2.5 && volt >=1.9)              // If the voltage is less than 2.5 volts and greater than or equal to 1.9 volts, set port E equal to 0x09
            PORTE = 0x09;                               // Set port E to green
        else                                            // If the voltage is greater than 2.5 volts, set port E equal to 0x08
            PORTE = 0x08;                               // Set port E to yellow
    }
}