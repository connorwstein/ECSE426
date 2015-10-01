#include <stdio.h>
#include "arm_math.h"

//#define S_DEF 2
//#define V_DEF 3
#define S_DEF 4
#define V_DEF 3
typedef struct {
	int S;
	int V;
	arm_matrix_instance_f32 transition;
	arm_matrix_instance_f32 emission;
	float32_t* prior;
}hmm_desc;
float32_t vitpsi_CMSIS[20][2*S_DEF];

extern int ViterbiUpdate_asm(float32_t* InputArray, float32_t* OutputArray, int Observation, hmm_desc* hmm);

int ViterbiUpdate_C_CMSIS(float32_t* InputArray, float32_t* OutputArray, hmm_desc* hmm, int Observation){

	uint32_t numStates=hmm->S;
	float32_t sumOfVitOutput = 0;
	
	for(int s=0; s<numStates; s++){
		float32_t trans_p[numStates];
		
		arm_mult_f32(InputArray,&(hmm->transition.pData[s*numStates]),trans_p,numStates);
		
		float32_t maximumValue = 0;
		uint32_t maximumValueIndex = 0;
		arm_max_f32(trans_p, numStates,&maximumValue, &maximumValueIndex);

		//Update vitpsi_CMSIS 
		OutputArray[numStates + s] = maximumValueIndex; //update psi
		
		OutputArray[s] = maximumValue * hmm->emission.pData[Observation*numStates+s]; // update vit
		
		//for scaling at the end
		sumOfVitOutput += OutputArray[s];
	} 
	
	if(sumOfVitOutput==0)
		return -1;
	else
	{
		arm_scale_f32(OutputArray, 1/sumOfVitOutput,OutputArray,numStates);
	}
	
	
	return 0;
}


//Return -1 if an individual update returns -1 (division by zero error)
int Viterbi_C_CMSIS(int *Observations, int Nobs, int *EstimatedStates, hmm_desc *hmm){

	//float32_t vitpsi_CMSIS[Nobs][2*S_DEF]; //20 rows 8 columns 
	float32_t sumOfVitInput = 0;
	int numStates=hmm->S;
	
	//Initializing t=0, fill in first row of vitspi using priors

	arm_mult_f32(hmm->prior,&(hmm->emission.pData[Observations[0]*numStates]),vitpsi_CMSIS[0],numStates);
	for(int i=0; i<numStates;i++)
	{	
		sumOfVitInput+=vitpsi_CMSIS[0][i];
	}	
	for(int i = 0; i<numStates; i++)
	{
		if(sumOfVitInput==0){
				return -1;
		}
		vitpsi_CMSIS[0][i] /= sumOfVitInput;
		//printf("vitInput[%d] = %f ",i,vitpsi_CMSIS[0][i]);
	}
	//printf("\n");
	//Done Initializing
	
	//Loop through observations, writing them to the Estimated states
	for(int t=1;t<Nobs;t++)
	{
		if(ViterbiUpdate_C_CMSIS(vitpsi_CMSIS[t-1], vitpsi_CMSIS[t], hmm, Observations[t])==-1){
			printf("Error at time %d\n", t);
			return -1;
		};
		//printf("vitpsi_CMSIS in row %d\n",t);
		for(int i=0;i<numStates*2;i++){
			//printf("%f, ",vitpsi_CMSIS[t][i]);
		}
		//printf("\n");
	}
	
	// Back tracking 
	uint32_t index_best_last_state;
	float32_t output;
	arm_max_f32(vitpsi_CMSIS[Nobs-1], numStates, &output, &index_best_last_state);
	EstimatedStates[Nobs-1]=index_best_last_state;
	for(int t=(Nobs-1);t>0;t--){
			EstimatedStates[t-1]=vitpsi_CMSIS[t][EstimatedStates[t]+numStates]; //+numStates to get to the psi values
	}
	return 0;
}
int getRegion_CMSIS(float32_t reading, float32_t alpha1, float32_t alpha2){
		
		if(reading>alpha2){
			return 1;
		}
		else if(reading<-alpha2){
			return 2;
		}
		else if(reading<=abs(alpha1)){
			return 0;
		}
		else{
			return 4; //not a valid region
		}
		return 0;
}

