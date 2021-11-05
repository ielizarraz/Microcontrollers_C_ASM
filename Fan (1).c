#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h> 

#pragma config OSC      =   INTIO67
#pragma config BOREN    =   OFF
#pragma config WDT      =   OFF
#pragma config LVP      =   OFF
#pragma config CCP2MX   =   PORTBE

#define TS_1 1                                    // Size of Normal Text
#define TS_2 2                                    // Size of Number Text
#define title_txt_X 2                             // X-location of Title Text
#define title_txt_Y 2                             // X-location of Title Text
#define voltage_txt_X 25                          // X-location of Voltage Text
#define voltage_txt_Y 25                          // Y-location of Voltage Text
#define voltage_X 40                              // X-location of Voltage Number
#define voltage_Y 37                              // Y-location of Voltage Number
#define voltage_Color ST7735_BLUE                 // Color of Voltage data
#define dc_txt_X 37                               // X-location of dc text
#define dc_txt_Y 60                               // Y-location of dc text
#define dc_X 52                                   // X-location of dc number
#define dc_Y 72                                   // Y-location of dc number
#define dc_Color ST7735_MAGENTA                   // Color of dc data
#define RPS_txt_X 20                              // X-location of RPS text
#define RPS_txt_Y 95                              // Y-location of RPS text
#define RPS_X 20                                  // X-location of RPS number            
#define RPS_Y 107                                 // Y-location of RPS number
#define RPS_Color ST7735_CYAN                     // Color of RPS data
#define HZ_txt_X 90                               // X-location of HZ text
#define HZ_txt_Y 95                               // Y-location of HZ text
#define HZ_X 75                                   // X-location of HZ number
#define HZ_Y 107                                  // Y-location of Hz number
#define HZ_Color ST7735_CYAN                      // Color of HZ data
#define RPM_txt_X 37                              // X-location of RPM text
#define RPM_txt_Y 130                             // Y-location of RPM text
#define RPM_X 20                                  // X-location of RPM number
#define RPM_Y 142                                 // Y-location of RPM number
#define RPM_Color ST7735_WHITE                    // Color of RPM data

#define _XTAL_FREQ      8000000                   // Set operation for 
#define TMR_CLOCK       _XTAL_FREQ/4              // Timer Clock 2 Mhz
#define COUNT_PER_MS    TMR_CLOCK/1000            // Count per ms = 2000
#define COUNT_SCALED    COUNT_PER_MS/32           // Divinding the count per ms to make the scaled count
#define TFT_RST         PORTBbits.RB4             // Location of TFT Reset
#define TFT_CS          PORTBbits.RB2             // Location of TFT Chip Select
#define TFT_DC          PORTBbits.RB5             // Location of TFT D/C
#define PULSE           PORTDbits.RD0             // Location of the pulse signal
#define TACHPULSE       PORTCbits.RC0             // Location of the tachpulse that comes from the fan
#define PMW             PORTCbits.RC2             // Location of the PWM that goes to the fan
void delay_ms(int);                               // Initializiation of all functions
unsigned int Get_Full_ADC(void);
float Read_Volt_In(void);
void delay_500ms(void);
void init_ADC(void);
void do_init(void);
void init_UART(void);
void putch(char);
void  Initialize_Screen(void);
void do_update_pwm(char);
int get_RPS(void); 

#include "ST7735_TFT.inc"

void putch (char c)                                // TeraTerm function
{
    while (!TRMT);
    TXREG = c;
} 
void delay_ms(int ms)
{
    int count;
    count = (0xffff - COUNT_SCALED) - 1;
    count = count * ms;
 
	T0CON = 0x04;                                 // Timer 0, 16-bit mode, pre scaler 1:32
	TMR0L = count & 0x00ff;                       // set the lower byte of TMR
	TMR0H = count >> 8;                           // set the upper byte of TMR
	INTCONbits.TMR0IF = 0;                        // clear the Timer 0 flag
	T0CONbits.TMR0ON = 1;                         // Turn on the Timer 0

	while (INTCONbits.TMR0IF == 0);               // wait for the Timer Flag to be 1 for done
	T0CONbits.TMR0ON = 0;                         // turn off the Timer 0
}


