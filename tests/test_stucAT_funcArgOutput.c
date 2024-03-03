#include "testovi.h"

bool function(void* unused1, float arg, void* unused2) {
	(void)unused1; (void)unused2; // So the caller has more args to enter
	if (arg == 4) return true;
	else return false;
}

bool test_stucAT_funcArgOutput(void) {
	size_t arch[] = {2, 1};
	Stuc_activationFunction act[STUC_LENP(arch)-1];
	stuc_setActivation(act, STUC_LENP(act), STUC_ACTIVATE_SIGMOID);
	Stuc_nn nn = stuc_nnAlloc(act, arch, STUC_LENP(arch));

	stuc_nnFill(nn, 14);
	STUC_NN_OUTPUT(nn).el[0] = 4;

	bool passed = function(NULL, STUC_AT_OUTPUT(nn, 0), NULL);
	
	stuc_nnFree(nn);
	return passed;
}

int main(void) {
	bool pass = test_stucAT_funcArgOutput();

	printf("stucAT_funcArgOutput Test: %s\n", pass ? PASS : FAIL);
	
	return 0;
}