int generateObservations_CMSIS(float32_t *accObs, int accT, int *observations, int maxObservations, float32_t alpha1, float32_t alpha2, int N1, int N2, int N3){
	//Thresholds are alpha1, alpha2, N1, N2, N3
	//Ni's are the number of consecutive samples required for an observation to occur in region i
	// if accObs[i] > alpha2 in region 2
	// < - alpha2 in region 3
	// <=  |alpha1| in region 1
	int observationsIndex=0;
	int r0Count=0, r1Count=0, r2Count=0;
	int i=0;
	while(i<accT && observationsIndex < maxObservations){
		//printf("Region: %d, iteration %d\n", getRegion_CMSIS(accObs[i],0.7,0.8),i);
		char recorded;
			switch(getRegion_CMSIS(accObs[i],alpha1, alpha2)){
				case 0:
					r0Count++; //Already detected a region 1 reading
					recorded=0;
					while(getRegion_CMSIS(accObs[++i],alpha1, alpha2)==0){ //get the next reading and check if it is already region 1
						//While the region is still 1 check if it is greater than the required number of samples 
						if(++r0Count>=N1 && !recorded){ //the next reading is region 1 (hence the prefix increment)
							observations[observationsIndex++]=0; //write the observation down
							recorded=1;
						}
					}
					r0Count=0; //If the next region is not region 1, no longer consecutive and must restart (note i has already been incremented though)
					break;
				case 1:
					r1Count++;
					recorded=0;
					while(getRegion_CMSIS(accObs[++i],alpha1, alpha2)==1){
						//While the region is still 1 check if it is greater than the required number of samples 
						if(++r1Count>=N2 && !recorded){
							observations[observationsIndex++]=1;
							recorded=1;
						}
					}
					r1Count=0;
					break;
				case 2:
					r2Count++;
					recorded =0;
					while(getRegion_CMSIS(accObs[++i],alpha1,alpha2)==2){
						//While the region is still 1 check if it is greater than the required number of samples 
						if(++r2Count>=N3 &&!recorded){
							observations[observationsIndex++]=2;
							recorded=1;
						}
					}
					r2Count=0;
					break;
				default:
					//ignore the value
					i++;
					break;
			}
	}
	return observationsIndex;
}	

int main()
{
	printf("Using CMSIS Library\n");
	//tranposed matrices so that the CMSIS library can be used
	float32_t transitionData[S_DEF*S_DEF] = {0.09,0.09,0.09,0.5,0.9,0.01,0.45,0.5,0.01,0.9,0.01,0,0,0,0.45,0};
	float32_t emissionData[S_DEF*V_DEF] = {1,0,0,0,0,1,0,1,0,0,1,0};
		
	arm_matrix_instance_f32 transitionMatrix;
	transitionMatrix.numRows = S_DEF;
	transitionMatrix.numCols = S_DEF;
	transitionMatrix.pData = transitionData;
	
	arm_matrix_instance_f32 emissionMatrix;
	emissionMatrix.numRows = S_DEF;
	emissionMatrix.numCols = S_DEF;
	emissionMatrix.pData = emissionData;
	
	float32_t priorData[S_DEF] = {0.25,0.25,0.25,0.25};
	
	hmm_desc hmm1;
	hmm1.S = S_DEF;
	hmm1.V = V_DEF;
	hmm1.transition = transitionMatrix;
	hmm1.emission = emissionMatrix;
	hmm1.prior = priorData;
	
	
	//Test 1
	
	printf("----------------TEST 1----------------\n");
	
	int EstimatedStates[20];
	memset(EstimatedStates, 0, sizeof(EstimatedStates));
	int vitTestArray[20] = {2,1,1,2,1,2,1,0,1,2,1,1,2,1,2,1,1,2,1,0};
	if(Viterbi_C_CMSIS(vitTestArray, 20, EstimatedStates, &hmm1)==-1){
		printf("Error in decoding\n");
	}
	printf("Vit path: ");
	for(int i=0;i<20;i++){
			if(i==19){
				printf("%d\n",EstimatedStates[i]);
				break;
			}
			printf("%d, ", EstimatedStates[i]);
	}
	printf("\n");
	//Test 2
	
	printf("----------------TEST 2----------------\n");
	
	
	int accT = 45;
	float32_t accObs[45] = 
	{-0.47, 0.51, 0.69, 0.02, 0.54, 0.12, -0.48, 0.42, 3.73, 
 6.19, 6.74, 6.49, -5.71, -4.15, -7.35, -7.20, -7.02, -3.11,
 4.37, 2.17, 4.36, 1.86, -0.62, 0.49, 0.08, 0.60, 0.27, 
 0.12, 6.67, 7.13, 7.92, 0.80, -1.77, -3.59, -0.87, 4.60, 
 4.25, 6.57, 2.44, 0.03, 0.56, 0.10, 0.48, 0.33, 0.12};
	int obs[45];
	int actualObservations=generateObservations_CMSIS(accObs, accT, obs, 45, 0.7, 0.8, 3, 3, 3);
	int finalObs[actualObservations];
  memcpy(finalObs, obs, sizeof(int)*actualObservations);
	printf("Observations: ");
	for(int i=0;i<actualObservations;i++){
		if(i==actualObservations-1){
			printf("%d\n", finalObs[i]);
			break;
		}
		printf("%d, ", finalObs[i]);
	}
	int EstimatedStates2[9];
	Viterbi_C_CMSIS(finalObs, 9, EstimatedStates2, &hmm1); 
	printf("Vit path: ");
	for(int i=0;i<9;i++){
			if(i==8){
				printf("%d\n",EstimatedStates2[i]);
				break;
			}
			printf("%d, ", EstimatedStates2[i]);
	}
	printf("\n");
	
	
	return 0;
}
