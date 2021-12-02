#include<P18F4321.inc>
 config OSC = INTIO2
 config WDT = OFF
 config LVP = OFF
 config BOR = OFF
 ORG 0x0000
		; CODE STARTS FROM THE NEXT LINE
START:
 MOVLW 0xFF	; Load W with 0xFF
 MOVWF TRISB	; Set PORT B as all inputs
 MOVLW 0x00	; Load W with 0x00
 MOVWF TRISA	; Make PORTA bits 0-7 as outputs
 MOVLW 0x0F	; Load W with 0x0F
 MOVWF ADCON1	; Set ADCON1
MAIN_LOOP:
BTFSC PORTB, 0	; If Bit 0 of PORTB = 0 skip the next instruction
 GOTO CASEB1	; else go to CASEB1 (PORTB Bit 0 = 1)
CASEB0:
 BSF PORTA, 0	; case PORTB bit 0 = 0, set bit 0 of PORTA
BCF PORTA, 1	; and clear bit 1 of PORTA
 GOTO MAIN_LOOP ; go back to Main Loop
CASEB1:
 BCF PORTA, 0	; case PORTB bit 0 = 1, clear bit 0 of PORT A
 BSF PORTA, 1	;set bit 1 of PORTA
 GOTO MAIN_LOOP ; go back to Loop 
 END

