#include <p18f4321.h>
#include <stdio.h>
#include <math.h>

#pragma config OSC = INTIO2
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOR =OFF

void Do_InIt(void)                         // declares which bits in the ports are inputs(4'b 1111) and outputs(4'b 0000)
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
            for(int i = 0; i<20000; i++){} // empty for loops serves as a counter from 0 to 19999 which takes about a second to complete
        }
void main(void)
{
    char i;
    Do_InIt();
    while(1)
    {
        for(i = 0; i < 8; i++){           // run for loop to display all 8 combinations of rgb led color for PORT D
            PORTD = i << 3;               // shift code 3 bits left so PORT D output displays ON D1 RGB LED
            Delay_one_second();           // delay LED display in order to visually see each LED
        }
        
    }
}




