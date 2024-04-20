#include "../src/stuc/stuc.h"
#include <stdio.h>
#include <time.h>

float_t tData[] = {
	// 0, 0, 0,
	// 0, 1, 1,
	// 1, 0, 1,
	// 1, 1, 0
	7, 13,
	5, 3,
	8, 1,
	1, -1,
};

int main(void) {

	srand(time(0));
	
	size_t numberOfInputs  = 1;
	size_t numberOfOutputs = 1;
	size_t numberOfSamples = 4;

	Stuc_mat tInput  = {numberOfSamples, numberOfInputs,  numberOfInputs + numberOfOutputs, tData};
	Stuc_mat tOutput = {numberOfSamples, numberOfOutputs, numberOfInputs + numberOfOutputs, tData + tInput.cols};

	size_t arch[] = {1, 25, 25, 1};
	Stuc_activationFunction funk[STUC_LENP(arch)-1];
	stuc_setActivation(funk, STUC_LENP(funk), STUC_ACTIVATE_RELU);
	Stuc_nn nn = stuc_nnAlloc(funk, arch, STUC_LENP(arch));
	Stuc_nn gdMap = stuc_nnAlloc(funk, arch, STUC_LENP(arch));

	stuc_nnRand(nn, -1, 1);

	const size_t gen_count = 5*100*1000;
	float_t learningRate = 1e-4;
	float_t boostMultiplier = 1;

	for (size_t i = 0; i < gen_count + 1; i++) {
		stuc_nnBackpropNoAlloc(nn, gdMap, tInput, tOutput, boostMultiplier);
		stuc_nnApplyDiff(nn, gdMap, learningRate);

		if (i % (gen_count/10) == 0) {
			printf("\rcost = %.32f                          \n", 
				stuc_nnCost(nn, tInput, tOutput));
		} else if (i % (gen_count / 10000) == 1) {
			printf("\rTraning Neural Network Model: %.2f%%\r",
				(i / (float_t)gen_count) * 100);
		}
	}

	int shouldExit = 0;
	do {
		float num;
		printf("Unesi broj: ");
		(void)scanf("%f", &num);

		if (num == 0) shouldExit = 1;
		STUC_AT_INPUT(nn, 0) = num;
		stuc_nnForward(nn);
		printf("= %.2f\n", STUC_AT_OUTPUT(nn, 0));
	} while(!shouldExit);


	NN_PRINT(nn); 
	printf("cost = %f\n", stuc_nnCost(nn, tInput, tOutput));
	stuc_nnFree(nn);
	stuc_nnFree(gdMap);
	return 0;
}

