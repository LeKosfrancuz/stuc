#include "testovi.h"

bool test_activationDerivSIGMOID_jednaPolovina(void) {
	float_t ret = stuc__activationDerivative(0.5, STUC_ACTIVATE_SIGMOID);
	if (ret == 0.25) return true;

	return false;
}

int main() {
	bool pass = test_activationDerivSIGMOID_jednaPolovina();

	printf("\t SIGMOID tests:\n");
	printf("\t\t  SIGMOID = 0.5: %s\n", pass ? PASS : FAIL);

	return 0;
}
