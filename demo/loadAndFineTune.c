#include <stdint.h>
#include <stdio.h>

#include "../src/stuc/stuc.h"

float_t tData[] = {
	0, 0, 0,
	0, 1, 1,
	1, 0, 1,
	1, 1, 2
};

void verify(Stuc_nn nn, char op) {
	size_t inputs = STUC_NN_INPUT(nn).cols;
	assert(inputs == 2);

	for (size_t i = 0; i < inputs; i++) {
		for (size_t j = 0; j < inputs; j++) {
			STUC_AT_INPUT(nn, 0) = i;
			STUC_AT_INPUT(nn, 1) = j;

			stuc_nnForward(nn);

			printf("  %zu %c %zu = %f\n", i, op, j, STUC_AT_OUTPUT(nn, 0));
		}
	}

	printf("\n");
}

int main(void) {
	Stuc_nn nn;
	uint8_t ret = stuc_nnLoadFromFile(&nn, "trainedNNs/Xor.snn");
	stuc_printIOFlags(ret);
	if (ret) return ret;

	printf("\n XOR Verification:\n");
	verify(nn, '^');

	size_t number_of_inputs  = 2;
	size_t number_of_outputs = 1;
	size_t number_of_samples = 4;

	Stuc_mat tInput  = {number_of_samples, number_of_inputs,  number_of_inputs + number_of_outputs, tData};
	Stuc_mat tOutput = {number_of_samples, number_of_outputs, number_of_inputs + number_of_outputs, tData + tInput.cols};

	size_t  gen_count = 500;
	float_t learn_rate = 1;
	float_t boost_multiplier = 1;

	for (size_t i = 0; i < gen_count; i++) {
		Stuc_nn gd_map = stuc_nnBackprop(nn, tInput, tOutput, boost_multiplier);
		stuc_nnApplyDiff(nn, gd_map, learn_rate);
		stuc_nnFree(gd_map);

		if (i % (gen_count/10) == 0) {
			printf("\rcost = %.32f                          \n",
				stuc_nnCost(nn, tInput, tOutput));
		} else if (gen_count >= 10000 && i % (gen_count / 10000) == 1) {
			printf("\rFine Tunning Neural Network Model: %.2f%s\r",
				(i / (float_t)gen_count) * 100, "%");
		}
	}

	printf("\n Fine Tuned Verification:\n");
	verify(nn, '?');

	stuc_nnFree(nn);

	return 0;
}
