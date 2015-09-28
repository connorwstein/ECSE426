	AREA myCode, CODE, READONLY
	EXPORT ViterbiUpdate_asm

;R0 = A pointer to vitpsi[:,t-1]
;R1 = A pointer to vitpsi[:,t]
;R2 = The observation
;R3 = A pointer to the hmm variables

ViterbiUpdate_asm FUNCTION
	PUSH{R4-R12}
	;SUB R0, #36
	;SUB R1, #36
	;SUB R3, #36
	LDR R4, [R3] ;load number of states
	LDR R5, [R3,#4] ;load number of observation types
	ADD R6, R3, #8 ;load address of transition
	
	MOV R7, #4 ;4 bytes
	MUL R8, R5, R7 ;this is number of bytes times number of observation types
	MUL R9, R4, R4 ;get to offset to emission address
	MUL R9, R9, R7 ;get to offset to emission address
	ADD R5, R6, R9 ;get address of emission

	MUL R8, R4, R7 ;this is number of bytes times number of states
	MOV R10, #-1 ;this is the number of states in the outer loop.
bigOuterLoop
	ADDS R10, #1
	CMP R10, R4
	BEQ doneBigOuterLoop
getTransP 
	ADDS R4, #-1
	CMP R4, #0
	BLT doneTransP
	MUL R9, R4, R7 ;jump amount for vit[:,t-1]
	ADDS R9, R0, R9 ;get address for new position in vit
	VLDR.32 S0, [R9] ;get the value in vit[:,t-1]
	MUL R9, R4, R8 ; jump amount for transition
	ADDS R9, R6, R9 ;get address for new position in transition
	MUL R11, R10, R7 ;amount needed to shift to correct column
	ADDS R9, R11, R9 ;this shifts the address to the correct column of transition
	VLDR.32 S1, [R9] ;get the next value in transition
	VMUL.F32 S2, S0, S1 ;multiply the values
	VPUSH{S2};store this value on the stack
	B getTransP
doneTransP 
	LDR R4, [R3] ;reload number of states
	VPOP{S3} ;get trans_p[2]
	VLDR.F32 S10, = 0.0 ;this will have the index of the max value.
	VLDR.F32 S9, = 0.0 ;this will increment by 1 at every loop iteration
	VLDR.F32 S8, = 1.0 ;the amount to increment by
getMaxValue 
	ADDS R4,#-1
	CMP R4, #0
	BEQ doneMaxValue
	VPOP{S4} ;get trans_p[3,4...n, and then lastly 1]
	VADD.F32 S9,S9,S8 ;increment the counter
	VCMP.F32 S4,S3 ;compare this value with current max value
	VMRS APSR_nzcv, FPSCR
	BLT getMaxValue ;if the new number is less than the max, go to beginning of loop
	VMOV.F32 S3,S4 ;if the new number is greater than or equal, set that as the max.
	VMOV.F32 S10, S9 ;also set the index to the current loop counter
	B getMaxValue ;go to beginning of loop
doneMaxValue
	LDR R4, [R3] ;reload number of states
	MUL R9, R4, R7; number of states times number of bytes
	ADDS R9, R1, R9 ;address for the beginning of psi
	MUL R11, R10, R7 ;amount needed to get to correct position within psi (current state number times number of bytes)
	ADDS R9, R11, R9 ;shift to the correct position within psi
	VSTR S10, [R9] ;store index in psi
	MUL R11, R10, R7 ;current state number times number of bytes
	LDR R4, [R3,#4] ;load number of observation types
	MUL R4, R11, R4 ;number of observation types times state number times number of bytes
	MOV R9, R5 ;get address of emission
	ADDS R9, R4, R9 ;get the address of the correct row
	MUL R12, R2, R7 ;observation number times number of bytes
	ADDS R9, R12, R9 ;shift to the correct column. This requires the observation input.
	VLDR.32 S4, [R9] ;load the emission entry
	VMUL.F32 S3, S3, S4 ;multiply the max value by the emission entry
	MUL R9, R10, R7 ;jump amount for vit[:,t]
	ADDS R9, R1, R9 ;get address for new position in vit
	VSTR S3, [R9] ;store the max value in vit[s,t]
	LDR R4, [R3] ;reload the number of states
	B bigOuterLoop ;go back to beginning of big loop	
doneBigOuterLoop
	VLDR.F32 S0, =0.0
getSumOfVit
	ADDS R4, #-1
	CMP R4, #0
	BLT doneSumOfVit
	MUL R9, R4, R7 ;jump amount for vit[:,t]
	ADDS R9, R1, R9 ;get address for new position in vit
	VLDR.32 S1, [R9] ;get the value in vit[:,t]
	VADD.F32 S0, S1, S0 ;add the value to the cumulative sum
	B getSumOfVit
doneSumOfVit
	LDR R4, [R3] ;reload the number of states
divideByScale
	ADDS R4, #-1
	CMP R4, #0
	BLT doneDivision
	MUL R9, R4, R7 ;jump amount for vit[:,t]
	ADDS R9, R1, R9 ;get address for new position in vit
	VLDR.32 S1, [R9] ;get the value in vit[:,t]
	VDIV.F32 S1, S1, S0 ;divide the value by the scale factor
	VSTR S1, [R9] ;store the value back in vit[:,t]
	B divideByScale
doneDivision
	POP{R4-R12}
	BX LR
	ENDFUNC
	end
		