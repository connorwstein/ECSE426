	AREA myCode, CODE, READONLY
	EXPORT ViterbiUpdate_asm

;R0 = A pointer to vitpsi[:,t-1]
;R1 = A pointer to vitpsi[:,t]
;R2 = The observation
;R3 = A pointer to the hmm variables

ViterbiUpdate_asm FUNCTION
	LDR R5, [R3] ;load number of states
	LDR R6, [R3,#4] ;load number of observation types
	LDR R7, [R3,#8] ;load address of transition
	
	MOV R8, #4 ;4 bytes
	MUL R9, R6, R8 ;this is number of bytes times number of observation types
	MUL R10, R5, R9 ;get to offset to emission address
	SUB R6, R7, R10 ;get address of emission

	MUL R9, R5, R8 ;this is number of bytes times number of states
	MOV R11, #-1 ;this is the number of states in the outer loop.
bigOuterLoop
	ADDS R11, #1
	CMP R11, R5
	BEQ doneBigOuterLoop
getTransP 
	ADDS R5, #-1
	CMP R5, #0
	BLT doneTransP
	MUL R10, R5, R8 ;jump amount for vit[:,t-1]
	ADDS R10, R0, R10 ;get address for new position in vit
	VLDR.32 S0, [R10] ;get the value in vit[:,t-1]
	MUL R10, R5, R9 ;jump amount for transition
	ADDS R10, R7, R10 ;get address for new position in transition
	MUL R12, R11, R8 ;amount needed to shift to correct column
	ADDS R10, R12, R10 ;this shifts the address to the correct column of transition
	VLDR.32 S1, [R10] ;get the next value in transition
	VMUL.F32 S2, S0, S1 ;multiply the values
	VPUSH{S2};store this value on the stack
	B getTransP
doneTransP 
	LDR R5, [R3] ;reload number of states
	VPOP{S3} ;get trans_p[2]
	VLDR.F32 S10, = 0.0 ;this will have the index of the max value.
	VLDR.F32 S9, = 0.0 ;this will increment by 1 at every loop iteration
	VLDR.F32 S8, = 1.0 ;the amount to increment by
getMaxValue 
	ADDS R5,#-1
	CMP R5, #0
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
	LDR R5, [R3] ;reload number of states
	ADDS R10, R1, R9 ;address for the beginning of psi
	MUL R12, R11, R8 ;amount needed to get to correct position within psi (current state number times number of bytes)
	ADDS R10, R12, R10 ;shift to the correct position within psi
	VSTR S10, [R10] ;store index in psi
	MUL R12, R11, R8 ;current state number times number of bytes
	LDR R5, [R3,#4] ;load number of observation types
	MUL R5, R12, R5 ;number of observation types times state number times number of bytes
	MOV R10, R6 ;get address of emission
	ADDS R10, R5, R10 ;get the address of the correct row
	ADDS R10, R2, R10 ;shift to the correct column. This requires the observation input.
	VLDR.32 S4, [R10] ;load the emission entry
	VMUL.F32 S3, S3, S4 ;multiply the max value by the emission entry
	MUL R10, R11, R8 ;jump amount for vit[:,t]
	ADDS R10, R1, R10 ;get address for new position in vit
	VSTR S3, [R10] ;store the max value in vit[s,t]
	LDR R5, [R3] ;reload the number of states
	B bigOuterLoop ;go back to beginning of big loop	
doneBigOuterLoop
	VLDR.F32 S0, =0.0
getSumOfVit
	ADDS R5, #-1
	CMP R5, #0
	BLT doneSumOfVit
	MUL R10, R5, R8 ;jump amount for vit[:,t]
	ADDS R10, R1, R10 ;get address for new position in vit
	VLDR.32 S1, [R10] ;get the value in vit[:,t]
	VADD.F32 S0, S1, S0 ;add the value to the cumulative sum
	B getSumOfVit
doneSumOfVit
	LDR R5, [R3] ;reload the number of states
divideByScale
	ADDS R5, #-1
	CMP R5, #0
	BLT doneDivision
	MUL R10, R5, R8 ;jump amount for vit[:,t]
	ADDS R10, R1, R10 ;get address for new position in vit
	VLDR.32 S1, [R10] ;get the value in vit[:,t]
	VDIV.F32 S1, S1, S0 ;divide the value by the scale factor
	VSTR S1, [R10] ;store the value back in vit[:,t]
	B divideByScale
doneDivision
	BX LR
	ENDFUNC
	end
		