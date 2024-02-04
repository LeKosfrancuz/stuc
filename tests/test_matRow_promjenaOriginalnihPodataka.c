#include "testovi.h"

int test_matRow_promjenaOriginala(void) {
	float_t tData[] = {
		0, 0, 0,
		1, 2, 3,
		0, 0, 0,
		0, 0, 0
	};

	Stuc_mat tInput = {4, 3, 3, tData};

	Stuc_mat row = stuc_matRowView(tInput, 1);


	STUC_MAT_AT(row, 0, 0) = 4;
	STUC_MAT_AT(row, 0, 1) = 5;
	STUC_MAT_AT(row, 0, 2) = 6;

	if (STUC_MAT_AT(tInput, 1, 0) == 4.0 && STUC_MAT_AT(tInput, 1, 1) == 5.0 && STUC_MAT_AT(tInput, 1, 2) == 6.0)
		return true;

	return false;
}

int main(void) {
	bool pass = test_matRow_promjenaOriginala();

	printf("matRow Test promjena originala: %s\n", pass ? PASS : FAIL);

	return 0;
}
