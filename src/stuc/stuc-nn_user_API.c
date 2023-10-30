#include "stuc.h"

extern void stuc__matActivate(Stuc_mat a, Stuc_activationFunction f);
extern void stuc__matRand(Stuc_mat a, float_t low, float_t high);

void stuc_nnForward(Stuc_nn nn) {
	
	for (size_t layer = 1; layer <= nn.layerCount; layer++) {
		stuc_matDot(STUC_NN_AT(nn, layer).a, STUC_NN_AT(nn, layer - 1).a, STUC_NN_AT(nn, layer).w);
		stuc_matAdd(STUC_NN_AT(nn, layer).a, STUC_NN_AT(nn, layer).b);
		for (size_t i = 0; i < STUC_NN_AT(nn, layer).a.cols; i++)
		stuc__matActivate(STUC_NN_AT(nn, layer).a, STUC_NN_AT(nn, layer).activation);
	}

	return;
}

#ifndef NO_STDIO
void stuc_nnPrint(Stuc_nn nn, char* name) {
	printf("\n%s = [\n", name);

	stuc_matPrint(STUC_NN_INPUT(nn), (char*)"input", 4);
	for (size_t i = 1; i <= nn.layerCount; i++) {
		char layerName[3][30] = {0};
		snprintf(layerName[0], sizeof (layerName[0]), "w%zu", i);
		snprintf(layerName[1], sizeof (layerName[1]), "b%zu", i);
		snprintf(layerName[2], sizeof (layerName[2]), "a%zu", i);
		stuc_matPrint(STUC_NN_AT(nn, i).w, layerName[0], 4);
		stuc_matPrint(STUC_NN_AT(nn, i).b, layerName[1], 4);
		stuc_matPrint(STUC_NN_AT(nn, i).a, layerName[2], 4);
	}

	printf("]\n");
}
#endif // NO_STDIO

void stuc_nnFill(Stuc_nn nn, float_t number) {
	stuc_matFill(STUC_NN_INPUT(nn), number);
	for (size_t i = 1; i <= nn.layerCount; i++) {
		stuc_matFill(STUC_NN_AT(nn, i).w, number);
		stuc_matFill(STUC_NN_AT(nn, i).b, number);
		stuc_matFill(STUC_NN_AT(nn, i).a, number);
	}

	return;
}

void stuc_nnRand(Stuc_nn nn, float_t low, float_t high) {
	stuc__matRand(STUC_NN_INPUT(nn), low, high);
	for (size_t i = 0; i < nn.layerCount; i++) {
		stuc__matRand(STUC_NN_AT(nn, i).a, low, high);
		stuc__matRand(STUC_NN_AT(nn, i).w, low, high);
		stuc__matRand(STUC_NN_AT(nn, i).b, low, high);
	}

	return;
}

float_t stuc_nnCost(Stuc_nn nn, Stuc_mat tInput, Stuc_mat tOutput) {
	STUC_ASSERT(tInput.rows == tOutput.rows);
	STUC_ASSERT(tInput.cols == STUC_NN_INPUT(nn).cols);
	STUC_ASSERT(tOutput.cols == STUC_NN_OUTPUT(nn).cols);

	size_t samples = tInput.rows;
	size_t inputs  = tInput.cols;
	size_t outputs = tOutput.cols;
	
	float_t cost = 0.f;

	for (size_t i = 0; i < samples; i++) {
		for (size_t input = 0; input < inputs; input++) {
			STUC_MAT_AT(STUC_NN_INPUT(nn), 0, input) = STUC_MAT_AT(tInput, i, input);
		}
		stuc_nnForward(nn);
		for (size_t output = 0; output < outputs; output++) {
			float_t dif =   STUC_MAT_AT(tOutput, i, output) 
				      - STUC_MAT_AT(STUC_NN_OUTPUT(nn), 0, output);
			cost += dif*dif;
		}
		// int x1 = (int)STUC_MAT_AT(tInput, i, 0);
		// int x2 = (int)STUC_MAT_AT(tInput, i, 1);
		// int y  = (int)STUC_MAT_AT(tOutput, i, 0);
		//
		// float_t w1 = STUC_MAT_AT(nn.wp[0], 0, 0);
		// float_t w2 = STUC_MAT_AT(nn.wp[0], 1, 0);
		// float_t b = STUC_MAT_AT(nn.bp[0], 0, 0);
		//
		// printf("x1 = %d x2 = %d y = %d\n", x1, x2, y);
		// // STUC_NN_INPUT(nn)[0] = x1;
		// // STUC_NN_INPUT(nn)[1] = x2;
		//
		// // stuc_nnForward(nn);
		// // float_t d = y - STUC_NN_OUTPUT(nn)[0];
		// float_t yr = stuc__sigmoid(x1*w1 + x2*w2 + b);
		// float_t d = y - yr;
		// printf("cost = %f yr = %f\n", d*d, yr);
		// cost += d*d;
	}

	return cost/samples;
}

