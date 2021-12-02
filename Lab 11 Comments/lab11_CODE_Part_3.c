
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

#define DS3231_ID       0x68
#define DS3231_Add_00   0x00
#define DS3231_Add_0E   0x0E
#define DS3231_Add_11   0x11

#define ACCESS_CFG      0xAC
#define START_CONV      0xEE
#define READ_TEMP       0xAA
#define CONT_CONV       0x02

#define ACK             1
#define NAK             0


void Setup_RTC_Time(char);
void DS3231_Read_Time(char);
int  DS3231_Read_Temp(char);
void delay_ms(int);
void Initialize_TFT_Screen(void);
void Update_TFT_Screen(void);
void Do_Update_PWM(char);
void INT1_isr(void);
void INT0_isr(void);
void delay_500ms(void);

#define TFT_CS              PORTDbits.RD2                                   // CS going to PORTD bit 2
#define TFT_DC              PORTDbits.RD1                                   // DC going to PORTD bit 1
#define TFT_RST             PORTDbits.RD3                                   // RST going to PORTD bit 3
#define PULSE               PORTCbits.RC2                                   // PULSE going to PORTC bit 2
#define TACH                PORTCbits.RC0                                   // TACH PULSE going to PORTC bit 0
#define setup_sw            PORTAbits.RA0                                   // TIME SWITCH IS NOW AT RA0 instead of RB0


#define TS_1                1                                               // Size of Normal Text
#define TS_2                2                                               // Size of Number Text
#define start_x             2                                               // X location of start text
#define start_y             2                                               // Y location of start text
#define temp_x              28                                              // X location of temp text
#define temp_y              23                                              // Y location of temp text
#define circ_x              40                                              // X location of Celsius circle
#define circ_y              35                                              // Y location of Celsius circle
#define data_tempc_x        15                                              // X location of celsius temp data
#define data_tempc_y        35                                              // Y location of celsius temp data
#define tempc_x             45                                              // X location of celsius temp label
#define tempc_y             35                                              // Y location of celsius temp label
#define cirf_x              95                                              // X location of Fahrenheit circle
#define cirf_y              35                                              // Y location of Fahrenheit circle
#define data_tempf_x        70                                              // X location of Fahrenheit data
#define data_tempf_y        35                                              // Y location of Fahrenheit data
#define tempf_x             100                                             // X location of Fahrenheit label
#define tempf_y             35                                              // Y location of Fahrenheit label
#define time_x              50                                              // X location of time label
#define time_y              58                                              // Y location of time label
#define data_time_x         15                                              // X location of time data
#define data_time_y         70                                              // Y location of time data
#define date_x              50                                              // X location of date label
#define date_y              93                                              // Y location of date label
#define data_date_x         15                                              // X location of date data
#define data_date_y         105                                             // Y location of date data
#define dc_x                15                                              // X location of duty cycle
#define dc_y                130                                             // Y location of duty cycle
#define RPM_x               90                                              // X location of revolutions per minute
#define RPM_y               130                                             // Y location of revolutions per minute
#define data_dc_x           4                                               // X location of duty cycle data
#define data_dc_y           142                                             // Y location of duty cycle data
#define data_RPM_x          75                                              // X location of revolutions per minute data
#define data_RPM_y          142                                             // Y location of revoltuions per minute data
unsigned char dutyCycle = 50;

#define SCL_PIN PORTBbits.RB3
#define SCL_DIR TRISBbits.RB3
#define SDA_PIN PORTBbits.RB4
#define SDA_DIR TRISBbits.RB4
#include "softi2c.inc"

unsigned char i, second, minute, hour, dow, day, month, year, old_second;
int DS3231_tempC, DS3231_tempF;
char SWUP_flag, SWDN_flag;
char duty_cycle;
int rps, RPM;
#include "ST7735_TFT.inc"

void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    	OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    	OSCCON = 0x70;
}

char buffer[31] = " ECE3301L Spring 2019\0";
char *nbr;
char *txt;
char tempC[]        = "25";
char tempF[]        = "77";
char time[]         = "00:00:00";
char date[]         = "00/00/00";
char dc_text[]      = "50%";
char RPM_text[]     = "1800";

