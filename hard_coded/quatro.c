#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int tData[][2] = {
	{-1, -4},
	{0, 0},
	{1, 4},
	{5, 20},
	{14, 56}
};

float cost(float w) {
	float modelCost = 0.f;
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
			printf("cost = %f, w = %f\n", cost(w), w);
		float c = cost(w);
		w -= rate* ( (cost(w + eps) - c) / eps );
	}

	printf("cost = %f, w = %f\n", cost(w), w);

	return 0;
}
