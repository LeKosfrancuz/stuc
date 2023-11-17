#include "testovi.h"

int test_activationDerivSIN_0(void) {
	float_t sin0 = sin(0);
	float_t ret = stuc__activationDerivative(sin0, STUC_ACTIVATE_SIN);
	if (ret == (float_t)cos(0)) return true;

	// printf("cos(0) %f, ret %f\n", cos(0), ret);
	return false;
}

int main() {
	bool pass = test_activationDerivSIN_0();

	printf("\t SIN tests:\n");
	printf("\t\t  SIN(  0°) = COS(  0°): %s\n", pass ? PASS : FAIL);

	return 0;
}