void Initialize_TFT_Screen(void) 
{ 
    LCD_Reset();                                                            // Screen reset
    TFT_GreenTab_Initialize();         
    fillScreen(ST7735_BLACK);                                               // Fills background of screen with color passed to it
 
    strcpy(txt, " ECE3301L Spring 2019\0");                                 // Text displayed for class name
    drawtext(start_x , start_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);   // X and Y coordinates of where the text is to be displayed
    strcpy(txt, "Temperature:");                                            // Text displayed for temperature
    drawtext(temp_x  , temp_y , txt, ST7735_MAGENTA, ST7735_BLACK, TS_1);   
    drawCircle(circ_x, circ_y , 2  , ST7735_YELLOW);
    strcpy(txt, "C/");                                                      // Text displayed for Celsius
    drawtext(tempc_x , tempc_y, txt, ST7735_YELLOW , ST7735_BLACK, TS_2); 
    strcpy(txt, "F");                                                       // Text displayed for Fahrenheit
    drawCircle(cirf_x, cirf_y , 2  , ST7735_YELLOW);
    drawtext(tempf_x , tempf_y, txt, ST7735_YELLOW , ST7735_BLACK, TS_2);
    strcpy(txt, "Time");                                                    // Text displayed for time
    drawtext(time_x  , time_y , txt, ST7735_BLUE   , ST7735_BLACK, TS_1);
    strcpy(txt, "Date");                                                    // Text displayed for date
    drawtext(date_x  , date_y , txt, ST7735_RED    , ST7735_BLACK, TS_1);
    strcpy(txt, "DC");                                                      // Text displayed for duty cycle
    drawtext(dc_x    , dc_y   , txt, ST7735_WHITE  , ST7735_BLACK, TS_1);
    strcpy(txt, "RPM");                                                     // Text displayed for revolutions per minute
    drawtext(RPM_x   , RPM_y  , txt, ST7735_WHITE   , ST7735_BLACK, TS_1);  
}


void Update_TFT_Screen(void)
{
    tempC[0]  = DS3231_tempC/10  + '0';                                     // Putting tens digit of Celsius temp into array
    tempC[1]  = DS3231_tempC%10  + '0';                                     // Putting ones digit of Celsius temp into array
    tempF[0]  = DS3231_tempF/10 + '0';                                      // Putting tens digit of Fahrenheit temp into array
    tempF[1]  = DS3231_tempF%10 + '0';                                      // Putting ones digit of Fahrenheit temp into array
    time[0]  = (hour>>4) + '0';                                             // Putting tens digit of hour into array
    time[1]  = (hour & 0x0f) + '0';                                         // Putting ones digit of hour into array
    time[3]  = (minute >> 4)  + '0';                                        // Putting tens digit of minute into array
    time[4]  = (minute & 0x0f) + '0';                                       // Putting ones digit of minute into array
    time[6]  = (second>> 4) + '0';                                          // Putting tens digit of second into array
    time[7]  = (second &0x0f) + '0';                                        // Putting ones digit of second into array
    date[0]  = (month>>4) + '0';                                            // Putting tens digit of month into array
    date[1]  = (month &0x0f) + '0';                                         // Putting ones digit of month into array
    date[3]  = (day>>4) + '0';                                              // Putting tens digit of day into array
    date[4]  = (day &0x0f) + '0';                                           // Putting ones digit of day into array
    date[6]  = (year>>4) + '0';                                             // Putting tens digit of year into array
    date[7]  = (year &0x0f) + '0';                                          // Putting ones digit of year into array
    
    drawtext(15, 35, tempC, ST7735_YELLOW, ST7735_BLACK, 2);                // Draw the Celsius temperature text on screen
    drawtext(70, 35, tempF, ST7735_YELLOW, ST7735_BLACK, 2);                // Draw the Fahrenheit temperature text on screen
    drawtext(15, 70, time, ST7735_CYAN, ST7735_BLACK, 2);                   // Draw the time text on screen
    drawtext(15, 105, date, ST7735_GREEN, ST7735_BLACK, 2);                 // Draw the date text on screen
    drawtext(10, 140, dc_text, ST7735_WHITE, ST7735_BLACK, 2);              // Draw the duty cycle text on screen
    drawtext(70, 140, RPM_text, ST7735_WHITE, ST7735_BLACK, 2);             // Draw the revolutions per minute text on screen
}

