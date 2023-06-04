#define TESTOVI_IMPLEMENTATION
#include "testovi.c"
#define STUC_IMPLEMENTATION
#include "stuc.h"
#include <stdio.h>
#include <time.h>

float_t tData[] = {
	0, 0, 0,
	0, 1, 1,
	1, 0, 1,
	1, 1, 1
};

int main() {
	test_runAll(); 

	// srand(time(0));
	srand(14);
	stuc_mat tInput = {4, 2, 3, tData};
	stuc_mat tOutput = {4, 1, 3, tData+2};

	stuc_mat a[] = {stuc_matAlloc(1, 2), stuc_matAlloc(1, 1)};
	stuc_mat w1 = stuc_matAlloc(2, 1);
	stuc_mat b1 = stuc_matAlloc(1, 1);

	stuc_nn nn = {1, &w1, &b1, a};

	//cost = 0.005711
	MAT_AT(w1, 0, 0) = 4.645961;
	MAT_AT(w1, 1, 0) = 4.646396;
	MAT_AT(b1, 0, 0) = -7.064404;
	//cost = 0.138656
	// MAT_AT(w1, 0, 0) = 1.524217;
	// MAT_AT(w1, 1, 0) = 1.650366;
	// MAT_AT(b1, 0, 0) = -1.594348;

	NN_PRINT(nn); 
	size_t inputs = nn.ap[0].cols;
	for (size_t i = 0; i < inputs; i++) {
		for (size_t j = 0; j < inputs; j++) {
			STUC_NN_INPUT(nn)[0] = i;
			STUC_NN_INPUT(nn)[1] = j;

			stuc_nnForward(nn);

			printf("%zu | %zu = %f\n", i, j, STUC_NN_OUTPUT(nn)[0]);
		}
	}

	printf("cost = %f\n", stuc_nnCost(nn, tInput, tOutput));

	return 0;
	/*
	float w1 = randFloat() * 5;
	float w2 = randFloat() * 5;
	float b  = randFloat() * 5;

	const float rate = 1;
	const float eps = 1e-3;
	const size_t gen_count = 1000;
	
	for (size_t i = 0; i < gen_count; i++) {
		if (i % (gen_count / 10) == 0)
			printf("%04zu: cost = %f, w1 = %f, w2 = %f, b = %f\n", i, cost(w1, w2, b), w1, w2, b);

		float c = cost(w1, w2, b);

		w1 -= rate* ( (cost(w1 + eps, w2, b) - c) / eps );
		w2 -= rate* ( (cost(w1, w2 + eps, b) - c) / eps );
		b  -= rate* ( (cost(w1, w2, b + eps) - c) / eps );
	}

	printf("%zu: cost = %f, w1 = %f, w2 = %f, b = %f\n",gen_count, cost(w1, w2, b), w1, w2, b);


	for (size_t i = 0; i < 2; i++)
		for (size_t j = 0; j < 2; j++) {
			printf("%zu | %zu = %f\n", i, j, sigmoid(i*w1 + j*w2 + b));
		}

	return 0;
	*/
}

