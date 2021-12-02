		; THIS SECOND ASSEMBLY LANGUAGE PROGRAM WILL READ THE VALUES OF
		; ALL THE BITS 0-3 OF PORT A AND OUTPUT THEM
		; TO THE PINS 0 THROUGH 3 OF PORT B
#include<P18F4321.inc>
 config OSC = INTIO2
 config WDT = OFF
 config LVP = OFF
 config BOR = OFF
 ORG 0x0000
START:
 MOVLW 0x0F	; Load W with 0x0F0
 MOVWF ADCON1	; Make ADCON1 to be all digital
 MOVLW 0xFF	; Load W with 0xFF
 MOVWF TRISB	; Set PORT B as all inputs
 MOVLW 0x00	; Load W with 0x00
 MOVWF TRISA    ; Make PORT A as outputs
MAIN_LOOP:
 MOVF PORTB, W	; Read from PORT A and move it into W
 ANDLW 0x0F	; Mask with 0x0F
 MOVWF PORTA	; Move from W to PORT A
 GOTO MAIN_LOOP ; Loop forever
 END 


