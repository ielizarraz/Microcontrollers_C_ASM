 #include<P18F4321.inc>
 config OSC = INTIO2
 config WDT = OFF
 config LVP = OFF
 config BOR = OFF
Color_PORTC equ 0X08  ;Load port C with a color switch
Color_PORTD equ 0x07  ;Load port C with color white
Color_Off equ 0x00    ;Hex address to turn a color off
OUTER_VALUE equ 0x0E  ;Proper value to get a period of 30ms
INNER_VALUE equ 0x2B  ;Proper value to get a period of 30ms
 ORG 0x0000
		      ; CODE STARTS FROM THE NEXT LINE
START:
 MOVLW 0x0F	      ;Load register W with 0F
 MOVWF ADCON1	      ;Allocate 0F to ADCON1
 MOVLW 0xFF	      ;Load register W with 0F
 MOVWF TRISB	      ;Allocate FF to TRISB
 MOVLW 0X00	      ;Load register W with 00
 MOVWF TRISC	      ;Make PORT C bits 0-7 as outputs
 MOVLW 0X00	      ;Load reigster W with 00
 MOVWF TRISD	      ;Make PORT D bits 0-7 as outputs
MAIN_LOOP:
    
   
   

 MOVLW OUTER_VALUE    ; Load OUTER_VALUE into W
 MOVWF 0x24	      ; save it o register 0x24
 MOVLW INNER_VALUE    ; Load INNER_VALUE into W
 MOVWF 0x25	      ; save it to register 0x25 
  BTFSC PORTB,1       ; If port B1 is 1 go to Port PBB1EQ1, else skip if clear (0)
    GOTO PBB1EQ1      ; Save space to tell program whether to go to white or blue
    GOTO PBB1EQ0      ; Save space to tell program whether to go to green or red
 PBB1EQ1:
    BTFSC PORTB,0     ; Skip if port B is set to high
    GOTO WHITE        ; Go to memory space saved for White color
    GOTO BLUE	      ; Go to memory space saved for Blue color
    
 PBB1EQ0:
    BTFSC PORTB,0     ; Skip if port B is set to high
    GOTO GREEN        ; Go to memory pace saved for green color
    GOTO RED          ; Go to memory space saved for red color
    
    RED:
    MOVLW 0x01	      ; Load register W with 01H (color red)
    MOVWF 0x22	      ; Move 0x01 to memory space 0x22 
    MOVLW 0x38        ; Load register W with 38H (color white)
    MOVWF 0X23	      ; Move 38H to register address 23H
    MOVLW OUTER_VALUE ; Load register with allocated space for OUTER_VALUE
    MOVWF 0X24        ; Move to register space 24H
    MOVLW INNER_VALUE ; Load register with allocated space for INNER_VALUE
    MOVWF 0X25        ; Move to register space 25H
    GOTO COLOR_LOOP   ; Go to color loop to execute the rest of the code
    
    GREEN:
    MOVLW 0x02	      ; Load register W with 01H (color green)
    MOVWF 0x22	      ; Move 0x01 to memory space 0x22 
    MOVLW 0x38        ; Load register W with 38H (color white)
    MOVWF 0X23	      ; Move 38H to register address 23H
    MOVLW OUTER_VALUE*2 ; Load register with allocated space for OUTER_VALUE and multiply by two to increase period time by factor of 2
    MOVWF 0X24        ; Move to register space 24H
    MOVLW INNER_VALUE ; Load register with allocated space for INNER_VALUE
    MOVWF 0X25        ; Move to register space 25H
    GOTO COLOR_LOOP   ; Go to color loop to execute the rest of the code
    
    BLUE:
    MOVLW 0x04;       ; Load register W with 04H (color blue)
    MOVWF 0x22	      ; Move 0x01 to memory space 0x22 
    MOVLW 0x38        ; Load register W with 38H (color white)
    MOVWF 0X23	      ; Move 38H to register address 23H
    MOVLW OUTER_VALUE*4 ; Load register with allocated space for OUTER_VALUE and multiply by 4 to increase period time by factor of 4
    MOVWF 0X24        ; Move to register space 24H
    MOVLW INNER_VALUE ; Load register with allocated space for INNER_VALUE
    MOVWF 0X25        ; Move to register space 25H
    GOTO COLOR_LOOP   ; Go to color loop to execute the rest of the code
    
    WHITE:
    MOVLW 0x07        ; Load register W with 07H (color WHITE)
    MOVWF 0x22	      ; Move 0x01 to memory space 0x22 
    MOVLW 0x38        ; Load register W with 38H (color white)
    MOVWF 0X23	      ; Move 38H to register address 23H
    MOVLW OUTER_VALUE*8 ; Load register with allocated space for OUTER_VALUE andm ultiply by 8 to increase period time by factor of 8
    MOVWF 0X24        ; Move to register space 24H
    MOVLW INNER_VALUE ; Load register with allocated space for INNER_VALUE
    MOVWF 0X25        ; Move to register space 25H
    GOTO COLOR_LOOP   ; Go to color loop to execute the rest of the code 
    
    
    
COLOR_LOOP:
 MOVFF 0x22,PORTC     ; Get saved color of PORTC and output to that Port
 MOVFF 0x23,PORTD     ; Get saved color of PORTD and output to that Port
 MOVFF 0x24,0x21      ; Copy saved outer loop cnt from 0x24 to 0x21
		      ; NESTED DELAY LOOP TO HAVE THE FIRST HALF OF WAVEFORM
LOOP_OUTER_1:
 NOP		      ; Do nothing
 NOP		      ;
 NOP		      ;
 NOP		      ;
 NOP		      ;
 NOP		      ; Preivous nops to increase period of time for loops
 MOVFF 0x25,0x20      ; Load saved inner loop cnt from 0x25 to 0x20
LOOP_INNER_1:
 NOP		      ; Do nothing
 NOP		      ;
 NOP		      ; Previous nops to increase period of time for loops
 DECF 0x20,F	      ; Decrement memory location 0x20
 BNZ LOOP_INNER_1     ; If value not zero, go back to LOOP_INNER_1
 DECF 0x21,F	      ; Decrement memory location 0x21
 BNZ LOOP_OUTER_1     ; If value not zero, go back to LOOP_OUTER_1
 MOVLW Color_Off      ; Load W with the second desired color
 MOVWF PORTC	      ; Output to PORT C to turn off the RGB LED D1
 MOVWF PORTD	      ; Output to PORT D to turn off the RGB LED D2
 MOVFF 0x24,0x21      ; Copy saved outer loop cnt from 0x24 to 0x21
		      ; NESTED DELAY LOOP TO HAVE THE FIRST HALF OF WAVEFORM BEING LOW
LOOP_OUTER_2:
 NOP		      ; Do nothing
 NOP		      ;
 NOP		      ;	
 NOP		      ;
 NOP		      ;
 NOP		      ;
 MOVFF 0x25,0x20      ; Load saved inner loop cnt from 0x25 to 0x20
LOOP_INNER_2:
 NOP		      ; Do nothing
 NOP		      ;
 NOP		      ;
 DECF 0x20,F	      ; Decrement memory location 0x20
 BNZ LOOP_INNER_2     ; If value not zero, go back to LOOP_INNER_2
 DECF 0x21,F	      ; Decrement memory location 0x21
 BNZ LOOP_OUTER_2     ; If value not zero, go back to LOOP_OUTER_2
		      ; START ALL OVER AGAIN
 GOTO MAIN_LOOP	      ; Go back to main loop
 END
 
 