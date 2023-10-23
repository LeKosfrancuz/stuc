#ifndef TESTOVI_H
#define TESTOVI_H

#include "../src/stuc.h"
#include <stdio.h>
#include <time.h>

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923
#define PASS "\x1B[1;32mpass\x1B[0;37m"
#define FAIL "\x1B[1;31mfail\x1B[0;37m"

int test_nnForward(void);
int test_matDot_Redosljed(void);
int test_matDot_RazliciteVelicine(void);
int test_nnCost(void);
int test_activationDerivRELU_veceOdNula(void);
int test_activationDerivRELU_manjeOdNula(void);
int test_activationDerivRELU_jednakoNula(void);
int test_activationDerivSIGMOID_minusJedan(void);
int test_activationDerivSIGMOID_plusJedan(void);
int test_activationDerivSIGMOID_jednaPolovina(void);
int test_activationDerivTANH_dva(void);
int test_activationDerivTANH_minusDva(void);
int test_activationDerivTANH_cetiri(void);
int test_activationDerivSIN_0(void);
int test_activationDerivSIN_45(void);
int test_activationDerivSIN_90(void);
int test_activationDerivSIN_180(void);
int test_matRow_tocnostKopiranja(void);
// TODO -> test spremanja u file

#endif // TESTOVI_H

#ifdef TESTOVI_IMPLEMENTATION

void test_runAll(void) {
	printf("Forward Test: %s\n", 
		test_nnForward() ? PASS : FAIL);

	printf("matDot Tests: \n");
	printf("\t\t  Redosljed: %s\n", 
		test_matDot_Redosljed() ? PASS : FAIL);
	printf("\t Razlicite Velicine: %s\n", 
		test_matDot_RazliciteVelicine() ? PASS : FAIL);
	printf("matRow Test: %s\n", 
		test_matRow_tocnostKopiranja() ? PASS : FAIL);
	printf("nnCost Test: %s\n", 
		test_nnCost() ? PASS : FAIL);

	printf("Derivitive Tests: \n");
	printf("\t RELU tests:\n");
	printf("\t\t  RELU > 0: %s\n", 
		test_activationDerivRELU_veceOdNula() ? PASS : FAIL);
	printf("\t\t  RELU < 0: %s\n", 
		test_activationDerivRELU_manjeOdNula() ? PASS : FAIL);
	printf("\t\t  RELU = 0: %s\n", 
		test_activationDerivRELU_jednakoNula() ? PASS : FAIL);

	printf("\t SIGMOID tests:\n");
	printf("\t\t  SIGMOID =  -1: %s\n", 
		test_activationDerivSIGMOID_minusJedan() ? PASS : FAIL);
	printf("\t\t  SIGMOID = 0.5: %s\n", 
		test_activationDerivSIGMOID_jednaPolovina() ? PASS : FAIL);
	printf("\t\t  SIGMOID =   1: %s\n", 
		test_activationDerivSIGMOID_plusJedan() ? PASS : FAIL);

	printf("\t TANH tests:\n");
	printf("\t\t  TANH =  2: %s\n", 
		test_activationDerivTANH_dva() ? PASS : FAIL);
	printf("\t\t  TANH = -2: %s\n", 
		test_activationDerivTANH_minusDva() ? PASS : FAIL);
	printf("\t\t  TANH =  4: %s\n", 
		test_activationDerivTANH_cetiri() ? PASS : FAIL);

	printf("\t SIN tests:\n");
	printf("\t\t  SIN =   0: %s\n", 
		test_activationDerivSIN_0() ? PASS : FAIL);
	printf("\t\t  SIN =  45: %s\n", 
		test_activationDerivSIN_45() ? PASS : FAIL);
	printf("\t\t  SIN =  90: %s\n", 
		test_activationDerivSIN_90() ? PASS : FAIL);
	printf("\t\t  SIN = 180: %s\n", 
		test_activationDerivSIN_180() ? PASS : FAIL);
	printf("Hello, World!\n");
 
	return;
}


