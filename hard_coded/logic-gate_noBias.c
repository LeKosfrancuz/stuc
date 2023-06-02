#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int tData[][3] = {
	{0, 0, 0},
	{0, 1, 1},
	{1, 0, 1},
	{1, 1, 1}
};

float cost(float w1, float w2) {
	float modelCost = 0.f;
	const int sampleCount = sizeof(tData)/sizeof(tData[0][0])/3;
	
	for (int i = 0; i < sampleCount; i++) {
		int x1 = tData[i][0];
		int x2 = tData[i][1];
		int y  = tData[i][2];

		float dif = y - (x1*w1 + x2*w2);
		modelCost += dif*dif;
	}

	return modelCost/sampleCount;
}

float randFloat() {
	return rand() / (float) RAND_MAX;
}

int main() {
	
	// y = x * w
	srand(time(0));
	float w1 = randFloat() * 5;
	float w2 = randFloat() * 5;

	const float rate = 1e-4;
	const float eps = 1e-4;

	for (int i = 0; i < 1000*1000; i++) {
		if (i % 10*1000 == 0)
			printf("cost = %f, w1 = %f, w2 = %f\n", cost(w1, w2), w1, w2);

		float c = cost(w1, w2);

		w1 -= rate* ( (cost(w1 + eps, w2) - c) / eps );
		w2 -= rate* ( (cost(w1, w2 + eps) - c) / eps );
	}

	printf("cost = %f, w1 = %f, w2 = %f\n", cost(w1, w2), w1, w2);


	for (size_t i = 0; i < 2; i++)
		for (size_t j = 0; j < 2; j++) {
			printf("%zu | %zu = %f\n", i, j, (i*w1 + j*w2));
		}

	return 0;
}