void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
   USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
   USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
} 
void do_init()
{
    TRISA = 0xFF;                                 // Set TRISA as inputs
    TRISB = 0x00;                                 // Set TRISB as outputs
    TRISC = 0x01;                                 // Set TRISC as all outputs except of the first bit
    TRISD = 0x00;                                 // Set TRISD as all outputs
    TRISE = 0x00;                                 // SET TRISE as all outputs
    PORTD = 0x00;                                 // Set PORTD as all outputs
    
}
void init_ADC(void)
{
    ADCON0=0x09;                                  // select channel AN0, and turn on the ADC subsystem
    ADCON1=0x1B;                                  // select pins AN0 through AN3 as analog signal, VDD-VSS as
    ADCON2=0xA9;                                  // right justify the result. Set the bit conversion time (TAD) and
} 
void delay_500ms(void)
{
    T0CON = 0x04;                                 // Timer 0, 16-bit mode, pre scaler 1:32
    TMR0L = 0xAA;                                 // set the lower byte of TMR
    TMR0H =0x85;                                  // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                        // clear the Timer 0 flag 
    T0CONbits.TMR0ON = 1;                         // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);               // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                         // turn off the Timer 0
} 

char *txt;                                        // Initialize txt variable
char buffer[30] = "";                             // Initialize buffer to empty char by default
char voltage_text[] = "0.0V";                     // Initialize voltage text to 0.0V by default
char dc_text[] = "--%";                           // Initialize dc text to --% by default
char RPS_text[] = "00";                           // Initialize RPS text to 00 by default
char HZ_text[] = "000";                           // Initialize HZ text to 000 by default
char RPM_text[] = "0000 RPM";                     // Initialize RPM text to 0000 RPM by default 

    void main()
{
    init_UART();                                 
    init_ADC();                                   // Set ADCONS using the ADC function
    do_init();                                    // Set TRISs and PORTs using do_init function
    OSCCON = 0x70;                                // Set the OSCON to 0x70
    txt = buffer;                                 // Set the text equal to the buffer
    
    Initialize_Screen();                          // Set up the screen to have the default values
    
    while (1)                                     // Infinite loop
    {   
        float input_voltage = Read_Volt_In();     // Get voltage on AN1

        char iv1 = (int)input_voltage;            // Set the typecasted input voltage to iv1 (no decimal))
        char iv2 = (int)((input_voltage - iv1) * 10); // Set the tycasted decimal of the input voltage to iv2
        voltage_text[0] = iv1 + '0';              // Set the first variable of the voltage txt array to iv1
        voltage_text[2] = iv2 + '0';              // Set the third variable of the voltage txt array to iv2 
        printf("Vin: %f\r\n", input_voltage);     // TeraTerm checking input voltage
        
        int step = Get_Full_ADC();                // Set step equal to ADC voltage
        
        float duty_cycle = (input_voltage/4.096) * 100; // Calculate duty cycle as a float
        char dc1 = (int)(duty_cycle/10);          // Set the tens digit of the duty cycle to dc1
        char dc2 = (((int)duty_cycle)%10);        // Set the ones digit of the duty cycle to dc2
        dc_text[0] = dc1 + '0';                   // Set the first variable of the dc_text array to dc1
        dc_text[1] = dc2 + '0';                   // Set the second variable of the dc text array to dc2
        
        char dc = (char)duty_cycle;               // Make dc a char of the duty cycle calculation
        printf("DC: %d\r\n", dc);                 // TeraTerm for dc char
        do_update_pwm(dc);                        // Generate PWM
        
        char RPS = get_RPS();                     // Measure RPS
        char RPS1 = (RPS/10);                     // Set the tens digit of the RPS to RPS1
        char RPS2 = (RPS%10);                     // Set the ones digit of the RPS to RPS2
        RPS_text[0] = RPS1 + '0';                 // Set the first variable of the RPS text array to RPS1
        RPS_text[1] = RPS2 + '0';                 // Set the second variable of the RPS text array to RPS2
        printf("RPS: %d\r\n", RPS);               // TeraTerm for RPS
        
        char HZ = RPS * 2;                        // Calculate Hz from RPS
        char HZ1 = HZ/100;                        // Set the ones digit of the HZ to HZ1
        char HZ2 = (HZ/10)%10;                    // Set the tens digit of the HZ to HZ2
        char HZ3 = HZ%10;                         // Set the hundreds digit of the HZ to HZ3
        HZ_text[0] = HZ1 + '0';                   // Set the first value of the HZ text array to HZ1
        HZ_text[1] = HZ2 + '0';                   // Set the second value of the HZ text array to HZ2
        HZ_text[2] = HZ3 + '0';                   // Set the third value of the HZ text array to HZ3
        printf("Hz: %d\r\n", HZ);                 // TeraTerm for HZ
        
        int RPM = RPS * 60;                       // Calculate RPM from RPS
        char RPM1 = RPM/1000;                     // Set the ones digit of RPM to PRM1
        char RPM2 = (RPM/100)%10;                 // Set the tens digit of RPM to RPM2
        char RPM3 = (RPM/10)%10;                  // Set the hundreds digit of RPM to RPM3
        char RPM4 = RPM%10;                       // Set the thousands digit of RPM to PRM4
        RPM_text[0] = RPM1 + '0';                 // Set the first variable of RPM text to RPM1
        RPM_text[1] = RPM2 + '0';                 // Set the second variable of RPM text to RPM2
        RPM_text[2] = RPM3 + '0';                 // Set the third variable of RPM text to RPM3
        RPM_text[3] = RPM4 + '0';                 // Set the fourth variable of RPM text to RPM4
        printf("RPM: %d\r\n\r\n", RPM);           // TeraTerm of RPM
    drawtext(voltage_X, voltage_Y, voltage_text, voltage_Color, ST7735_BLACK, TS_2); // Draw the new values of voltage on the screen
    drawtext(dc_X, dc_Y, dc_text, dc_Color, ST7735_BLACK, TS_2); // Draw the new values of dc on the screen
    drawtext(RPS_X, RPS_Y, RPS_text, RPS_Color, ST7735_BLACK, TS_2); // Draw the new values of RPS on the screen
    drawtext(HZ_X, HZ_Y, HZ_text, HZ_Color, ST7735_BLACK, TS_2); // Draw the new values of HZ on the screen
    drawtext(RPM_X, RPM_Y, RPM_text, RPM_Color, ST7735_BLACK, TS_2); // Draw the new values of RPM on the screen
    }
    
}
float step;
float volt;
float Read_Volt_In()                              // Calculates the read in voltage
{
    float volt = (float)(Get_Full_ADC()); 
    return ((volt*4.0)/1000);
}
void do_update_pwm(char duty_cycle)
{
     float dc_f;
     int dc_I;
     PR2 = 0b00000100 ;                           // set the frequency for 25 Khz
     T2CON = 0b00000111 ;                         //
     dc_f = ( 4.0 * duty_cycle / 20.0) ;          // calculate factor of duty cycle versus a 25 Khz
                                                  // signal
     dc_I = (int) dc_f;                           // get the integer part
     if (dc_I > duty_cycle) dc_I++;               // round up function
     CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
     CCPR1L = (dc_I) >> 2;
} 
int get_RPS(void)
{
    TMR1L = 0;                                    // clear TMR1L to clear the pulse counter
    T1CON = 0x03;                                 // enable the hardware counter
    PULSE = 1;                                    // turn on the PULSE signal
    delay_500ms ();                               // delay 500 msec
    PULSE = 0;                                    // turn off the PULSE signal
    char RPS = TMR1L;                             // read the number of pulse
    T1CON = 0x02;                                 // disable the hardware counter
    return (RPS);                                 // return the counter
} 
unsigned int Get_Full_ADC(void)
{
    int result;
    ADCON0bits.GO=1;                              // Start Conversion
    while(ADCON0bits.DONE==1);                    // Wait for conversion to be completed (DONE=0)
    result = (ADRESH * 0x100) + ADRESL;           // Combine result of upper byte and lower byte into
    return result;                                // return the most significant 8- bits of the result.
}


