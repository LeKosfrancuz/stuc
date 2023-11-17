#include "testovi.h"

bool test_activationDerivRELU_jednakoNula(void) {
	float_t ret = stuc__activationDerivative(0, STUC_ACTIVATE_RELU);
	if (ret == (float_t)STUC_LRELU_FACT) return true;

	return false;
}

int main(void) {
	bool pass = test_activationDerivRELU_jednakoNula();

	printf("\t RELU tests:\n");
	printf("\t\t  RELU = 0: %s\n", pass ? PASS : FAIL);

	return 0;
}
