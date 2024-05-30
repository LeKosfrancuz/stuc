#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int tData[][2] = {
	{7, 13},
	{5, 3},
	{8, 1},
	{1, -1},
	// {-1, -4},
	// { 0,  0},
	// { 1,  4},
	// { 5, 20},
	// {14, 56}
};

float cost(float w) {
	float modelCost = 0.0f;
	const int sampleCount = sizeof(tData)/sizeof(tData[0][0])/2;

	for (int i = 0; i < sampleCount; i++) {
		int x = tData[i][0];
		int y = tData[i][1];

		float dif = y - x*w;
		modelCost += dif*dif;
	}

	return modelCost/sampleCount;
}

float randFloat() {
	return rand() / (float)RAND_MAX;
}

int main() {

	// y = x * w
	srand(time(0));
	float w = randFloat() * 5;
	float rate = 1e-4;
	float eps = 1e-4;

	for (int i = 0; i < 1000; i++) {
		if (i % 100 == 0)
			printf("cost = %10f, w = %f\n", cost(w), w);
		float c = cost(w);
		w -= rate* ( (cost(w + eps) - c) / eps );
	}

	printf("cost = %10f, w = %f\n", cost(w), w);

	int shouldExit = 0;
	do {
		float num;
		printf("Unesi broj: ");
		scanf("%f", &num);

		if (num == 0) shouldExit = 1;
		float rez = w*num;
		printf("%.2f * 4 = %.2f\n", num, rez);
	} while(!shouldExit);

	return 0;
}
