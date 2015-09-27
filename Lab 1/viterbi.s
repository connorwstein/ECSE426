	AREA text, CODE, READONLY
	EXPORT viterbi_testbed
	IMPORT input
	IMPORT output
	IMPORT s
	IMPORT v
	IMPORT transition
	IMPORT emission
	IMPORT prior
	IMPORT observation
	IMPORT ViterbiUpdate_asm
	; INPUT parameters
	; R0: Pointer to the vitpsi[:,t-1] vector
	; R1: Pointer to the vitspi[:,t] vector for output
	; R2: The observation
	; R3: Pointer to the hmm variable (struct)
	; hmm
	;float a[] = {0.75,0.25,0.32,0.68};
	;hmm.transition = a;
	
	;float b[] = {0.8,0.1,0.1,0.1,0.2,0.7};
	;hmm.emission = b;
	
	;float c[] = {0.5,0.5};
	;hmm.prior = c;
	
	;int observation[] = {0,1,2,0,1,2};
	
	; int S, int V, float * transition, float * emission, float * prior
	; update(inputarray, outputarray, hmm* , observation)
	
viterbi 
	;When this is called, R0=*inputarray, R1=*outputarray, R2=hmm* and R3=observation[0]
	MOV R10, #0x0101
	BX LR
	
	
viterbi_testbed
	;Load s into memory
	MOV R0, #2
	LDR R1, =s
	STR R0, [R1]
	;Load v into memory
	MOV R0, #3
	LDR R1, =v
	STR R0, [R1]
	
	; Load constants into fp regs for transition matrix
	VLDR.F32 S0, =0.75 ;psuedo instruction for some reason can use VMOV.F32 with all immediate values, some dont work
	VLDR.F32 S1, =0.25
	VLDR.F32 S2, =0.32
	VLDR.F32 S3, =0.68
	LDR R0, =transition ;load address of transition matrix array
	VSTM R0, {S0-S3}
	
	; Load constants into fp regs for emission matrix, should be a way to do this in a single instruction?
	VLDR.F32 S0, =0.8
	VLDR.F32 S1, =0.1
	VLDR.F32 S2, =0.1
	VLDR.F32 S3, =0.1
	VLDR.F32 S4, =0.2
	VLDR.F32 S5, =0.7
	LDR R0, =emission ;load address of transition matrix array
	VSTM R0, {S0-S5}
	
	; Load prior into memory
	VLDR.F32 S0, =0.5
	VLDR.F32 S1, =0.5
	LDR R0,=prior
	VSTM R0, {S0-S1}
	
	;Load observation into memory
	MOV R0, #0
	MOV R1, #1
	MOV R2, #2
	LDR R3, =observation
	LDR R4, =observation+12
	STM R3, {R0-R2}
	STM R4, {R0-R2}
	
	;hmm struct is at =s
	;input array is at =input
	;ouput array is at =output
	;observations are at =observations 
	LDR R0, =input ; pointer to input array
	LDR R1, =output ; pointer to output array
	LDR R4, =observation
	LDR R2, [R4] ; observation value
	LDR R3, =s ; pointer to hmm struct
	BL ViterbiUpdate_asm
	

	END