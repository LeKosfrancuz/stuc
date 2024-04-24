#include "testovi.h"

bool test_nnForward(void) {
	Stuc_mat a[] = {stuc_matAlloc(1, 2), stuc_matAlloc(1, 1)};
	Stuc_mat w1 = stuc_matAlloc(2, 1);
	Stuc_mat b1 = stuc_matAlloc(1, 1);
	
	Stuc_nnLayer layers[] = {{STUC_ACTIVATE_SIGMOID, w1, b1, a[0]}, {STUC_ACTIVATE_SIGMOID, w1, b1, a[1]}};

	Stuc_nn nn = {NULL, NULL, 2, layers};

	stuc_nnFill(nn, 14);
	size_t inputs = STUC_NN_INPUT(nn).cols;
	STUC_NN_AT(nn, 1).w.el[0] = 0;
	STUC_NN_AT(nn, 1).b.el[0] = 0;

	bool passed = true;
	for (size_t i = 0; i < inputs; i++) {
		for (size_t j = 0; j < inputs; j++) {
			STUC_NN_INPUT(nn).el[0] = i;
			STUC_NN_INPUT(nn).el[1] = j;

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
	
	stuc_matFree(a[0]);
	stuc_matFree(a[1]);
	stuc_matFree(w1);
	stuc_matFree(b1);

	return passed;
}

int main(void) {
	bool pass = test_nnForward();

	printf("Forward Test: %s\n", pass ? PASS : FAIL);
	
	return 0;
}