int test_nnForward(void) {
	Stuc_mat a[] = {stuc_matAlloc(1, 2), stuc_matAlloc(1, 1)};
	Stuc_mat w1 = stuc_matAlloc(2, 1);
	Stuc_mat b1 = stuc_matAlloc(1, 1);
	
	Stuc_nnLayer layers[] = {{STUC_ACTIVATE_SIGMOID, w1, b1, a[0]}, {STUC_ACTIVATE_SIGMOID, w1, b1, a[1]}};

	Stuc_nn nn = {NULL, NULL, 1, layers};

	stuc_nnFill(nn, 14);
	size_t inputs = STUC_NN_INPUT(nn).cols;
	STUC_NN_AT(nn, 0).w.el[0] = 0;
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

int test_matDot_RazliciteVelicine(void) {
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

int test_matDot_Redosljed(void) {
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
int test_matAdd_1(void) {
	return 1;
}

int test_nnCost(void) {
	float_t tData[] = {
		0, 0, 0,
		0, 1, 1,
		1, 0, 1,
		1, 1, 1
	};

	Stuc_mat tInput = {4, 2, 3, tData};
	Stuc_mat tOutput = {4, 1, 3, tData+2};

	Stuc_mat a[] = {stuc_matAlloc(1, 2), stuc_matAlloc(1, 1)};
	Stuc_mat w1 = stuc_matAlloc(2, 1);
	Stuc_mat b1 = stuc_matAlloc(1, 1);

	Stuc_nnLayer layers[] = {{STUC_ACTIVATE_SIGMOID, w1, b1, a[0]}, {STUC_ACTIVATE_SIGMOID, w1, b1, a[1]}};

	Stuc_nn nn = {NULL, NULL, 1, layers};

	STUC_MAT_AT(w1, 0, 0) = 4.645961;
	STUC_MAT_AT(w1, 1, 0) = 4.646396;
	STUC_MAT_AT(b1, 0, 0) = -7.064404;

	float_t cost = stuc_nnCost(nn, tInput, tOutput);
	float_t eps  = 1e-6; 
	float_t expectedCost = 0.423917;

	stuc_matFree(a[0]);
	stuc_matFree(a[1]);
	stuc_matFree(w1);
	stuc_matFree(b1);

	if (cost < expectedCost + eps && cost > expectedCost - eps) return true;
	else return false;
}

extern float_t stuc__activationDerivative(float_t x, Stuc_activationFunction f);

int test_activationDerivRELU_veceOdNula(void) {
	float_t ret = stuc__activationDerivative(10*1000, STUC_ACTIVATE_RELU);
	if (ret == 1.0) return true;

	return false;
}

int test_activationDerivRELU_manjeOdNula(void) {
	float_t ret = stuc__activationDerivative(-10*1000, STUC_ACTIVATE_RELU);
	if (ret == (float_t)STUC_LRELU_FACT) return true;

	return false;
}

int test_activationDerivRELU_jednakoNula(void) {
	float_t ret = stuc__activationDerivative(0, STUC_ACTIVATE_RELU);
	if (ret == (float_t)STUC_LRELU_FACT) return true;

	return false;
}

int test_activationDerivSIGMOID_minusJedan(void) {
	float_t ret = stuc__activationDerivative(-1, STUC_ACTIVATE_SIGMOID);
	if (ret == -2.0) return true;

	return false;
}

int test_activationDerivSIGMOID_plusJedan(void) {
	float_t ret = stuc__activationDerivative(1, STUC_ACTIVATE_SIGMOID);
	if (ret == 0.0) return true;

	return false;
}

int test_activationDerivSIGMOID_jednaPolovina(void) {
	float_t ret = stuc__activationDerivative(0.5, STUC_ACTIVATE_SIGMOID);
	if (ret == 0.25) return true;

	return false;
}

int test_activationDerivTANH_dva(void) {
	float_t ret = stuc__activationDerivative(2, STUC_ACTIVATE_TANH);
	if (ret == -3) return true;

	return false;
}

int test_activationDerivTANH_minusDva(void) {
	float_t ret = stuc__activationDerivative(-2, STUC_ACTIVATE_TANH);
	if (ret == -3) return true;

	return false;
}
int test_activationDerivTANH_cetiri(void) {
	float_t ret = stuc__activationDerivative(4, STUC_ACTIVATE_TANH);
	if (ret == -15) return true;

	return false;
}

int test_activationDerivSIN_0(void) {
	float_t sin0 = sin(0);
	float_t ret = stuc__activationDerivative(sin0, STUC_ACTIVATE_SIN);
	if (ret == (float_t)cos(0)) return true;

	// printf("cos(0) %f, ret %f\n", cos(0), ret);
	return false;
}

int test_activationDerivSIN_45(void) {
	float_t sin45 = sin(PI/4);
	float_t ret = stuc__activationDerivative(sin45, STUC_ACTIVATE_SIN);
	if (ret == (float_t)cos(PI/4)) return true;

	// printf("cos(PI/4) %f, ret %f\n", cos(PI/4), ret);
	return false;
}

int test_activationDerivSIN_90(void) {
	float_t sin90 = sin(PI/2);
	float_t ret = stuc__activationDerivative(sin90, STUC_ACTIVATE_SIN);
	if (ret == (float_t)cos(PI/2)) return true;

	// printf("cos(PI/2) %f, ret %f\n", cos(PI/2), ret);
	return false;
}

int test_activationDerivSIN_180(void) {
	float_t sin180 = sin(PI);
	float_t ret = stuc__activationDerivative(sin180, STUC_ACTIVATE_SIN);
	if (ret == (float_t)cos(PI)) return true;

	// printf("cos(PI) %f, ret %f\n", cos(PI), ret);
	return false;
}

int test_matRow_tocnostKopiranja(void) {
	float_t tData[] = {
		1, 2, 0,
		3, 4, 1,
		5, 6, 2,
		7, 8, 3
	};

	Stuc_mat tInput = {4, 2, 3, tData};

	Stuc_mat row = stuc_matRow(tInput, 2);


	if (STUC_MAT_AT(row, 0, 0) == 5.0 && STUC_MAT_AT(row, 0, 1) == 6.0)
		return true;

	return false;
}

#endif // TESTOVI_IMPLEMENTATION
