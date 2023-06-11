#ifndef STUC_H
#define STUC_H

#include <stddef.h>
#include <math.h>
#include <stdbool.h>

#define STUC_LRELU_FACT 0.1

#define STUC_MAT_AT(mat, i, j) mat.el[(i)*(mat).stride + (j)]
#define STUC_NN_AT(nn, layer)  nn.layers[(layer)]
#define MAT_PRINT(mat) stuc_matPrint((mat), (#mat), 0)
#define NN_PRINT(nn) stuc_nnPrint((nn), (#nn))
#define STUC_NN_OUTPUT(nn) STUC_NN_AT((nn), (nn).count).a
#define STUC_NN_INPUT(nn) STUC_NN_AT((nn), 0).a
#define STUC_LENP(p) sizeof (p) / sizeof ((p)[0])

typedef struct {
	size_t rows;
	size_t cols;
	size_t stride;
	float_t *el;
} stuc_mat;

void stuc_matAdd(stuc_mat a, stuc_mat b);
void stuc_matSub(stuc_mat a, stuc_mat b);
void stuc_matDot(stuc_mat dest, stuc_mat a, stuc_mat b);
void stuc_matCpy(stuc_mat dst, stuc_mat src);
bool stuc_matEq(stuc_mat a, stuc_mat b);
void stuc_matFill(stuc_mat a, float_t number);
void stuc_matPrint(stuc_mat a, char* name, int indent);
stuc_mat stuc_matAlloc(size_t rows, size_t cols);
void stuc_matFree(stuc_mat a);


typedef enum {
	STUC_ACTIVATE_RELU,
	STUC_ACTIVATE_SIGMOID,
	STUC_ACTIVATE_TANH,
	STUC_ACTIVATE_SIN,
	STUC_ACTIVATIONS_COUNT
} stuc_activationFunction;

typedef struct {
	stuc_activationFunction activation;
	stuc_mat w; // weights
	stuc_mat b; // biases
	stuc_mat a; // aktivacije
} stuc_nnLayer;

typedef struct {
	size_t count;
	stuc_nnLayer* layers; // count + 1, jer je 0. za input
} stuc_nn;

void stuc_nnForward(stuc_nn nn);
void stuc_nnFill(stuc_nn nn, float_t number);
void stuc_nnRand(stuc_nn nn, float_t low, float_t high);
void stuc_nnPrint(stuc_nn nn, char* name);
float_t stuc_nnCost(stuc_nn nn, stuc_mat tInput, stuc_mat tOutput);
void stuc_nnFiniteDiff(stuc_nn fd, stuc_nn nn, float_t eps, stuc_mat tInput, stuc_mat tOutput);
void stuc_nnApplyDiff(stuc_nn nn, stuc_nn fd, float_t learningRate);
stuc_nn stuc_nnAlloc(stuc_activationFunction* aktivacije, size_t* arhitektura, size_t arhCount);
void stuc_nnFree(stuc_nn nn); 

#ifndef STUC_MALLOC
	#define STUC_MALLOC malloc
	#include <stdlib.h>
#endif

#ifndef STUC_FREE
	#define STUC_FREE free
	#include <stdlib.h>
#endif

#ifndef STUC_ASSERT
	#define STUC_ASSERT assert
	#include <assert.h>
#endif

#ifndef NO_STDIO
	#include <stdio.h>	
#else 
	#undef MAT_PRINT
	#undef NN_PRINT
#endif


#endif // STUC_H


#ifdef STUC_IMPLEMENTATION

float_t stuc__randFloat() {
	return rand() / (float_t) RAND_MAX;
}

float_t stuc__sigmoid(float_t x) {
	return 1.f / (1 + exp(-x));
	// Deriv: sig(x) * (1 - sig(x))
}

float_t stuc__relu(float_t x) {
	return (x > 0) ? x : STUC_LRELU_FACT * x;
	// Deriv: x > 0 -> 1, x < 0 -> STUC_LRELU_FACT
}

float_t stuc__tanh(float_t x) {
	float_t ex  = exp(x);
	float_t emx = exp(-x);

	return (ex - emx) / (ex + emx);
	// Deriv: [ tanh(x) ]^2
}

float_t stuc__sin(float_t x) {
	return sin(x);
	// Deriv: cos(x);
}

void stuc__matRand(stuc_mat a, float_t low, float_t high) {
	for (size_t i = 0; i < a.rows; i++) {
		for (size_t j = 0; j < a.cols; j++) {
			STUC_MAT_AT(a, i, j) = stuc__randFloat() * (high - low) + low;
		}
	}

	return;
}

void stuc_matFill(stuc_mat a, float_t number) {
	for (size_t i = 0; i < a.rows; i++) {
		for (size_t j = 0; j < a.cols; j++){
			STUC_MAT_AT(a, i, j) = number;
		}
	}
	return;
}

