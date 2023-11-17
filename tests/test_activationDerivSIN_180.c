#include "testovi.h"

int test_activationDerivSIN_180(void) {
	float_t sin180 = sin(PI);
	float_t ret = stuc__activationDerivative(sin180, STUC_ACTIVATE_SIN);
	if (ret == (float_t)cos(PI) || ret == (float_t)cos(0)) return true;

	// printf("cos(PI) %f, ret %f\n", cos(PI), ret);
	return false;
}

int main() {
	bool pass = test_activationDerivSIN_180();

	printf("\t SIN tests:\n");
	printf("\t\t  SIN(180°) = COS(180°): %s\n", pass ? PASS : FAIL);

	return 0;
}
