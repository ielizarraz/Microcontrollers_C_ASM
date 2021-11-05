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
#define setup_sw        PORTAbits.RA0

#define DS3231_ID       0x68
#define DS3231_Add_00   0x00
#define DS3231_Add_0E   0x0E
#define DS3231_Add_11   0x11
#define ACK             1
#define NAK             0


void Setup_RTC_Time(char);                             // Prototyping
void DS3231_Read_Time(char);
int  DS3231_Read_Temp(char);
void delay_ms(int);
void putch (char);
void init_UART(void);

#include "i2c.inc"

unsigned char i, second, minute, hour, dow, day, month, year, old_second; 
                                                        // Variable initialization
int DS3231_tempC, DS3231_tempF;

void main()
{
    init_UART();
    OSCCON =0x70;                                       // Set the OSCON to 0x70
    ADCON1 = 0x0F;                
    nRBPU = 0;                                          // Enable PORTB internal pull up resistor
    TRISB = 0x01;                                       // PORT B bit 0 as input
    TRISE = 0x00;


    TRISCbits.RC3 = 0;                                  // PORT C bit 3 as output
    TRISCbits.RC4 = 0;                                  // PORT C bit 4 as output
    TRISEbits.RE0 = 0;

    
        
    I2C_Init(100000);                                   // Initialize I2C Master with 100KHz clock 
  

    while(TRUE)
    {  
          if(setup_sw == 0) 
          {     
            Setup_RTC_Time(DS3231_ID);                  // Initialize the Real Time Clock
          }
          else {
              
            
            DS3231_Read_Time(DS3231_ID);                // Use read time function on the device in Use
            if(second != old_second){                   // Time delay that ensures function only continues after 1 second passes
            DS3231_Read_Temp(DS3231_ID);                // Use the read temp function on the device in Use
            
            printf("\rHour: %02x", hour);               // Set up all of the Labels for the display on TeraTerm
            printf ("  Min: %02x", minute);
            printf("  Sec: %02x", second);
            printf("\tM/D/Y: %02x", month);
            printf("/%02x", day);
            printf("/%02x", year);
            printf("  Degrees C: %d", DS3231_tempC);
            printf("  Degrees F: %d", DS3231_tempF);
            PORTEbits.RE0 = ~PORTEbits.RE0;             // Turn off/on LED
            old_second = second;                        // Time delay that ensures function only continues after 1 second passes
           }
        }
    }

}

void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    	OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    	OSCCON = 0x70;                                  // Set the internal Clock
}

int DS3231_Read_Temp(char Device)
{
    I2C_Start();                                        // Start I2C bus
    I2C_Write((Device << 1) | 0);                       // Write to the device to tell it to receive address
    I2C_Write(DS3231_Add_11);                           // Decide address in use
    I2C_ReStart();                                      // Use Start again so device knows it's receiving more info
    I2C_Write((Device<< 1 ) | 1 );                      // Write again so the slave knows to transmit info
    DS3231_tempC = I2C_Read(NAK);                       // Read the first byte of the info of address 0x11, Celsius, NAK to stop transmission
    DS3231_tempF = (int)(((DS3231_tempC * 9.0) / 5.0) + 32); // Calculate F
    I2C_Stop();                                         // Stop transmission
    delay_ms(50);                                       // Delay for 50ms
}

void Setup_RTC_Time(char Device)
{
    I2C_Start();                                        // Start I2C bus
    I2C_Write((Device << 1) | 0);                       // Write to device so it knows to receive address
    I2C_Write(DS3231_Add_00);                           // Decide what address to use
    I2C_Write(0x00);                                    // Write to the next byte for times (Second)
    I2C_Write(0x00);                                    // Minute
    I2C_Write(0x19);                                    // Hour
    I2C_Write(0x4);                                     // Dow
    I2C_Write(0x11);                                    // Day
    I2C_Write(0x4);                                     // Month
    I2C_Write(0x19);                                    // Year
    I2C_Stop();                                         // Stop reading
    delay_ms(50);                                       // Delay for 50ms
}

void DS3231_Read_Time(char Device)
{
    I2C_Start();                                        // Start I2C bus
    I2C_Write((Device << 1) | 0);                       // Write to device telling it to receive 
    I2C_Write(DS3231_Add_00);                           // Write to Address 00
    I2C_ReStart();                                      // Restart so it knows that read is coming
    I2C_Write((Device << 1 ) | 1 );                     // Write to device so it 
    second = I2C_Read(ACK);                             // Write seconds
    minute = I2C_Read(ACK);                             // Write Minute
    hour = I2C_Read(ACK);                               // Write hour
    dow = I2C_Read(ACK);                                // Write DOW
    day = I2C_Read(ACK);                                // Write day
    month = I2C_Read(ACK);                              // Write month
    year = I2C_Read(NAK);                               // Write year, NAK means end of info
    I2C_Stop();                                         // Stop transmission
    delay_ms(50);                                       // Delay 50ms
}    
 
void delay_ms(int ms)
{
#define CPU_CLOCK       _XTAL_FREQ/4
#define COUNT_PER_MS    CPU_CLOCK/1000
#define COUNT_SCALED    COUNT_PER_MS/16
    
    int count;
    count = (0xffff - COUNT_SCALED) - 1;               // Set count to the difference of the scaled count and 0xFFFF minus 1
    count = count * ms;                                // Count multiplied by ms gives actual delay
    
	T0CON = 0x03;                                      // Timer 0, 16-bit mode, prescaler 1:16
	TMR0L = count & 0x00ff;                            // set the lower byte of TMR
	TMR0H = count >> 8;                                // set the upper byte of TMR
	INTCONbits.TMR0IF = 0;                             // clear the Timer 0 flag
	T0CONbits.TMR0ON = 1;                              // Turn on the Timer 0

	while (INTCONbits.TMR0IF == 0);                    // Wait for the Timer Flag to be 1 for done
	T0CONbits.TMR0ON = 0;                              // Turn off the Timer 0
}


