#ifndef TESTOVI_H
#define TESTOVI_H

#include "../src/stuc.h"
#include <stdio.h>
#include <time.h>

#define PASS "\x1B[1;32mpass\x1B[0;37m"
#define FAIL "\x1B[1;31mfail\x1B[0;37m"

int test_nnForward();
int test_matDot_Redosljed();
int test_matDot_RazliciteVelicine();
int test_nnCost();

#endif // TESTOVI_H

#ifdef TESTOVI_IMPLEMENTATION

void test_runAll() {
	printf("Forward Test: %s\n", 
		test_nnForward() ? PASS : FAIL);
	printf("matDot Tests: \n");
	printf("\t\t  Redosljed: %s\n", 
		test_matDot_Redosljed() ? PASS : FAIL);
	printf("\t Razlicite Velicine: %s\n", 
		test_matDot_RazliciteVelicine() ? PASS : FAIL);
	printf("nnCost Test: %s\n", 
		test_nnCost() ? PASS : FAIL);
	printf("Hello, World!\n");
 
	return;
}


int test_nnForward() {
	stuc_mat a[] = {stuc_matAlloc(1, 2), stuc_matAlloc(1, 1)};
	stuc_mat w1 = stuc_matAlloc(2, 1);
	stuc_mat b1 = stuc_matAlloc(1, 1);
	
	stuc_nnLayer layers[] = {{STUC_ACTIVATE_SIGMOID, w1, b1, a[0]}, {STUC_ACTIVATE_SIGMOID, w1, b1, a[1]}};

	stuc_nn nn = {1, layers};

	stuc_matFill(a[0], 14);
	// MAT_PRINT(a[0]);
	// NN_PRINT(nn);
	stuc_nnFill(nn, 14);
	size_t inputs = STUC_NN_INPUT(nn).cols;
	STUC_NN_AT(nn, 0).w.el[0] = 0;
	STUC_NN_AT(nn, 0).b.el[1] = 0;
	STUC_NN_AT(nn, 0).b.el[0] = 0;

	bool passed = true;
	for (size_t i = 0; i < inputs; i++) {
		for (size_t j = 0; j < inputs; j++) {
			STUC_NN_INPUT(nn).el[0] = i;
			STUC_NN_INPUT(nn).el[1] = j;

			stuc_nnForward(nn);
			float_t res = STUC_NN_OUTPUT(nn).el[0];
			if (j % 2 == 0) {
				if (res != 0.5)
					passed = false;
			} else {
				if (res == 0.5)
					passed = false;
			}

		}
	}
	
	stuc_matFree(a[0]);
	stuc_matFree(a[1]);
	stuc_matFree(w1);
	stuc_matFree(b1);

	return passed;
}

int test_matDot_RazliciteVelicine() {
	float_t aDat[] = {1, 2, 3, 4, 5, 6};
	float_t bDat[] = {16, -12, 5.125};

	stuc_mat a = {2, 3, 3, aDat};
	stuc_mat b = {3, 1, 1, bDat};
	stuc_mat dest = stuc_matAlloc(2, 1);
	
	stuc_matDot(dest, a, b);
	
	if (STUC_MAT_AT(dest, 0, 0) != 7.375) return false;
	if (STUC_MAT_AT(dest, 1, 0) != 34.75) return false;
	
	stuc_matFree(dest);
	return true;
}

int test_matDot_Redosljed() {
	float_t aDat[] = {16, 2, 3.25, 4};
	float_t bDat[] = {2, 2, -6.5, -1};

	stuc_mat a = {2, 2, 2, aDat};
	stuc_mat b = {2, 2, 2, bDat};
	stuc_mat dest = stuc_matAlloc(2, 2);
	
	stuc_matDot(dest, a, b);
	
	if (STUC_MAT_AT(dest, 0, 0) != 19) return false;
	if (STUC_MAT_AT(dest, 0, 1) != 30) return false;
	if (STUC_MAT_AT(dest, 1, 0) != -19.5) return false;
	if (STUC_MAT_AT(dest, 1, 1) != 2.5) return false;
	
	stuc_matFree(dest);


	return true;
}
int test_matAdd_1() {
	return 1;
}

int test_nnCost() {
	float_t tData[] = {
		0, 0, 0,
		0, 1, 1,
		1, 0, 1,
		1, 1, 1
	};

	stuc_mat tInput = {4, 2, 3, tData};
	stuc_mat tOutput = {4, 1, 3, tData+2};

	stuc_mat a[] = {stuc_matAlloc(1, 2), stuc_matAlloc(1, 1)};
	stuc_mat w1 = stuc_matAlloc(2, 1);
	stuc_mat b1 = stuc_matAlloc(1, 1);

	stuc_nnLayer layers[] = {{STUC_ACTIVATE_SIGMOID, w1, b1, a[0]}, {STUC_ACTIVATE_SIGMOID, w1, b1, a[1]}};

	stuc_nn nn = {1, layers};

	STUC_MAT_AT(w1, 0, 0) = 4.645961;
	STUC_MAT_AT(w1, 1, 0) = 4.646396;
	STUC_MAT_AT(b1, 0, 0) = -7.064404;

	float_t cost = stuc_nnCost(nn, tInput, tOutput);
	float_t eps  = 1e-6; 
	float_t expectedCost = 0.423917;

	if (cost < expectedCost + eps && cost > expectedCost - eps) return true;
	else return false;
}

#endif // TESTOVI_IMPLEMENTATION
