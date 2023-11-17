#include "testovi.h"

int test_activationDerivSIN_45(void) {
	float_t sin45 = sin(PI/4);
	float_t ret = stuc__activationDerivative(sin45, STUC_ACTIVATE_SIN);
	if (ret == (float_t)cos(PI/4)) return true;

	// printf("cos(PI/4) %f, ret %f\n", cos(PI/4), ret);
	return false;
}

int main() {
	bool pass = test_activationDerivSIN_45();

	printf("\t SIN tests:\n");
	printf("\t\t  SIN( 45°) = COS( 45°): %s\n", pass ? PASS : FAIL);

	return 0;
}
