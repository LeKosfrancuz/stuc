#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int tData[][3] = {
	{0, 0, 0},
	{0, 1, 1},
	{1, 0, 1},
	{1, 1, 1}
};

float randFloat() {
	return rand() / (float) RAND_MAX;
}

float sigmoidf(float x) {
	return 1.f / (1 + exp(-x));
}

float cost(float w1, float w2, float b) {
	float modelCost = 0.f;
	const int sampleCount = sizeof(tData)/sizeof(tData[0][0])/3;
	
	for (int i = 0; i < sampleCount; i++) {
		int x1 = tData[i][0];
		int x2 = tData[i][1];
		int y  = tData[i][2];

		float dif = y - sigmoidf(x1*w1 + x2*w2 + b);
		modelCost += dif*dif;
	}

	return modelCost/sampleCount;
}

int main() {
	
	// y = x * w
	srand(time(0));
	float w1 = randFloat() * 5;
	float w2 = randFloat() * 5;
	float b  = randFloat() * 5;

	const float rate = 1e-3;
	const float eps = 1e-3;
	const size_t gen_count = 1000*1000;

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
			printf("%zu | %zu = %f\n", i, j, sigmoidf(i*w1 + j*w2 + b));
		}

	return 0;
}
