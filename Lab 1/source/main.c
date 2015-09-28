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


extern void ViterbiUpdate_asm(float* InputArray, float* OutputArray, int Observation, hmm_desc* hmm);






int main()
{
	
	hmm_desc hmm = {S_DEF,V_DEF,{{0.75,0.25},{0.32,0.68}},{{0.8,0.1,0.1},{0.1,0.2,0.7}},{0.5,0.5}};

	int observation[] = {0,1,2,0,1,2};
	
	float vitInput[S_DEF];
	float vitpsiOutput[2*S_DEF];
		
	memset(vitpsiOutput, 0, sizeof(vitpsiOutput));
	
	int numberOfTimeSteps = 6;
						
	float sumOfVitInput = 0;
	
	for(int i=0; i<hmm.S;i++)
	{
		vitInput[i] = hmm.prior[i] * hmm.emission[i][observation[0]];
		
		sumOfVitInput+=vitInput[i];
	}	
	
	for(int i = 0; i<hmm.S; i++)
	{
		vitInput[i] /= sumOfVitInput;
		printf("vitInput[%d] = %f",i,vitInput[i]);
	}
	printf("\n");

	
	for(int t=1;t<numberOfTimeSteps;t++)
	{
		
		//ViterbiUpdate_C(vitInput, vitpsiOutput, &hmm, observation[t]);
		ViterbiUpdate_asm(vitInput,vitpsiOutput,observation[t],&hmm);
		printf("timestep %d: vitInput is\n",t);
		
		for(int i=0;i<hmm.S;i++)
		{
			vitInput[i] = vitpsiOutput[i];
			
			printf("%f ",vitInput[i]);
		}
		printf("\n");
	}
	

	return 0;
}
