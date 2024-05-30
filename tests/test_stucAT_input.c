#include "testovi.h"

bool test_stucAT_input(void) {
	size_t arch[] = {2, 1};
	Stuc_activationFunction act[STUC_LENP(arch)-1];
	stuc_setActivation(act, STUC_LENP(act), STUC_ACTIVATE_SIGMOID);
	Stuc_nn nn = stuc_nnAlloc(act, arch, STUC_LENP(arch));

	stuc_nnFill(nn, 14);
	size_t inputs = STUC_NN_INPUT(nn).cols;
	STUC_NN_AT(nn, 1).w.el[0] = 0;
	STUC_NN_AT(nn, 1).b.el[0] = 0;

	bool passed = true;
	for (size_t i = 0; i < inputs; i++) {
		for (size_t j = 0; j < inputs; j++) {
			STUC_AT_INPUT(nn, 0) = i;
			STUC_AT_INPUT(nn, 1) = j;

			stuc_nnForward(nn);
			float_t res = STUC_NN_OUTPUT(nn).el[0];
			if (j % 2 == 0) {
				if (res != 0.5)
					passed = false;
			} else {
				if (res == 0.5)
					passed = false;
			}

		}
	}

	stuc_nnFree(nn);
	return passed;
}

int main(void) {
	bool pass = test_stucAT_input();

	printf("stucAT_input Test: %s\n", pass ? PASS : FAIL);

	return 0;
}
