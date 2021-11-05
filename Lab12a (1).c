#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>

#pragma config OSC = INTIO67
#pragma config BOREN =OFF
#pragma config WDT=OFF
#pragma config LVP=OFF

#define _XTAL_FREQ      8000000        
 
void TIMER1_isr(void);                              // Prototyping 
void INT0_isr(void); 
void INT2_ISR(void);
unsigned char nec_state = 0; 
unsigned char i; 
unsigned int bit_count;
short nec_ok = 0; 
unsigned long long nec_code; 
 
void putch (char c) 
{        
    while (!TRMT);            
    TXREG = c; 
} 
 

 void init_UART()
 {      
     OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
     OSCCON = 0x70; 
 } 
 
void interrupt high_priority chkisr()  
{     
    if (PIR1bits.TMR1IF == 1) TIMER1_isr();         // If the timer 1 flag is high, run the timer1 function
    if (INTCON3bits.INT2IF == 1) INT2_ISR();        // If the timer 2 flag is high, run the timer2 function
} 
 
void TIMER1_isr(void) 
{   
    nec_state = 0;                                  // Reset decoding process   
    INTCON2bits.INTEDG2 = 0;                        // Edge programming for INT2 falling edge   
    T1CONbits.TMR1ON=0;                             // Disable T1 Timer   
    PIR1bits.TMR1IF=0;                              // Clear interrupt flag 
} 

void INT2_ISR(void) 
{  
    unsigned int timer;    
    INTCON3bits.INT2IF = 0;                         // Clear external interrupt 
 
   if (nec_state != 0)    
   {      
   timer = (TMR1H << 8) | TMR1L;                    // Store Timer1 value      
   TMR1H = 0;                                       // Reset Timer1      
   TMR1L = 0;      
   }   
   switch(nec_state)    
    {      
        
    case 0 :                                        // First detection of leading pulse 
        TMR1H=0;                                    // Clear Timer 1
        TMR1L=0;
        T1CON=0x90;                                 // System clockk at 8Mhz
        T1CONbits.TMR1ON=1;                         // Enable timer 1
        bit_count=0;                                // Force bit count to 0
        nec_code=0;                                 // Reset nec_code
        INTCON2bits.INTEDG2 = 1;                    // Change edge interrupt of INT2 to Low to High
        nec_state=1;                                // Set nec_state to 1
        return;                                     // Return
    case 1:                                         // If nec_state is 1
        if ((timer <= 9500) && (timer >= 8500))     // Checking if timer is between 8500usec and 9500usec
        {
            INTCON2bits.INTEDG2 = 0;                // Change edge interrupt of INT2 to high to low
            nec_state=2;                            // Set nec_state to 2
        }
        else {
            nec_state=0;                            // If not in range, reset nec_state
        }
        return;                                     // Return
   case 2:                                          // If nec_state is 2
        if ((timer<= 5000) && (timer>= 4000))       // Check if timer1 value is between 4000usec and 5000usec
        {
            INTCON2bits.INTEDG2 = 1;                // Change edge interrupt of INT2 to low to high
            nec_state=3;                            // Set nec_state to 4
        }
        else {
            nec_state=0;                            // Reset nec_state to zero
        }
        return;                                     // Return
    case 3:                                         // If nec_state is 3
        if ((timer>=400) && (timer <= 700))         // Check if timer1 value is between 400usec and 700usec
        {
            INTCON2bits.INTEDG2 = 0;                // Change edge interrupt of INT2 to low to high
            nec_state=4;                            // Set nec_state to 4
        }
        else {
            nec_state=0;                            // Reset nec_state to zero
        }
        return;
   case 4:                                          // If nec_state is 4
        if ((timer >= 400) && (timer <= 1800))      // check if timer1 is between 400usec and 700usec
        {
            nec_code= nec_code << 1;                // Shift nec_code to the left by 1 bit
        
            if (timer > 1000)                       // Check if timer1 is greater than 1000usec
            {
                nec_code = nec_code + 1;            // If it is, increment nec_code
            }
            bit_count++;                            // Increment bit_count
            if (bit_count > 31)                     // Check if bit_count is greater than 31
            {           
                nec_ok = 1;                         // If it is, set nec_ok to 1
                INTCON3bits.INT2IE = 0;             // Set INT2 interrupt enable to zero
            }
            else 
            {
                INTCON2bits.INTEDG2 = 1;            // If bit count is smaller than 31, change INT2 to high to low
                nec_state = 3;                      // Go back to state 3 by setting nec_state to 3
            }
        }
        else {
            nec_state = 0;                          // If no condition above, reset nec_state
        }
    }
        return;                                     // Return
  }


void main() 
{     
    init_UART();       
    OSCCON = 0x70;                                  // 8 Mhz    
    nRBPU = 0;                                      // Enable PORTB internal pull up resistor  
    TRISA = 0x01;                                   // Set PORTA bit 0 to input and the rest as outputs
    TRISB = 0x07;                                   // Set PORTB bit 0-2 as inputs and the rest as outputs
    TRISC = 0x01;                                   // Set PORTC bit 0 as inputs and the rest as outputs
    TRISD = 0x00;                                   // Set PORTD as all outputs
    TRISE = 0x00;                                   // Set PORTE as all outputs
    ADCON1 = 0x0F;                                  // Set analog/digital conversion
 
   INTCON3bits.INT2IF = 0;                          // Clear external interrupt    
   INTCON2bits.INTEDG2 = 0;                         // Edge programming for INT0 falling edge H to L    
   INTCON3bits.INT2IE = 1;                          // Enable external interrupt      
   TMR1H = 0;                                       // Reset Timer1    
   TMR1L = 0;                                       //    
   PIR1bits.TMR1IF = 0;                             // Clear timer 1 interrupt flag      
   PIE1bits.TMR1IE = 1;                             // Enable Timer 1 interrupt       
   INTCONbits.PEIE = 1;                             // Enable Peripheral interrupt      
   INTCONbits.GIE = 1;                              // Enable global interrupts    
   nec_ok = 0;                                      // Clear flag    
   nec_code = 0x0;                                  // Clear code          
   while(1)    
   {         
       if (nec_ok == 1)       
       {                
       nec_ok = 0;                                  // Reset nec_ok after finishing cases
       printf ("NEC_Code = %08lx \r\n", nec_code);  // TeraTerm output
       INTCON3bits.INT2IE = 1;                      // Enable external interrupt               
       INTCON2bits.INTEDG2 = 0;                     // Edge programming for INT0 falling edge       
       }    
    } 
}