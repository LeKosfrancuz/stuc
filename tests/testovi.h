#ifndef TESTOVI_H
#define TESTOVI_H

#include "../src/stuc/stuc.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923
#define PASS "\x1B[1;32mpass\x1B[0;37m"
#define FAIL "\x1B[1;31mfail\x1B[0;37m"

// extern float_t stuc__activationDerivative(float_t x, Stuc_activationFunction f);

int test_activationDerivTANH_dva(void);
int test_activationDerivTANH_minusDva(void);
int test_activationDerivTANH_cetiri(void);
// TODO -> test spremanja u file

#endif // TESTOVI_H

#ifdef TESTOVI_IMPLEMENTATION

void test_runAll(void) {

	printf("Derivitive Tests: \n");

	printf("\t TANH tests:\n");
	printf("\t\t  TANH =  2: %s\n",
		test_activationDerivTANH_dva() ? PASS : FAIL);
	printf("\t\t  TANH = -2: %s\n",
		test_activationDerivTANH_minusDva() ? PASS : FAIL);
	printf("\t\t  TANH =  4: %s\n",
		test_activationDerivTANH_cetiri() ? PASS : FAIL);

	printf("Hello, World!\n");

	return;
}

#endif // TESTOVI_IMPLEMENTATION
