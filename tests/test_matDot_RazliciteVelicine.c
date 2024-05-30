#include "testovi.h"

bool test_matDot_RazliciteVelicine(void) {
	float_t aDat[] = {1, 2, 3, 4, 5, 6};
	float_t bDat[] = {16, -12, 5.125};

	Stuc_mat a = {2, 3, 3, aDat};
	Stuc_mat b = {3, 1, 1, bDat};
	Stuc_mat dest = stuc_matAlloc(2, 1);

	stuc_matDot(dest, a, b);

	if (STUC_MAT_AT(dest, 0, 0) != 7.375) return false;
	if (STUC_MAT_AT(dest, 1, 0) != 34.75) return false;

	stuc_matFree(dest);
	return true;
}


int main(void) {
	bool pass = test_matDot_RazliciteVelicine();

	printf("matDot Tests: \n");
	printf("\t Razlicite Velicine: %s\n", pass ? PASS : FAIL);

	return 0;
}
