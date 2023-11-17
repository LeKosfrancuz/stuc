#include "testovi.h"

int test_activationDerivSIN_90(void) {
	float_t sin90 = sin(PI/2);
	float_t ret = stuc__activationDerivative(sin90, STUC_ACTIVATE_SIN);
	if (ret == (float_t)cos(PI/2)) return true;

	// printf("cos(PI/2) %f, ret %f\n", cos(PI/2), ret);
	return false;
}

int main() {
	bool pass = test_activationDerivSIN_90();

	printf("\t SIN tests:\n");
	printf("\t\t  SIN( 90°) = COS( 90°): %s\n", pass ? PASS : FAIL);

	return 0;
}
