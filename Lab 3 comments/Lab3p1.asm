		    ; THIS FIRST ASSEMBLY LANGUAGE PROGRAM WILL FLASH AN LED CONNECTED
		    ; TO THE PINS 0 THROUGH 3 OF PORT B
#include<P18F4321.inc>
config OSC = INTIO2
config WDT = OFF
config LVP = OFF
config BOR = OFF
		    ; Constant declarations
Delay1 equ 0xFF
Delay2 equ 0xFF
ORG 0x0000
		    ; CODE STARTS FROM THE NEXT LINE
START:
 MOVLW 0x0F	    ; Load W with 0x0F0
 MOVWF ADCON1	    ; Make ADCON1 to be all digital
 MOVLW 0x00	    ; Load W with 0x00
 MOVWF TRISA	    ; Make PORT A as outputs
MAIN_LOOP:
MOVLW 0x0A	    ; Load W with value 0x0A
MOVWF PORTA	    ; Output to PORT A 
		    ; NESTED DELAY LOOP
MOVLW Delay1	    ; Load constant Delay1 into W
MOVWF 0x21	    ; Load W to memory 0x21
LOOP_OUTER_1:
NOP		    ; Do nothing
MOVLW Delay2	    ; Load constant Delay2 into W
MOVWF 0x20	    ; Load W to memory 0x20
LOOP_INNER_1:
NOP		    ; Do nothing
DECF 0x20,F	    ; Decrement memory location 0x20
BNZ LOOP_INNER_1    ; If value not zero, go back to LOOP_INNER_1
DECF 0x21,F	    ; Decrement memory location 0x21
BNZ LOOP_OUTER_1    ; If value not zero, go back to LOOP_OUTER_1
MOVLW 0x05	    ; Load W with value 0x05
MOVWF PORTA	    ; Output to PORT A (flipping the LEDs)
		    ; NESTED DELAY LOOP AGAIN
MOVLW Delay1	    ; Load constant Delay1 into W
MOVWF 0x21	    ; Load W to memory 0x21
LOOP_OUTER_2:
NOP		    ; Do nothing
MOVLW Delay2	    ; Load constant Delay2 into W
MOVWF 0x20	    ; Load W to memory 0x20
LOOP_INNER_2:
 NOP		    ; Do nothing
DECF 0x20,F	    ; Decrement memory location 0x20
BNZ LOOP_INNER_2    ; If value not zero, go back to LOOP_INNER_2
DECF 0x21,F	    ; Decrement memory location 0x21
BNZ LOOP_OUTER_2    ; If value not zero, go back to LOOP_OUTER_2
		    ; START ALL OVER AGAIN
GOTO MAIN_LOOP	    ;Go back to main loop
 END 



