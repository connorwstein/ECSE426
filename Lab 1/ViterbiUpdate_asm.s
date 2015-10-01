	AREA myCode, CODE, READONLY
	EXPORT ViterbiUpdate_asm

;This function takes the following arguments
;R0 = A pointer to vitpsi[:,t-1]
;R1 = A pointer to vitpsi[:,t]
;R2 = The observation
;R3 = A pointer to the hmm variables
;The output is stored in vitpsi[:,t], the memory ADDSress of which is located at R1.

ViterbiUpdate_asm FUNCTION
	PUSH{R4-R12}
	LDR R4, [R3] ;load number of states
	MOV R7, #4 ;4 bytes
	
	MUL R9, R4, R4 ;get to offset to emission ADDSress
	MUL R9, R7 ;get to offset to emission ADDSress
	ADDS R6, R3, #8 ;load ADDSress of transition
	ADDS R5, R6, R9 ;get ADDSress of emission

	MUL R8, R4, R7 ;this is number of bytes times number of states
	MUL R12, R2, R7 ;observation number times number of bytes
	
	MOV R10, #-1 ;this is the number of states in the outer loop.
	VLDR.F32 S5, =0.0 ;this stores the sum of vit[t]
bigOuterLoop
	ADDS R10, #1
	CMP R10, R4
	BEQ divideBySum
	ADDS R0,R8; ;reset ADDSress of vit[t-1]
	MUL R11, R10, R7 ;amount needed to shift to correct column
getTransP 
	ADDS R4, #-1
	CMP R4, #0
	BLT doneTransP
	VLDR.32 S0,[R0,#-4]! ;load vit[t-1]
	MUL R9, R4, R8 ; jump amount for transition
	ADDS R9, R6 ;get ADDSress for new position in transition
	ADDS R9, R11 ;this shifts the ADDSress to the correct column of transition
	VLDR.32 S1, [R9] ;get the next value in transition
	VMUL.F32 S0, S1 ;multiply the values
	VPUSH{S0};store this value on the stack
	B getTransP
doneTransP 
	LDR R4, [R3] ;reload number of states
	VPOP{S3} ;get trans_p[2]
	VLDR.F32 S2, = 0.0 ;this will have the index of the max value.
	VLDR.F32 S1, = 0.0 ;this will increment by 1 at every loop iteration
	VLDR.F32 S0, = 1.0 ;the amount to increment by
getMaxValue 
	ADDS R4,#-1
	CMP R4, #0
	BEQ doneMaxValue
	VPOP{S4} ;get trans_p elements
	VADD.F32 S1,S0 ;increment the counter
	VCMP.F32 S4,S3 ;compare this value with current max value
	VMRS APSR_nzcv, FPSCR
	BLT getMaxValue ;if the new number is less than the max, go to beginning of loop
	VMOV.F32 S3,S4 ;if the new number is greater than or equal, set that as the max.
	VMOV.F32 S2, S1 ;also set the index to the current loop counter
	B getMaxValue ;go to beginning of loop
doneMaxValue
	LDR R4, [R3,#4] ;load number of observation types
	MUL R4, R11 ;number of observation types times state number times number of bytes
	ADDS R9, R4, R5 ;get the ADDSress of the correct row of emission
	ADDS R9, R12 ;shift to the correct column. This requires the observation input.
	VLDR.32 S4, [R9] ;load the emission entry
	VMUL.F32 S3,S4 ;multiply the max value by the emission entry
	VADD.F32 S5,S3 ;ADDS the value to the cumulative sum
	ADDS R9, R1, R11 ;get ADDSress for new position in vit
	VSTR S3, [R9] ;store the max value in vit[s,t]
	ADDS R9, R8 ;ADDSress for the correct position in psi
	VSTR S2, [R9] ;store index in psi
	LDR R4, [R3] ;reload the number of states
	B bigOuterLoop ;go back to beginning of big loop	
divideBySum
	VCMP.F32 S5, #0.0
	VMRS APSR_nzcv, FPSCR
	BEQ failure
	ADDS R4, #-1
	CMP R4, #0
	BLT doneDivision
	VLDR.32 S1, [R1] ;get the value in vit[:,t]
	VDIV.F32 S1,S5 ;divide the value by the scale factor
	VSTR S1, [R1], #4 ;store the value back in vit[:,t]
	B divideBySum
doneDivision
	SUB R1,R8
	POP{R4-R12}
	BX LR
failure
	MOV R2, #-1
	ENDFUNC
	end
		