void stuc_nnApplyDiff(Stuc_nn nn, Stuc_nn fd, float_t learningRate) {
	STUC_ASSERT(fd.layerCount == nn.layerCount);
	
	for (size_t layer = 1; layer <= nn.layerCount; layer++) {
		for (size_t i = 0; i < STUC_NN_AT(nn, layer).w.rows; i++) {
			for (size_t j = 0; j < STUC_NN_AT(nn, layer).w.cols; j++) {
				STUC_MAT_AT(STUC_NN_AT(nn, layer).w, i, j) -= learningRate*STUC_MAT_AT(STUC_NN_AT(fd, layer).w, i, j);
			}
		}
		for (size_t i = 0; i < STUC_NN_AT(nn, layer).b.cols; i++) {
			STUC_MAT_AT(STUC_NN_AT(nn, layer).b, 0, i) -= learningRate*STUC_MAT_AT(STUC_NN_AT(fd, layer).b, 0, i);
		}
	
	}

	return;
}

void stuc_setActivation(Stuc_activationFunction* aktivacije, size_t aktCount, Stuc_activationFunction aktivacija) {
	for (size_t i = 0; i < aktCount; i++) {
		aktivacije[i] = aktivacija;
	}

	return;
}

Stuc_nn stuc_nnAlloc(Stuc_activationFunction* aktivacije, size_t* arhitektura, size_t arhCount) {
	
	Stuc_nn nn;
	nn.arhitektura = (size_t*)STUC_MALLOC(sizeof(size_t) * arhCount);
	nn.layerCount  = arhCount - 1; // nn.layerCount ne ukljucuje a[0];
	nn.aktivacije = (Stuc_activationFunction*)STUC_MALLOC(sizeof (Stuc_activationFunction) * (arhCount - 1));
	nn.layers = (Stuc_nnLayer*)STUC_MALLOC(sizeof (Stuc_nnLayer) * (arhCount));

	for (size_t i = 0; i < arhCount - 1; i++)
		nn.aktivacije[i] = aktivacije[i];

	for (size_t i = 0; i < arhCount; i++)
		nn.arhitektura[i] = arhitektura[i];

	STUC_NN_INPUT(nn) = stuc_matAlloc(1, arhitektura[0]);
	for (size_t layer = 1; layer <= nn.layerCount; layer++) {
		STUC_NN_AT(nn, layer).activation = aktivacije[layer-1];
		STUC_NN_AT(nn, layer).a = stuc_matAlloc(1, arhitektura[layer]);
		STUC_NN_AT(nn, layer).b = stuc_matAlloc(1, arhitektura[layer]);
		STUC_NN_AT(nn, layer).w = stuc_matAlloc(STUC_NN_AT(nn, layer - 1).a.cols, STUC_NN_AT(nn, layer).a.cols);
	}

	return nn;
}

void stuc_nnFree(Stuc_nn nn) { 
	stuc_matFree(STUC_NN_INPUT(nn));
	for (size_t layer = 1; layer <= nn.layerCount; layer++) {
		stuc_matFree(STUC_NN_AT(nn, layer).a);
		stuc_matFree(STUC_NN_AT(nn, layer).b);
		stuc_matFree(STUC_NN_AT(nn, layer).w);
	}

	STUC_FREE(nn.layers);
	STUC_FREE(nn.aktivacije);
	STUC_FREE(nn.arhitektura);

	return;
}
