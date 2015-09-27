	AREA mycode, CODE ; must be indented
	EXPORT quiz1 ; must be indented
quiz1 
	; Mov value of R7 into R9
	MOV R7, #0xFF
	MOV R9, R7
	; Mov value at the address stored in R11 into R10
value DCD 0xFF
	LDR R11, =value
	LDR R10, [R11]
	; Move the value stored at 8 bytes after the address stored in R7 into R8
value2 DCD 0x12
	LDR R7, =value2-8 ; store address of 8 bytes before 0x12 in R7 note can use negative numbers
	LDR R8, [R7,#8] ; R8=whats at R7+8 address
	; Move 2s complement of R4 into R3 with two instructions
	MOV R4, #0x0101
	MVN R4, R4 ;invert and add 1
	ADD R3, R4, #1 ; note you need the carry add or it wont work
	; Write one line of assembly to do the same using the reverse subtraction
	MOV R4, #0x12
	MOV R3, #0
	RSB R4, R4, #0
	MOV R5, #0
	
	END ; must be indented