void Initialize_Screen(void)
{
    LCD_Reset();                                  // Clear and reset the LCD
    TFT_GreenTab_Initialize();                    
    fillScreen(ST7735_BLACK);                     // Sets the default screen to a black background
    strcpy(txt, " ECE3301L Spring 2019\0");       // Label the top with the class name
    drawtext(title_txt_X, title_txt_Y, txt, ST7735_WHITE, ST7735_BLACK, TS_1); // Draw initial label

    strcpy(txt, "Input Voltage:");                // Label the input voltage
    drawtext(voltage_txt_X, voltage_txt_Y, txt, voltage_Color, ST7735_BLACK, TS_1); // Draw the voltage label

    strcpy(txt, "Duty Cycle:");                   // Label the duty cycle
    drawtext(dc_txt_X, dc_txt_Y, txt, dc_Color, ST7735_BLACK, TS_1); // Draw the duty cycle label

    strcpy(txt, "RPS:");                          // Label the RPS
    drawtext(RPS_txt_X, RPS_txt_Y, txt, RPS_Color, ST7735_BLACK, TS_1);// Draw the RPS label 
    
    strcpy(txt, "Hz:");                           // Label the Hz
    drawtext(HZ_txt_X, HZ_txt_Y, txt, HZ_Color, ST7735_BLACK, TS_1);// Draw the HZ label
    
    strcpy(txt, "RPM:");                          // Label the RPM
    drawtext(RPM_txt_X, RPM_txt_Y, txt, RPM_Color, ST7735_BLACK, TS_1);// Draw the RPM label
}