stuc_mat stuc_matAlloc(size_t rows, size_t cols) {
	stuc_mat a;
	a.rows = rows;
	a.stride = cols;
	a.cols = cols;

	STUC_ASSERT(rows * cols > 0);
	a.el = STUC_MALLOC(rows * cols * sizeof *a.el);

	return a;
}

void stuc_matFree(stuc_mat a) {
	STUC_FREE(a.el);
	(void)a;

	return;
}

void stuc__matAddSub(stuc_mat a, stuc_mat b, int addSub) {
	STUC_ASSERT(addSub == 1 || addSub == -1);
	STUC_ASSERT(a.rows == b.rows);
	STUC_ASSERT(a.cols == b.cols);
	
	for (size_t i = 0; i < a.rows; i++)
		for (size_t j = 0; j < a.cols; j++)
			STUC_MAT_AT(a, i, j) += STUC_MAT_AT(b, i, j) * addSub;
	
	return;
}

void stuc_matAdd(stuc_mat a, stuc_mat b) {
	stuc__matAddSub(a, b, 1);

	return;
}

void stuc_matSub(stuc_mat a, stuc_mat b) {
	stuc__matAddSub(a, b, -1);

	return;
}

void stuc_matCpy(stuc_mat dst, stuc_mat src) {
	stuc_matFill(dst, 0);
	stuc__matAddSub(dst, src, 1);
	
	return;
}

bool stuc_matEq(stuc_mat a, stuc_mat b) {
	if (a.rows != b.rows) return false;
	if (a.cols != b.cols) return false;
	
	for (size_t i = 0; i < a.rows; i++)
		for (size_t j = 0; j < a.cols; j++)
			if (STUC_MAT_AT(a, i, j) != STUC_MAT_AT(b, i, j))
				return false;
	
	return true;
}

void stuc_matDot(stuc_mat dest, stuc_mat a, stuc_mat b) {
	STUC_ASSERT(a.cols == b.rows);
	STUC_ASSERT(dest.rows == a.rows);
	STUC_ASSERT(dest.cols == b.cols);
	stuc_matFill(dest, 0);

	for (size_t i = 0; i < dest.rows; i++) {
		for (size_t j = 0; j < dest.cols; j++) {
			for (size_t k = 0; k < a.cols; k++) {
				STUC_MAT_AT(dest, i, j) += STUC_MAT_AT(a, i, k) * STUC_MAT_AT(b, k, j);
			}
		}
	}
}

void stuc__matActivate(stuc_mat a, stuc_activationFunction f) {
	float_t (*activation)(float_t) = &stuc__sigmoid;
	
	switch(f) {
	case STUC_ACTIVATE_RELU:    activation = &stuc__relu;    break;
	case STUC_ACTIVATE_SIGMOID: activation = &stuc__sigmoid; break;
	case STUC_ACTIVATE_TANH:    activation = &stuc__tanh;    break;
	case STUC_ACTIVATE_SIN:     activation = &stuc__sin;     break;
	default: STUC_ASSERT(0 && "Not a valid activation funtion!");
	}

	STUC_ASSERT(a.rows == 1);
	STUC_ASSERT(a.cols >= 1);

	for (size_t i = 0; i < a.cols; i++) {
		STUC_MAT_AT(a, 0, i) = activation(STUC_MAT_AT(a, 0, i));
	}

	return;
}


void stuc_nnForward(stuc_nn nn) {
	
	for (size_t layer = 1; layer <= nn.count; layer++) {
		stuc_matDot(STUC_NN_AT(nn, layer).a, STUC_NN_AT(nn, layer - 1).a, STUC_NN_AT(nn, layer).w);
		stuc_matAdd(STUC_NN_AT(nn, layer).a, STUC_NN_AT(nn, layer).b);
		for (size_t i = 0; i < STUC_NN_AT(nn, layer).a.cols; i++)
		stuc__matActivate(STUC_NN_AT(nn, layer).a, STUC_NN_AT(nn, layer).activation);
	}

	return;
}

#ifndef NO_STDIO
void stuc_matPrint(stuc_mat a, char* name, int indent) {
	printf("%*s%s = [\n", (int)indent, "", name);

	for (size_t i = 0; i < a.rows; i++) {
		printf("\t%*s", (int)indent, "");
		for (size_t j = 0; j < a.cols; j++) {
			printf("%10.7f  ", STUC_MAT_AT(a, i, j));
		}
		printf("\n");
	}

	printf("%*s]\n", (int)indent, "");
}

void stuc_nnPrint(stuc_nn nn, char* name) {
	printf("\n%s = [\n", name);

	stuc_matPrint(STUC_NN_INPUT(nn), "input", 4);
	for (size_t i = 1; i <= nn.count; i++) {
		char layerName[3][30] = {0};
		snprintf(layerName[0], 30, "w%zu", i + 1);
		snprintf(layerName[1], 30, "b%zu", i + 1);
		snprintf(layerName[2], 30, "a%zu", i + 1);
		stuc_matPrint(STUC_NN_AT(nn, i).w, layerName[0], 4);
		stuc_matPrint(STUC_NN_AT(nn, i).b, layerName[1], 4);
		stuc_matPrint(STUC_NN_AT(nn, i).a, layerName[2], 4);
	}

	printf("]\n");
}
#endif // NO_STDIO

