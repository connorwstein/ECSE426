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

	int numStates=hmm->S;
	float sumOfVitOutput = 0;
	
	for(int s=0; s<numStates; s++){
		float trans_p[numStates];
		
		for(int i = 0; i<numStates; i++){
			trans_p[i] = InputArray[i]*(hmm->transition[i][s]);
		}
		
		float maximumValue = 0;
		uint32_t maximumValueIndex = 0;
		arm_max_f32(trans_p, numStates,&maximumValue, &maximumValueIndex);

		//Update vitpsi 
		OutputArray[numStates + s] = maximumValueIndex; //update psi
		OutputArray[s] = maximumValue * hmm->emission[s][Observation]; // update vit
		
		//for scaling at the end
		sumOfVitOutput += OutputArray[s];
	} 
	
	for(int i = 0; i<numStates; i++)
	{
		if(sumOfVitOutput==0)
			return -1;
		else
			OutputArray[i] /= sumOfVitOutput;
	}	
	return 0;
}


//Return -1 if an individual update returns -1 (division by zero error)
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
		if(ViterbiUpdate_C(vitpsi[t-1], vitpsi[t], hmm, Observations[t])==-1){
			printf("Error at time %d\n", t);
			return -1;
		};
		//ViterbiUpdate_asm(vitpsi[t-1],vitpsi[t],Observations[t],hmm);
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
	return 0;
}
int getRegion(float reading){
		int alpha1=10, alpha2=20;
		
		if(reading>alpha2){
			return 2;
		}
		else if(reading<-alpha2){
			return 3;
		}
		else if(reading<=abs(alpha1)){
			return 1;
		}
		else{
			return 0; //not a valid region
		}
		return 0;
}

int generateObservations(float *accObs, int accT, int *observations, int maxObservations){
	//Thresholds are alpha1, alpha2, N1, N2, N3
	//Ni's are the number of consecutive samples required for an observation to occur in region i
	// if accObs[i] > alpha2 in region 2
	// < - alpha2 in region 3
	// <=  |alpha1| in region 1
	int N1=2, N2=2, N3=2;
	int observationsIndex=0;
	int r1Count=0, r2Count=0, r3Count=0;
	int i=0;
	while(i<accT && observationsIndex < maxObservations){
			switch(getRegion(accObs[i])){
				case 1:
					r1Count++; //Already detected a region 1 reading
					while(getRegion(accObs[++i])==1){ //get the next reading and check if it is already region 1
						//While the region is still 1 check if it is greater than the required number of samples 
						if(++r1Count>=N1){ //the next reading is region 1 (hence the prefix increment)
							observations[observationsIndex++]=1; //write the observation down
							r1Count=0; //reset the counter
						}
					}
					r1Count=0; //If the next region is not region 1, no longer consecutive and must restart (note i has already been incremented though)
					break;
				case 2:
					r2Count++;
					while(getRegion(accObs[++i])==2){
						//While the region is still 1 check if it is greater than the required number of samples 
						if(++r2Count>=N2){
							observations[observationsIndex++]=2;
							r2Count=0;
						}
					}
					r2Count=0;
					break;
				case 3:
					r3Count++;
					while(getRegion(accObs[++i])==3){
						//While the region is still 1 check if it is greater than the required number of samples 
						if(++r3Count>=N3){
							observations[observationsIndex++]=3;
							r3Count=0;
						}
					}
					r3Count=0;
					break;
				default:
					//ignore the value
					i++;
					break;
			}
	}
	return 0;
}	

int main()
{
	
	hmm_desc hmm = {S_DEF,V_DEF,{{0.75,0.25},{0.32,0.68}},{{0.8,0.1,0.1},{0.1,0.2,0.7}},{0.5,0.5}};
	
	int EstimatedStates[S_DEF];
	memset(EstimatedStates, 0, sizeof(EstimatedStates));
	//Call generate observations to create the array below from a bunch of accelerometer data
	float accel[]={0.5, 0.8, 5, 5, 6,  10.5, 20.2, 20.2, -20.5, -40, -50, -50};
	int ob[]={0,0,0,0};
	generateObservations(accel, 12, ob, 4);
	printf("%d %d %d %d\n", ob[0], ob[1], ob[2],ob[3]); 
	int Observations[] = {0,1,2,0,1,2}; //6 observations
	Viterbi_C(Observations, 6, EstimatedStates, &hmm); 
	return 0;
}
