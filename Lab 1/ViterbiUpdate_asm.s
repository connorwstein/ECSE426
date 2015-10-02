	AREA myCode, CODE, READONLY
	EXPORT ViterbiUpdate_asm

;This function takes the following arguments
;R0 = A pointer to vitpsi[:,t-1]
;R1 = A pointer to vitpsi[:,t]
;R2 = The observation
;R3 = A pointer to the hmm variables
;The output is stored in vitpsi[:,t], the memory ADDSress of which is located at R1.

ViterbiUpdate_asm FUNCTION
	;Initial steps
	PUSH{R4-R12} ;push registers that the calling function is using
	LDR R4, [R3] ;load number of states
	MOV R7, #4 ;4 bytes
	
	;Calculate offset required to get to the transition and emission matrix address
	MUL R9, R4, R4 ;transition matrix is S by S
	MUL R9, R7 ;this is S by S times 4 bytes
	ADDS R6, R3, #8 ;get address of transition
	ADDS R5, R6, R9 ;get address of emission

	;These numbers will be used throughout the code
	MUL R8, R4, R7 ;S times 4 bytes
	MUL R12, R2, R7 ;observation number times 4 bytes
	MOV R10, #-1 ;this is the state number in the outer most loop
	VLDR.F32 S3, =0.0 ;this stores the cumulative sum of vit[t]. It will be used to scale the final result.
	
	;This loop corresponds to: "for s in range (0,nStates)" 	
bigOuterLoop
	ADDS R10, #1 ;increment the loop counter
	CMP R10, R4 ;check if it is equal to S
	BEQ doneBigOuterLoop ;if it is, then exit the loop
	
	ADDS R0,R8; ;reset the address of vit[t-1] (this goes to the very end of the array, because the inner loops work backwards)
	MUL R11, R10, R7 ;amount needed to shift to correct column
	PUSH{R10} ;push the loop counter, so that R10 can be used for another purpose
	MOV R10, R4
	;This loop corresponds to: "trans_p = vit[:,t-1] * self.transition[:,s]"
getTransP 
	ADDS R10, #-1 ;decrement the number of states
	CMP R10, #0 
	BLT doneTransP ;if we've reached 0, then the calculation is complete
	
	VLDR.32 S0,[R0,#-4]! ;load vit[t-1]
	MUL R9, R10, R8 ; jump amount for transition
	ADDS R9, R6 ;get address for new position in transition
	ADDS R9, R11 ;this shifts the address to the correct column of transition
	VLDR.32 S1, [R9] ;get the next value in transition
	VMUL.F32 S0, S1 ;multiply the values
	VPUSH{S0};store this value on the stack
	B getTransP ;go to beginning of loop
	
	;This part gets ready for finding the maximum value in trans_p
doneTransP 
	VPOP{S0} ;get the most recently stored value in trans_p
	MOV R9, #0 ;this register will increment at every loop
	MOV R10, #0 ;this will store the index of the maximum value 
	
	;This part corresponds to: "max(enumerate(trans_p), key=operator.itemgetter(1))"
getMaxValue 
	ADDS R9,#1 ;increment loop counter
	CMP R9, R4 
	BEQ doneMaxValue ;if we've reached S, then the calculation is complete
	
	VPOP{S1} ;get trans_p elements
	VCMP.F32 S1,S0 ;compare this value with current max value
	VMRS APSR_nzcv, FPSCR ;make flag accessible
	BLT getMaxValue ;if the new number is less than the max, go to beginning of loop
	VMOV.F32 S0,S1 ;if the new number is greater than or equal, set that as the max.
	MOV R10, R9 ;also set the index to the current loop counter
	B getMaxValue ;go to beginning of loop
	
	;This part corresponds to: "psi[s,t], vit[s,t] = max(enumerate(trans_p), key=operator.itemgetter(1))"
	;and this: "vit[s,t] = vit[s,t]*self.emission[s,observations(t)]"
doneMaxValue
	VMOV S2, R10 ;move index to floating point register
	VCVT.F32.S32 S2, S2 ;convert to "integer" in floating point

	LDR R10, [R3,#4] ;load number of observation types
	MUL R10, R11 ;number of observation types times state number times number of bytes
	ADDS R9, R10, R5 ;get the address of the correct row of emission
	ADDS R9, R12 ;shift to the correct column. This requires the observation input.
	VLDR.32 S1, [R9] ;load the emission entry
	VMUL.F32 S0,S1 ;multiply the max value by the emission entry
	VADD.F32 S3,S0 ;add the value to the cumulative sum
	ADDS R9, R1, R11 ;get ADDSress for new position in vit
	VSTR S0, [R9] ;store the max value in vit[s,t]
	
	ADDS R9, R8 ;address for the correct position in psi
	VSTR S2, [R9] ;store index in ps
	POP{R10}
	B bigOuterLoop ;go back to beginning of big loop
	
	;After the big loop, check if the cumulative sum is 0. If it is, exit the function and return -1.
doneBigOuterLoop
	VCMP.F32 S3, #0.0
	VMRS APSR_nzcv, FPSCR
	BEQ failure	
	
	;Scale the elements in the output vit
divideBySum
	ADDS R4, #-1
	CMP R4, #0
	BLT doneDivision
	VLDR.32 S0, [R1] ;get the value in vit[:,t]
	VDIV.F32 S0,S3 ;divide the value by the scale factor
	VSTR S0, [R1], #4 ;store the value back in vit[:,t]
	B divideBySum
	
	;exit function
doneDivision
	SUB R1,R8
	POP{R4-R12}
	BX LR
	
failure
	MOV R0, #-1
	POP{R4-R12}
	BX LR
	ENDFUNC
	end
		