void stuc_nnFill(stuc_nn nn, float_t number) {
	stuc_matFill(STUC_NN_INPUT(nn), number);
	for (size_t i = 1; i <= nn.count; i++) {
		stuc_matFill(STUC_NN_AT(nn, i).w, number);
		stuc_matFill(STUC_NN_AT(nn, i).b, number);
		stuc_matFill(STUC_NN_AT(nn, i).a, number);
	}

	return;
}

void stuc_nnRand(stuc_nn nn, float_t low, float_t high) {
	stuc__matRand(STUC_NN_INPUT(nn), low, high);
	for (size_t i = 0; i < nn.count; i++) {
		stuc__matRand(STUC_NN_AT(nn, i).a, low, high);
		stuc__matRand(STUC_NN_AT(nn, i).w, low, high);
		stuc__matRand(STUC_NN_AT(nn, i).b, low, high);
	}

	return;
}

float_t stuc_nnCost(stuc_nn nn, stuc_mat tInput, stuc_mat tOutput) {
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
			float_t dif = STUC_MAT_AT(tOutput, i, output) - STUC_MAT_AT(STUC_NN_OUTPUT(nn), 0, output);
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

void stuc_nnFiniteDiff(stuc_nn fd, stuc_nn nn, float_t eps, stuc_mat tInput, stuc_mat tOutput) {
	STUC_ASSERT(fd.count == nn.count);
	float_t originalCost = stuc_nnCost(nn, tInput, tOutput);

	for (size_t layer = 1; layer <= nn.count; layer++) {
		for (size_t i = 0; i < STUC_NN_AT(nn, layer).w.rows; i++) {
			for (size_t j = 0; j < STUC_NN_AT(nn, layer).w.cols; j++) {
				float_t savedW = STUC_MAT_AT(STUC_NN_AT(nn, layer).w, i, j);
				
				STUC_MAT_AT(STUC_NN_AT(nn, layer).w, i, j) += eps;
				float_t costPlusEps = stuc_nnCost(nn, tInput, tOutput);

				STUC_MAT_AT(STUC_NN_AT(fd, layer).w, i, j) = (costPlusEps - originalCost) / eps; // Finite Difference

				STUC_MAT_AT(STUC_NN_AT(nn, layer).w, i, j) = savedW;
			}
		}

		for (size_t i = 0; i < STUC_NN_AT(nn, layer).b.cols; i++) {
			float_t savedB = STUC_MAT_AT(STUC_NN_AT(nn, layer).b, 0, i);
			
			STUC_MAT_AT(STUC_NN_AT(nn, layer).b, 0, i) += eps;
			float_t costPlusEps = stuc_nnCost(nn, tInput, tOutput);

			STUC_MAT_AT(STUC_NN_AT(fd, layer).b, 0, i) = (costPlusEps - originalCost) / eps; // Finite Difference

			STUC_MAT_AT(STUC_NN_AT(nn, layer).b, 0, i) = savedB;
		}
	}

	return;

}

void stuc_nnApplyDiff(stuc_nn nn, stuc_nn fd, float_t learningRate) {
	STUC_ASSERT(fd.count == nn.count);
	
	for (size_t layer = 1; layer <= nn.count; layer++) {
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

stuc_nn stuc_nnAlloc(stuc_activationFunction* aktivacije, size_t* arhitektura, size_t arhCount) {
	
	stuc_nn nn;
	nn.count  = arhCount - 1; // nn.count ne ukljucuje a[0];
	nn.layers = STUC_MALLOC(sizeof (stuc_nnLayer) * (arhCount));

	STUC_NN_INPUT(nn) = stuc_matAlloc(1, arhitektura[0]);
	for (size_t layer = 1; layer <= nn.count; layer++) {
		STUC_NN_AT(nn, layer).activation = aktivacije[layer-1];
		STUC_NN_AT(nn, layer).a = stuc_matAlloc(1, arhitektura[layer]);
		STUC_NN_AT(nn, layer).b = stuc_matAlloc(1, arhitektura[layer]);
		STUC_NN_AT(nn, layer).w = stuc_matAlloc(STUC_NN_AT(nn, layer - 1).a.cols, STUC_NN_AT(nn, layer).a.cols);
	}

	return nn;
}

void stuc_nnFree(stuc_nn nn) { 
	stuc_matFree(STUC_NN_INPUT(nn));
	for (size_t layer = 1; layer <= nn.count; layer++) {
		stuc_matFree(STUC_NN_AT(nn, layer).a);
		stuc_matFree(STUC_NN_AT(nn, layer).b);
		stuc_matFree(STUC_NN_AT(nn, layer).w);
	}

	STUC_FREE(nn.layers);

	return;
}

#endif // STUC_IMPLEMENTATION