#define STUC_LRELU_FACT 0.1
#include "../src/stuc/stuc.h"
#include <stdio.h>
#include <time.h>

float_t tData[] = {
	0, 0, 0,
	0, 1, 1,
	1, 0, 1,
	1, 1, 0
};

int main(void) {

	time_t tim = time(0);
	srand(tim);
	printf("perfect: %zu\n", tim);

#ifdef PERFECT
	srand(1713945632);	//GNU_LINUX_PERFECT_SEED
#ifdef WIN32
	srand(1713947929);	//MS_WINDOWS_PERFECT_SEED
#endif
#endif

	size_t numberOfInputs  = 2;
	size_t numberOfOutputs = 1;
	size_t numberOfSamples = 4;

	Stuc_mat tInput  = {numberOfSamples, numberOfInputs,  numberOfInputs + numberOfOutputs, tData};
	Stuc_mat tOutput = {numberOfSamples, numberOfOutputs, numberOfInputs + numberOfOutputs, tData + tInput.cols};

	size_t arch[] = {2, 2, 1};
	Stuc_activationFunction funk[STUC_LENP(arch)-1] = {0};
	stuc_setActivation(funk, STUC_LENP(funk), STUC_ACTIVATE_RELU);

	Stuc_nn nn = stuc_nnAlloc(funk, arch, STUC_LENP(arch));
	Stuc_nn gdMap = stuc_nnAlloc(funk, arch, STUC_LENP(arch));

	stuc_nnRand(nn, -1, 1);

	const size_t gen_count = 2*100*1000;
	float_t learningRate = 1;
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

	printf("\nVerification:\n");
	size_t inputs = STUC_NN_INPUT(nn).cols;
	for (size_t i = 0; i < inputs; i++) {
		for (size_t j = 0; j < inputs; j++) {
			STUC_AT_INPUT(nn, 0) = i;
			STUC_AT_INPUT(nn, 1) = j;

			stuc_nnForward(nn);

			printf("%zu ^ %zu = %f\n", i, j, STUC_AT_OUTPUT(nn, 0));
		}
	}


	NN_PRINT(nn);
	printf("cost = %f\n", stuc_nnCost(nn, tInput, tOutput));
	stuc_nnFree(nn);
	stuc_nnFree(gdMap);
	return 0;
}

