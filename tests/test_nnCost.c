#include "testovi.h"

bool test_nnCost(void) {
	float_t tData[] = {
		0, 0, 0,
		0, 1, 1,
		1, 0, 1,
		1, 1, 1
	};

	Stuc_mat tInput = {4, 2, 3, tData};
	Stuc_mat tOutput = {4, 1, 3, tData+2};

	Stuc_mat a[] = {stuc_matAlloc(1, 2), stuc_matAlloc(1, 1)};
	Stuc_mat w1 = stuc_matAlloc(2, 1);
	Stuc_mat b1 = stuc_matAlloc(1, 1);

	Stuc_nnLayer layers[] = {{STUC_ACTIVATE_SIGMOID, w1, b1, a[0]}, {STUC_ACTIVATE_SIGMOID, w1, b1, a[1]}};

	Stuc_nn nn = {NULL, NULL, 1, layers};

	STUC_MAT_AT(w1, 0, 0) = 4.645961;
	STUC_MAT_AT(w1, 1, 0) = 4.646396;
	STUC_MAT_AT(b1, 0, 0) = -7.064404;

	float_t cost = stuc_nnCost(nn, tInput, tOutput);
	float_t eps  = 1e-6; 
	float_t expectedCost = 0.423917;

	stuc_matFree(a[0]);
	stuc_matFree(a[1]);
	stuc_matFree(w1);
	stuc_matFree(b1);

	if (cost < expectedCost + eps && cost > expectedCost - eps) return true;
	else return false;
}

int main(void) {
	bool pass = test_nnCost();

	printf("nnCost Test: %s\n", pass ? PASS : FAIL);
	
	return 0;
}
