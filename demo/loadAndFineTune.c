#include <stdint.h>
#include <stdio.h>

#include "../src/stuc.h"
#define STUC_IMPLEMENTATION
#include "../src/stuc.h"

float_t tData[] = {
	0, 0, 0,
	0, 1, 1,
	1, 0, 1,
	1, 1, 2
};

int main(void) {
	uint8_t ret;
	Stuc_nn nn = stuc_nnLoadFromFile("trainedNNs/Xor.snn", &ret);
	stuc_printIOFlags(ret);
	if (ret) return ret;

	printf("\n XOR Verification:\n");
	size_t inputs = STUC_NN_INPUT(nn).cols;
	for (size_t i = 0; i < inputs; i++) {
		for (size_t j = 0; j < inputs; j++) {
			STUC_MAT_AT(STUC_NN_INPUT(nn), 0, 0) = i;
			STUC_MAT_AT(STUC_NN_INPUT(nn), 0, 1) = j;

			stuc_nnForward(nn);

			printf("  %zu ^ %zu = %f\n", i, j, STUC_MAT_AT(STUC_NN_OUTPUT(nn), 0, 0));
		}
	}

	size_t numberOfInputs  = 2;
	size_t numberOfOutputs = 1;
	size_t numberOfSamples = 4;

	Stuc_mat tInput  = {numberOfSamples, numberOfInputs,  numberOfInputs + numberOfOutputs, tData};
	Stuc_mat tOutput = {numberOfSamples, numberOfOutputs, numberOfInputs + numberOfOutputs, tData + tInput.cols};

	size_t gen_count = 70*1000;
	float_t learningRate = 1;
	float_t boostMultiplier = 1;

	for (size_t i = 0; i < gen_count + 1; i++) {
		Stuc_nn gdMap = stuc_nnBackprop(nn, tInput, tOutput, boostMultiplier);
		stuc_nnApplyDiff(nn, gdMap, learningRate);
		stuc_nnFree(gdMap);

		if (i % (gen_count/10) == 0) {
			printf("\rcost = %.32f                          \n", 
				stuc_nnCost(nn, tInput, tOutput));
		} else if (gen_count >= 10000 && i % (gen_count / 10000) == 1) {
			printf("\rFine Tunning Neural Network Model: %.2f%s\r",
				(i / (float_t)gen_count) * 100, "%"); 
		}
	}

	printf("\n Fine Tuned Verification:\n");
	inputs = STUC_NN_INPUT(nn).cols;
	for (size_t i = 0; i < inputs; i++) {
		for (size_t j = 0; j < inputs; j++) {
			STUC_MAT_AT(STUC_NN_INPUT(nn), 0, 0) = i;
			STUC_MAT_AT(STUC_NN_INPUT(nn), 0, 1) = j;

			stuc_nnForward(nn);

			printf("  %zu ? %zu = %f\n", i, j, STUC_MAT_AT(STUC_NN_OUTPUT(nn), 0, 0));
		}
	}

	stuc_nnFree(nn);
	
	return 0;
}
