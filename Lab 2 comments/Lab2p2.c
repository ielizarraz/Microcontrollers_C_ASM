#include <p18f4321.h>
#include <stdio.h>
#include <math.h>

#pragma config OSC = INTIO2
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOR =OFF

void Do_InIt(void)          // declares which bits in the ports are inputs(4'b 1111) and outputs(4'b 0000)
{
    TRISA = 0x00;
    TRISB = 0xFF;
    TRISC = 0x00;
    TRISD = 0x0F;
    ADCON1 = 0x0F;
    PORTC = 0;
    PORTD = 0;
         
}

void main(void)
{
    Do_InIt();
    TRISA = 0x00;           //set RA<3:0> as outputs
    TRISB = 0x0F;           //set RB<3:0> as inputs
    char in;
    while(1)
    {
        in = PORTB;         //read data from PORTB and save it
                            //into 'in'
        in = in & 0x07;     //Mask out the unused upper four bits
        PORTC = in;         //Output the data to PORTA
    }
}