void DS3231_Read_Time(char Device)
{
    I2C_Start();                                                            // Start I2C bus
    I2C_Write((Device << 1) | 0);                                           // Write to device telling it to receive 
    I2C_Write(DS3231_Add_00);                                               // Write to Address 00
    I2C_ReStart();                                                          // Restart so it knows that read is coming
    I2C_Write((Device << 1 ) | 1 );                                         // Write to device so it 
    second = I2C_Read(ACK);                                                 // Write seconds
    minute = I2C_Read(ACK);                                                 // Write Minute
    hour = I2C_Read(ACK);                                                   // Write hour
    dow = I2C_Read(ACK);                                                    // Write DOW
    day = I2C_Read(ACK);                                                    // Write day
    month = I2C_Read(ACK);                                                  // Write month
    year = I2C_Read(NAK);                                                   // Write year, NAK means end of info
    I2C_Stop();                                                             // Stop transmission
    delay_ms(50);                                                           // Delay 50ms
}
int DS3231_Read_Temp(char Device)
{
    I2C_Start();                                                            // Start I2C bus
    I2C_Write((Device << 1) | 0);                                           // Write to the device to tell it to receive address
    I2C_Write(DS3231_Add_11);                                               // Decide address in use
    I2C_ReStart();                                                          // Use Start again so device knows it's receiving more info
    I2C_Write((Device<< 1 ) | 1 );                                          // Write again so the slave knows to transmit info
    DS3231_tempC = I2C_Read(NAK);                                           // Read the first byte of the info of address 0x11, Celsius, NAK to stop transmission
    DS3231_tempF = (int)(((DS3231_tempC * 9.0) / 5.0) + 32);                // Calculate F
    I2C_Stop();                                                             // Stop transmission
    delay_ms(50);                          
    
}

void Setup_RTC_Time(char Device)
{
I2C_Start();                                                                // Start I2C bus
    I2C_Write((Device << 1) | 0);                                           // Write to device so it knows to receive address
    I2C_Write(DS3231_Add_00);                                               // Decide what address to use
    I2C_Write(0x00);                                                        // Write to the next byte for times (Second)
    I2C_Write(0x00);                                                        // Minute
    I2C_Write(0x19);                                                        // Hour
    I2C_Write(0x4);                                                         // Dow
    I2C_Write(0x11);                                                        // Day
    I2C_Write(0x4);                                                         // Month
    I2C_Write(0x19);                                                        // Year
    I2C_Stop();                                                             // Stop reading
    delay_ms(50);                                                           // Delay for 50ms
}

void main()
{
    init_UART();                                                            // Initialization
    OSCCON =0x70;                                                           // Set OSCON to 0x70 because of timer
    ADCON1 = 0x0F;                                                          // Set analog pins to digital
    nRBPU = 0;                                                              // Enable PORTB internal pull up resistor 
	TRISA = 0xFF;                                                           // PORTA to all inputs
    TRISB = 0x03;                                                           // PORTB bits 0-1 to inputs and the rest outputs
    TRISC = 0x11;                                                           // PORTC bit 0 and 5 to input and the rest outputs
	TRISD = 0x00;                                                           // PORTD to all outputs
 	TRISE = 0x00;                                                           // PORTE to all outputs
    INTCONbits.INT0IF = 0;                                                  // INT0 IF is in INTCON 
    INTCON3bits.INT1IF = 0;                                                 // INT1 IF is in INTCON3
    INTCONbits.INT0IE = 1;                                                  // INT0 IE is in INTCON
    INTCON3bits.INT1IE = 1;                                                 // INT1 IE is in INTCON3
    INTCON2bits.INTEDG0 = 0;                                                // Edge programming for INT0, INT1 and
    INTCON2bits.INTEDG1= 0;                                                 // INT2 are in INTCON2 
    INTCONbits.PEIE = 1;                                                    // Peripheral interrupt enable
    INTCONbits.GIE = 1;                                                     // Global interrupt enable
    
    I2C_Init(100000);                                                       // Initialize I2C Master with 100KHz clock 
    txt = buffer;    

    Initialize_TFT_Screen();
   while(1){                                                                // Infinite loop where when the push button is pressed, the duty cycle is updated
        if(SWUP_flag){
            if(dutyCycle < 100){                                            // Add 5 when up button pushed
                dutyCycle += 5;
            }
            if(dutyCycle == 100)                                            // Set it to zero if 100
                dutyCycle = 0; 
            SWUP_flag = 0; 
        }
        else if(SWDN_flag){
            if(dutyCycle > 0){                                              // Subtract 5 when down button pushed
                dutyCycle -= 5;
            }
            if(dutyCycle == 0)                                              // Overflow to 95 when zero
                dutyCycle = 95;
            SWDN_flag = 0;
        }
        
        RPM = get_RPS() * 60 * 5;                                           // Calculate RPM from RPS
        printf("rps %d" , get_RPS());                                       // Use TeraTerm to display RPS
        printf("rpm %d" , RPM);
        RPM_text[0] = (RPM/1000) + '0';                                     // Put RPM numbers into array
        RPM_text[1] = (RPM/100)%10 + '0';
        RPM_text[2] = (RPM/10)%10 + '0';
        RPM_text[3] = RPM%10 + '0';
        
        dc_text[0] = (dutyCycle/10) + '0';                                  // Put duty cycle numbers into array
        dc_text[1] = (dutyCycle%10) + '0';
        Do_Update_PWM(dutyCycle);
        
        DS3231_Read_Temp(DS3231_ID);                                        // Read temperature in C
        
        if(setup_sw){
            DS3231_Read_Time(DS3231_ID);                                    // Read time and date
            
            if(second != old_second){                                       // If the seconds have changed, printout new time
                printf("Time: %02x:%02x:%02x\tDate: %02x/%02x/%02x\tTemperature: %dC/%dF\t\r", hour, minute, second, month, day, year, DS3231_tempC, DS3231_tempF);
                old_second = second;
                Update_TFT_Screen();                                        // Send all new data to LCD
            }
        }
        else{                                                               // Reset if setup not on
            second  = 0x00;
            minute  = 0x30;
            hour    = 0x15;
            dow     = 0x05;
            day     = 0x18;
            month   = 0x04;
            year    = 0x19;
            Setup_RTC_Time(DS3231_ID);
        }
   }
}
int get_RPS(void)
{
    TMR1L = 0;                                                             // clear TMR1L to clear the pulse counter
    T1CON = 0x03;                                                          // enable the hardware counter
    PULSE = 1;                                                             // turn on the PULSE signal
    delay_500ms();                                                         // delay 500 msec
    PULSE = 0;                                                             // turn off the PULSE signal
    char RPS = TMR1L;                                                      // read the number of pulse
    T1CON = 0x02;                                                          // disable the hardware counter
    return (RPS);                                                          // return the counter
}

