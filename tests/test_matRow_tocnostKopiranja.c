#include "testovi.h"

int test_matRow_tocnostKopiranja_1(void) {
	float_t tData[] = {
		1, 2, 0,
		3, 4, 1,
		5, 6, 2,
		7, 8, 3
	};

	Stuc_mat tInput = {4, 2, 3, tData};

	Stuc_mat row = stuc_matRowView(tInput, 2);


	if (STUC_MAT_AT(row, 0, 0) == 5.0 && STUC_MAT_AT(row, 0, 1) == 6.0)
		return true;

	return false;
}

int test_matRow_tocnostKopiranja_2(void) {
	float_t tData[] = {
		0, 0, 0, 0, 0, 0, 0, 1, 6, 2,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 3, 5
	};

	Stuc_mat tInput = {6, 2, 10, tData+8};

	Stuc_mat row = stuc_matRowView(tInput, tInput.rows-1);


	if (STUC_MAT_AT(row, 0, 0) == 3.0 && STUC_MAT_AT(row, 0, 1) == 5.0)
		return true;

	return false;
}

int main(void) {
	bool pass = test_matRow_tocnostKopiranja_1();
	printf("matRow Test tocnostKopiranja_1: %s\n", pass ? PASS : FAIL);

	pass = test_matRow_tocnostKopiranja_2();
	printf("matRow Test tocnostKopiranja_2: %s\n", pass ? PASS : FAIL);

	return 0;
}
