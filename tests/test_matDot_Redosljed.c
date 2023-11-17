#include "testovi.h"

bool test_matDot_Redosljed(void) {
	float_t aDat[] = {16, 2, 3.25, 4};
	float_t bDat[] = {2, 2, -6.5, -1};

	Stuc_mat a = {2, 2, 2, aDat};
	Stuc_mat b = {2, 2, 2, bDat};
	Stuc_mat dest = stuc_matAlloc(2, 2);
	
	stuc_matDot(dest, a, b);
	
	if (STUC_MAT_AT(dest, 0, 0) != 19) return false;
	if (STUC_MAT_AT(dest, 0, 1) != 30) return false;
	if (STUC_MAT_AT(dest, 1, 0) != -19.5) return false;
	if (STUC_MAT_AT(dest, 1, 1) != 2.5) return false;
	
	stuc_matFree(dest);


	return true;
}

int main(void) {
	bool pass = test_matDot_Redosljed();

	printf("matDot Tests: \n");
	printf("\t\t  Redosljed: %s\n", pass ? PASS : FAIL);

	return 0;
}