void Do_Update_PWM(char duty_cycle)
{
    float dc_f;
    int dc_I;
    PR2 = 0b00000100 ;                                                     // set the frequency for 25 Khz
    T2CON = 0b00000111 ;                                                   // set timer two
    dc_f = ( 4.0 * duty_cycle / 20.0) ;                                    // calculate factor of duty cycle versus a 25 Khz
                                                                           // signal
    dc_I = (int) dc_f;                                                     // get the integer part
    if (dc_I > duty_cycle) dc_I++;                                         // round up function
    CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
    CCPR1L = (dc_I) >> 2;
}
void interrupt high_priority chkisr() 
{
    if (INTCONbits.INT0IF == 1) INT0_isr();                                // Set interrupt flags
    if (INTCON3bits.INT1IF == 1) INT1_isr();
}

void INT0_isr(){
      INTCONbits.INT0IF = 0;                                               // Reset interrupt flag, but set up flag
      SWUP_flag = 1;
}

void INT1_isr(){
    INTCON3bits.INT1IF = 0;                                                // Reset interrupt flag, but set down flag
    SWDN_flag  = 1;
}

void delay_ms(int ms)
{
#define CPU_CLOCK       _XTAL_FREQ/4
#define COUNT_PER_MS    CPU_CLOCK/1000
#define COUNT_SCALED    COUNT_PER_MS/16
    
    int count;
    count = (0xffff - COUNT_SCALED) - 1;
    count = count * ms;
    
	T0CON = 0x03;                                                          // Timer 0, 16-bit mode, prescaler 1:16
	TMR0L = count & 0x00ff;                                                // set the lower byte of TMR
	TMR0H = count >> 8;                                                    // set the upper byte of TMR
	INTCONbits.TMR0IF = 0;                                                 // clear the Timer 0 flag
	T0CONbits.TMR0ON = 1;                                                  // Turn on the Timer 0

	while (INTCONbits.TMR0IF == 0);                                        // wait for the Timer Flag to be 1 for done
	T0CONbits.TMR0ON = 0;                                                  // turn off the Timer 0
}

void delay_500ms(void)
{
    T0CON = 0x04;                                                          // Timer 0, 16-bit mode, pre scaler 1:32
    TMR0L = 0x79;                                                          // set the lower byte of TMR
    TMR0H = 0x85;                                                          // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                                                 // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                                                  // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);                                        // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                                                  // turn off the Timer 0
}