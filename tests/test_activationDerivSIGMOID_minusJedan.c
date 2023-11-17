#include "testovi.h"

bool test_activationDerivSIGMOID_minusJedan(void) {
	float_t ret = stuc__activationDerivative(-1, STUC_ACTIVATE_SIGMOID);
	if (ret == -2.0) return true;

	return false;
}

int main() {
	bool pass = test_activationDerivSIGMOID_minusJedan();

	printf("\t SIGMOID tests:\n");
	printf("\t\t  SIGMOID =  -1: %s\n", pass ? PASS : FAIL);

	return 0;
}
