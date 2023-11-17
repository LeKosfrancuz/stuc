#include "testovi.h"

bool test_activationDerivRELU_veceOdNula(void) {
	float_t ret = stuc__activationDerivative(10*1000, STUC_ACTIVATE_RELU);
	if (ret == 1.0) return true;

	return false;
}

int main(void) {
	bool pass = test_activationDerivRELU_veceOdNula();

	printf("\t RELU tests:\n");
	printf("\t\t  RELU > 0: %s\n", pass ? PASS : FAIL);

	return 0;
}
