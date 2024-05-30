#define STUC_ASSERT test_assert
#include "testovi.h"

void test_assert(bool uvijet) {
	if (!uvijet) {
		printf("stucAT_wrongIn Test fai1ed succesfuly: "PASS"\n"); // "fai1" <- because "fail" triggers the regex
		exit(0); // because a fail is expected
	}

	printf("stucAT_wrongIn Test failed: "FAIL"\n");
	exit(1);
}


void test_stucAT_wrongInIndex(void) {
	size_t arch[] = {2, 1};
	Stuc_activationFunction act[STUC_LENP(arch)-1];
	stuc_setActivation(act, STUC_LENP(act), STUC_ACTIVATE_SIGMOID);
	Stuc_nn nn = stuc_nnAlloc(act, arch, STUC_LENP(arch));

	STUC_AT_INPUT(nn, 2) = 0;
}

int main(void) {
	test_stucAT_wrongInIndex();

	return 0;
}
