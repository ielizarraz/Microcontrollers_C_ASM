#include <p18f4321.h>
#include <stdio.h>
#include <math.h>

#pragma config OSC = INTIO2
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOR =OFF

void Do_InIt(void)                                                // declares which bits in the ports are inputs(4'b 1111) and outputs(4'b 0000)
{
    TRISA = 0x00;
    TRISB = 0xFF;
    TRISC = 0x00;
    TRISD = 0x00;
    ADCON1 = 0x0F;
    PORTC = 0;
    PORTD = 0;
         
}

void Delay_one_second(){
            for(int i = 0; i<20000; i++){}                         // empty for loops serves as a counter from 0 to 19999 which takes about a second to complete
        }

char array1[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}; // hexadecimal array that contains the color values for D1 starting with no light and ending with white
char array2[8] = {0x38, 0x30, 0x28, 0x20, 0x18, 0x10, 0x08, 0x00}; // hexadecimal array that contains the color values for D1 starting with white and ending with no light
void main(void)
{
    char i;                                                        // instantiating the loop variable
    Do_InIt();                                                     // running the function that sets the input and output bits
    while(1)
    {
        for(i = 0; i < 8; i++){                                    // looping through the color arrays of each respective LED with a one second delay to change them simultaneously
            PORTC = array1[i];
            PORTD = array2[i];
            Delay_one_second();
            }
        
    }
}