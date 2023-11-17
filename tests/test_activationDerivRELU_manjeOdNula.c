#include "testovi.h"

bool test_activationDerivRELU_manjeOdNula(void) {
	float_t ret = stuc__activationDerivative(-10*1000, STUC_ACTIVATE_RELU);
	if (ret == (float_t)STUC_LRELU_FACT) return true;

	return false;
}

int main(void) {
	bool pass = test_activationDerivRELU_manjeOdNula();

	printf("\t RELU tests:\n");
	printf("\t\t  RELU < 0: %s\n", pass ? PASS : FAIL);

	return 0;
}
