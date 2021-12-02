#include <p18f4321.h>
#include <stdio.h>
#include <math.h>

#pragma config OSC = INTIO2
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOR =OFF

void Do_InIt(void) 
{
    TRISA = 0x00;
    TRISB = 0xFF;
    TRISC = 0x00;
    TRISD = 0x00;
    ADCON1 = 0x0F;
    //PORTC = 0;
   // PORTD = 0;
         
}

void Delay_one_second(){
            for(int i = 0; i<20000; i++){}                              // empty for loops serves as a counter from 0 to 19999 which takes about a second to complete
        }

      
void main(void)
{
    char array1[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};  // hexadecimal array that contains the color values for D1 starting with no light and ending with white
    char array2[8] = {0x38, 0x30, 0x28, 0x20, 0x18, 0x10, 0x08, 0x00};  // hexadecimal array that contains the color values for D1 starting with white and ending with no light
    char array3[8] = {0x04, 0x01, 0x05, 0x06, 0x07, 0x02, 0x03, 0x00};  // hexadecimal array that contains the color values for D1 starting with blue and ending with no light
    char array4[8] = {0x30, 0x20, 0x18, 0x10, 0x08, 0x38, 0x00, 0x28};  // hexadecimal array that contains the color values for D1 starting with cyan and ending with purple
    char i;                                                             // instantiating the loop variable
    Do_InIt();                                                          // running the function that sets the input and output bits
    while(1)
    {
        if (PORTB == 0){                                                // when the switch is turned off, the set of LED colors from part four(array 1 & 2) blink simultaneously 
            for(i = 0; i < 8; i++){
            PORTC = array1[i];
            PORTD = array2[i];
            Delay_one_second();
            }
        }
        else{                                                           // when the switch is turned on, the set of LED colors from part five(array 2 & 3) blink simultaneously 
            for(i = 0; i < 8; i++){
            PORTC = array3[i];
            PORTD = array4[i];
            Delay_one_second();
            }
        }
       
        
    }
}

