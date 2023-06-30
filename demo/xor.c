#define TESTOVI_IMPLEMENTATION
#include "testovi.h"
#define STUC_IMPLEMENTATION
#include "../src/stuc.h"
#include <stdio.h>
#include <time.h>

float_t tData[] = {
	0, 0, 0,
	0, 1, 1,
	1, 0, 1,
	1, 1, 0
};

int main() {
	test_runAll(); 

	srand(time(0));
	// srand(14);
	Stuc_mat tInput = {4, 2, 3, tData};
	Stuc_mat tOutput = {4, 1, 3, tData+2};

	size_t arch[] = {2, 2, 1};
	Stuc_activationFunction funk[] = {STUC_ACTIVATE_SIGMOID, STUC_ACTIVATE_SIGMOID};
	Stuc_nn nn = stuc_nnAlloc(funk, arch, STUC_LENP(arch));
	Stuc_nn gdMap = stuc_nnAlloc(funk, arch, STUC_LENP(arch));

	stuc_nnRand(nn, -1, 1);

	size_t gen_count = 2*100*1000;
	float_t learningRate = 1;
	float_t boostMultiplier = 1;

	for (size_t i = 0; i < gen_count + 1; i++) {
		stuc_nnBackpropNoAlloc(nn, gdMap, tInput, tOutput, boostMultiplier);
		stuc_nnApplyDiff(nn, gdMap, learningRate);

		if (i % (gen_count/10) == 0) {
			printf("\rcost = %f                          \n", 
				stuc_nnCost(nn, tInput, tOutput));
		} else if (i % (gen_count / 10000) == 1) {
			printf("\rTraning Neural Network Model: %.2f%s\r",
				(i / (float_t)gen_count) * 100, "%"); 
		}
	}

	printf("\nVerification:\n");
	size_t inputs = STUC_NN_INPUT(nn).cols;
	for (size_t i = 0; i < inputs; i++) {
		for (size_t j = 0; j < inputs; j++) {
			STUC_MAT_AT(STUC_NN_INPUT(nn), 0, 0) = i;
			STUC_MAT_AT(STUC_NN_INPUT(nn), 0, 1) = j;

			stuc_nnForward(nn);

			printf("%zu ^ %zu = %f\n", i, j, STUC_MAT_AT(STUC_NN_OUTPUT(nn), 0, 0));
		}
	}


	NN_PRINT(nn); 
	printf("cost = %f\n", stuc_nnCost(nn, tInput, tOutput));
	stuc_nnFree(nn);
	stuc_nnFree(gdMap);
	return 0;
}

