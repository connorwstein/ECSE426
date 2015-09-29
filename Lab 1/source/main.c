#include <stdio.h>
#include "arm_math.h"

#define S_DEF 2
#define V_DEF 3

typedef struct {
	int S;
	int V;
	float transition[S_DEF][S_DEF];
	float emission[S_DEF][V_DEF];
	float prior[S_DEF];
}hmm_desc;

extern void ViterbiUpdate_asm(float* InputArray, float* OutputArray, int Observation, hmm_desc* hmm);

int ViterbiUpdate_C(float* InputArray, float* OutputArray, hmm_desc* hmm, int Observation){

	for(int s = 0; s<hmm->S; s++)
	{
		float trans_p[hmm->S];
		for(int i = 0; i<hmm->S; i++)
		{
			trans_p[i] = InputArray[i]*(hmm->transition[i][s]);
		}
		
		float maximumValue = -1;
		float maximumValueIndex = -1;
		
		for(int i = 0; i<hmm->S; i++)
		{
			if(trans_p[i] > maximumValue)
			{
				maximumValue = trans_p[i];
				maximumValueIndex = i;
			}
		}
		
		//vit at time t
		OutputArray[s] = maximumValue;
		
		//psi at time t
		OutputArray[hmm->S + s] = maximumValueIndex;
		
		OutputArray[s] *= hmm->emission[s][Observation];
		
	} 
	
	float sumOfVitOutput = 0;
	
	for(int i = 0; i<hmm->S; i++)
	{
		sumOfVitOutput += OutputArray[i];
	}
	
	float scalingFactor = 1.0f/sumOfVitOutput;
	
	for(int i = 0; i<hmm->S; i++)
	{
		OutputArray[i] *= scalingFactor;
	}	
	
	return 0;
}

int Viterbi_C(int *Observations, int Nobs, int *EstimatedStates, hmm_desc *hmm){

	float vitpsi[Nobs][2*S_DEF];
	float sumOfVitInput = 0;
	int numStates=hmm->S;
	
	//Initializing t=0, fill in first row of vitspi using priors
	for(int i=0; i<numStates;i++)
	{
		vitpsi[0][i] = hmm->prior[i] * hmm->emission[i][Observations[0]];
		sumOfVitInput+=vitpsi[0][i];
	}	
	
	for(int i = 0; i<numStates; i++)
	{
		vitpsi[0][i] /= sumOfVitInput;
		printf("vitInput[%d] = %f ",i,vitpsi[0][i]);
	}
	printf("\n");
	//Done Initializing
	
	//Loop through observations, writing them to the Estimated states
	for(int t=1;t<Nobs;t++)
	{
		//ViterbiUpdate_C(vitInput, vitpsiOutput, &hmm, observation[t]);
		ViterbiUpdate_asm(vitpsi[t-1],vitpsi[t],Observations[t],hmm);
		printf("vitpsi in row %d\n",t);
		for(int i=0;i<numStates*2;i++){
			printf("%f, ",vitpsi[t][i]);
		}
		printf("\n");
	}
	
	// Back tracking 
	uint32_t index_best_last_state;
	float output;
	arm_max_f32(vitpsi[Nobs-1], numStates, &output, &index_best_last_state);
	printf("Best estimates: %d, ", index_best_last_state); 
	EstimatedStates[Nobs-1]=index_best_last_state;
	for(int t=(Nobs-1);t>0;t--){
			EstimatedStates[t-1]=vitpsi[t][EstimatedStates[t]+numStates]; //+numStates to get to the psi values
			printf("%d, ", EstimatedStates[t-1]);
	}
	
}

int main()
{
	
	hmm_desc hmm = {S_DEF,V_DEF,{{0.75,0.25},{0.32,0.68}},{{0.8,0.1,0.1},{0.1,0.2,0.7}},{0.5,0.5}};
	int Observations[] = {0,1,2,0,1,2};
	int EstimatedStates[]={0,0,0,0,0,0};
	
	Viterbi_C(Observations, 6,EstimatedStates, &hmm); 
//	float test[]={1.2, 3.4, 5.6};
//	float output=0;
//	uint32_t index=0;
//	arm_max_f32(test, 3, &output,&index);
//	printf("output %f\n",output);
	return 0;
}
