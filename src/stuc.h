#ifndef STUC_H
#define STUC_H

#include <stddef.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define STUC_LRELU_FACT 0.1

#define STUC_MAT_AT(mat, i, j) mat.el[(i)*(mat).stride + (j)]
#define STUC_NN_AT(nn, layer)  nn.layers[(layer)]
#define MAT_PRINT(mat) stuc_matPrint((mat), (#mat), 0)
#define NN_PRINT(nn) stuc_nnPrint((nn), (#nn))
#define STUC_NN_OUTPUT(nn) STUC_NN_AT((nn), (nn).layerCount).a
#define STUC_NN_INPUT(nn) STUC_NN_AT((nn), 0).a
#define STUC_LENP(p) sizeof (p) / sizeof ((p)[0])

typedef struct {
	size_t rows;
	size_t cols;
	size_t stride;
	float_t *el;
} Stuc_mat;

void stuc_matAdd(Stuc_mat a, Stuc_mat b);
void stuc_matSub(Stuc_mat a, Stuc_mat b);
void stuc_matDot(Stuc_mat dest, Stuc_mat a, Stuc_mat b);
void stuc_matCpy(Stuc_mat dst, Stuc_mat src);
bool stuc_matEq(Stuc_mat a, Stuc_mat b);
void stuc_matFill(Stuc_mat a, float_t number);
void stuc_matPrint(Stuc_mat a, char* name, int indent);
Stuc_mat stuc_matAlloc(size_t rows, size_t cols);
void stuc_matFree(Stuc_mat a);
Stuc_mat stuc_matRow(Stuc_mat a, size_t row);


typedef enum {
	STUC_ACTIVATE_RELU,
	STUC_ACTIVATE_SIGMOID,
	STUC_ACTIVATE_TANH,
	STUC_ACTIVATE_SIN,
	STUC_ACTIVATIONS_COUNT
} Stuc_activationFunction;

typedef struct {
	Stuc_activationFunction activation;
	Stuc_mat w; // weights
	Stuc_mat b; // biases
	Stuc_mat a; // aktivacije
} Stuc_nnLayer;

typedef struct {
	Stuc_activationFunction* aktivacije;   // tip aktivacije po sloju (ptr na tablicu)
	size_t* arhitektura;
	size_t  layerCount;
	Stuc_nnLayer* layers; // count + 1, jer je 0. za input
} Stuc_nn;

void stuc_nnForward(Stuc_nn nn);
void stuc_nnFill(Stuc_nn nn, float_t number);
void stuc_nnRand(Stuc_nn nn, float_t low, float_t high);
void stuc_nnPrint(Stuc_nn nn, char* name);
float_t stuc_nnCost(Stuc_nn nn, Stuc_mat tInput, Stuc_mat tOutput);
Stuc_nn stuc_nnBackprop(Stuc_nn nn, Stuc_mat tInput, Stuc_mat tOutput, float_t boost);
void stuc_nnBackpropNoAlloc(Stuc_nn nn, Stuc_nn gdMap, Stuc_mat tInput, Stuc_mat tOutput, float_t boost);
Stuc_nn stuc_nnFiniteDiff(Stuc_nn nn, float_t eps, Stuc_mat tInput, Stuc_mat tOutput);
void stuc_nnFiniteDiffNoAlloc(Stuc_nn nn, Stuc_nn fd, float_t eps, Stuc_mat tInput, Stuc_mat tOutput);
void stuc_nnApplyDiff(Stuc_nn nn, Stuc_nn fd, float_t learningRate);
Stuc_nn stuc_nnAlloc(Stuc_activationFunction* aktivacije, size_t* arhitektura, size_t arhCount);
void stuc_setActivation(Stuc_activationFunction* aktivacije, size_t aktCount, Stuc_activationFunction aktivacija);
void stuc_nnFree(Stuc_nn nn); 
uint8_t stuc_nnSaveToFile(Stuc_nn nn, const char* filePath);
Stuc_nn stuc_nnLoadFromFile(const char* filePath, uint8_t* returnFlags);
void stuc_printIOFlags(uint8_t flags);

typedef enum {
STUC_IOFLAG_FILE_ERROR      = 0x1 << 0,
STUC_IOFLAG_TYPE_MISMATCH   = 0x1 << 1,
STUC_IOFLAG_UNABLE_TO_READ  = 0x1 << 2,
STUC_IOFLAG_UNABLE_TO_WRITE = 0x1 << 3,
STUC_IOFLAG_WRONG_FILE_TYPE = 0x1 << 4,
STUC_IOFLAG_COUNT = 5,
} fileIOFlags;

#ifndef STUC_MALLOC
	#define STUC_MALLOC malloc_set0
	#define malloc_set0(n) calloc((n), 1); 
	#include <stdlib.h>
#endif // STUC_MALLOC

#ifndef STUC_FREE
	#define STUC_FREE free
	#include <stdlib.h>
#endif // STUC_FREE

#ifndef STUC_ASSERT
	#define STUC_ASSERT assert
	#define STUC_SOFT_ASSERT(uvijet) ((uvijet) ? 1 : 0 * printf("\x1b[1;31mAssert failed\x1b[0;37m in file: %s:%d %s: `%s`\n", __FILE__, __LINE__, __func__, #uvijet))
	#include <assert.h>
#else
	#define STUC_SOFT_ASSERT STUC_ASSERT
#endif // STUC_ASSERT

#ifndef NO_STDIO
	#include <stdio.h>	
#else 
	#undef STUC_SOFT_ASSERT
	#define STUC_SOFT_ASSERT STUC_ASSERT
	#undef MAT_PRINT
	#undef NN_PRINT
#endif // NO_STDIO


#endif // STUC_H


#ifdef STUC_IMPLEMENTATION

#define STUC_FILE_PREFIX "MKstucNN"

void stuc__matActivate(Stuc_mat a, Stuc_activationFunction f);
float_t stuc__activationDerivative(float_t x, Stuc_activationFunction f);
void stuc__matAddSub(Stuc_mat a, Stuc_mat b, int addSub);
void stuc__nnBackprop(Stuc_nn nn, Stuc_nn gdMap, Stuc_mat tInput, Stuc_mat tOutput, float_t boost);
void stuc__nnFiniteDiff(Stuc_nn nn, Stuc_nn fd, float_t eps, Stuc_mat tInput, Stuc_mat tOutput);

float_t stuc__randFloat(void) {
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
	// Deriv: 1 - [ tanh(x) ]^2
}

float_t stuc__sin(float_t x) {
	return sin(x);
	// Deriv: cos(x);
}

void stuc__matRand(Stuc_mat a, float_t low, float_t high) {
	for (size_t i = 0; i < a.rows; i++) {
		for (size_t j = 0; j < a.cols; j++) {
			STUC_MAT_AT(a, i, j) = stuc__randFloat() * (high - low) + low;
		}
	}

	return;
}

void stuc_matFill(Stuc_mat a, float_t number) {
	for (size_t i = 0; i < a.rows; i++) {
		for (size_t j = 0; j < a.cols; j++) {
			STUC_MAT_AT(a, i, j) = number;
		}
	}
	return;
}

Stuc_mat stuc_matAlloc(size_t rows, size_t cols) {
	Stuc_mat a;
	a.rows = rows;
	a.stride = cols;
	a.cols = cols;

	STUC_ASSERT(rows * cols > 0);
	a.el = (float_t*)STUC_MALLOC(rows * cols * sizeof *a.el);

	return a;
}

void stuc_matFree(Stuc_mat a) {
	STUC_FREE(a.el);
	(void)a;

	return;
}

void stuc__matAddSub(Stuc_mat a, Stuc_mat b, int addSub) {
	STUC_ASSERT(addSub == 1 || addSub == -1);
	STUC_ASSERT(a.rows == b.rows);
	STUC_ASSERT(a.cols == b.cols);
	
	for (size_t i = 0; i < a.rows; i++)
		for (size_t j = 0; j < a.cols; j++)
			STUC_MAT_AT(a, i, j) += STUC_MAT_AT(b, i, j) * addSub;
	
	return;
}

void stuc_matAdd(Stuc_mat a, Stuc_mat b) {
	stuc__matAddSub(a, b, 1);

	return;
}

void stuc_matSub(Stuc_mat a, Stuc_mat b) {
	stuc__matAddSub(a, b, -1);

	return;
}

void stuc_matCpy(Stuc_mat dst, Stuc_mat src) {
	stuc_matFill(dst, 0);
	stuc__matAddSub(dst, src, 1);
	
	return;
}

bool stuc_matEq(Stuc_mat a, Stuc_mat b) {
	if (a.rows != b.rows) return false;
	if (a.cols != b.cols) return false;
	
	for (size_t i = 0; i < a.rows; i++)
		for (size_t j = 0; j < a.cols; j++)
			if (STUC_MAT_AT(a, i, j) != STUC_MAT_AT(b, i, j))
				return false;
	
	return true;
}

void stuc_matDot(Stuc_mat dest, Stuc_mat a, Stuc_mat b) {
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

void stuc__matActivate(Stuc_mat a, Stuc_activationFunction f) {
	float_t (*activation)(float_t) = &stuc__sigmoid;
	
	switch(f) {
	case STUC_ACTIVATE_RELU:    activation = &stuc__relu;    break;
	case STUC_ACTIVATE_SIGMOID: activation = &stuc__sigmoid; break;
	case STUC_ACTIVATE_TANH:    activation = &stuc__tanh;    break;
	case STUC_ACTIVATE_SIN:     activation = &stuc__sin;     break;
	default: STUC_ASSERT(0 && "Not a valid activation funtion!");
	}

	STUC_ASSERT(a.rows == 1);
	STUC_SOFT_ASSERT(a.cols >= 1);

	for (size_t i = 0; i < a.cols; i++) {
		STUC_MAT_AT(a, 0, i) = activation(STUC_MAT_AT(a, 0, i));
	}

	return;
}

float_t stuc__activationDerivative(float_t x, Stuc_activationFunction f) {
	//x - aktiviran neuron
	switch(f) {
		case STUC_ACTIVATE_RELU:	return (x > 0) ? 1 : STUC_LRELU_FACT;
		case STUC_ACTIVATE_SIGMOID:	return x * (1 - x);
		case STUC_ACTIVATE_TANH:	return 1 - x * x;
		case STUC_ACTIVATE_SIN:		return cos(asin(x));
		default: STUC_ASSERT(0 && "The activation function is not supported!");
	}
	
	return 0.0f;
}


void stuc_nnForward(Stuc_nn nn) {
	
	for (size_t layer = 1; layer <= nn.layerCount; layer++) {
		stuc_matDot(STUC_NN_AT(nn, layer).a, STUC_NN_AT(nn, layer - 1).a, STUC_NN_AT(nn, layer).w);
		stuc_matAdd(STUC_NN_AT(nn, layer).a, STUC_NN_AT(nn, layer).b);
		for (size_t i = 0; i < STUC_NN_AT(nn, layer).a.cols; i++)
		stuc__matActivate(STUC_NN_AT(nn, layer).a, STUC_NN_AT(nn, layer).activation);
	}

	return;
}

Stuc_mat stuc_matRow(Stuc_mat a, size_t row) {
	Stuc_mat mat;
	mat.rows = 1;
	mat.cols = a.cols;
	mat.stride = a.stride;
	mat.el = &STUC_MAT_AT(a, row, 0);

	return mat;
}

#ifndef NO_STDIO
void stuc_matPrint(Stuc_mat a, char* name, int indent) {
	printf("%*s%s = [\n", (int)indent, "", name);

	for (size_t i = 0; i < a.rows; i++) {
		printf("\t%*s", (int)indent, "");
		for (size_t j = 0; j < a.cols; j++) {
			printf("%10.9f  ", STUC_MAT_AT(a, i, j));
		}
		printf("\n");
	}

	printf("%*s]\n", (int)indent, "");
}

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

Stuc_nn stuc_nnBackprop(Stuc_nn nn, Stuc_mat tInput, Stuc_mat tOutput, float_t boost) {
	Stuc_nn gdMap = stuc_nnAlloc(nn.aktivacije, nn.arhitektura, nn.layerCount + 1);
	stuc__nnBackprop(nn, gdMap, tInput, tOutput, boost);

	return gdMap;
}

void stuc_nnBackpropNoAlloc(Stuc_nn nn, Stuc_nn gdMap, Stuc_mat tInput, Stuc_mat tOutput, float_t boost) {
	stuc__nnBackprop(nn, gdMap, tInput, tOutput, boost);

	return;
}

// Matematički izvor: 
// https://github.com/tsoding/ml-notes/blob/master/papers/grad.pdf
void stuc__nnBackprop(Stuc_nn nn, Stuc_nn gdMap, Stuc_mat tInput, Stuc_mat tOutput, float_t boost) {
	STUC_ASSERT(tInput.rows == tOutput.rows);
	STUC_ASSERT(tInput.cols == STUC_NN_INPUT(nn).cols);
	STUC_ASSERT(tOutput.cols == STUC_NN_OUTPUT(nn).cols);

	STUC_ASSERT(gdMap.layerCount == nn.layerCount);
	// STUC_SOFT_ASSERT(gdMap.arhitektura == nn.arhitektura);

	size_t sampleCount = tInput.rows;
	stuc_nnFill(gdMap, 0.0);

	for (size_t sample = 0; sample < sampleCount; sample++) {
		stuc_matCpy(STUC_NN_INPUT(nn), stuc_matRow(tInput, sample));
		stuc_nnForward(nn);

		for (size_t i = 0; i <= gdMap.layerCount; i++) {
			stuc_matFill(STUC_NN_AT(gdMap, i).a, 0);
		}

		for (size_t i = 0; i < tOutput.cols; i++) {
			STUC_MAT_AT(STUC_NN_OUTPUT(gdMap), 0, i) = boost * (STUC_MAT_AT(STUC_NN_OUTPUT(nn), 0, i) - STUC_MAT_AT(tOutput, sample, i));
		}

		// layer - current
		for (size_t layer = nn.layerCount; layer > 0; layer--) {
			// activation - current
			for (size_t act = 0; act < STUC_NN_AT(nn, layer).a.cols; act++) {
				float_t currentAct = STUC_MAT_AT(STUC_NN_AT(nn, layer).a, 0, act);
				float_t deltaAct = STUC_MAT_AT(STUC_NN_AT(gdMap, layer).a, 0, act);
				float_t derivAct = stuc__activationDerivative(currentAct, STUC_NN_AT(nn, layer).activation);

				STUC_MAT_AT(STUC_NN_AT(gdMap, layer).b, 0, act) += boost * deltaAct * derivAct;

				for (size_t prevAct = 0; prevAct < STUC_NN_AT(nn, layer - 1).a.cols; prevAct++) {
					// act - weight matrix col
					// prevAct - weight matrix row
					float_t previousAct = STUC_MAT_AT(STUC_NN_AT(nn, layer - 1).a, 0, prevAct);
					float_t currentWeight = STUC_MAT_AT(STUC_NN_AT(nn, layer).w, prevAct, act);

					STUC_MAT_AT(STUC_NN_AT(gdMap, layer).w, prevAct, act) += boost * deltaAct * derivAct * previousAct;
					STUC_MAT_AT(STUC_NN_AT(gdMap, layer - 1).a, 0, prevAct) += boost * deltaAct * derivAct * currentWeight;
					// TODO: podijeliti a_novi sa sampleCount <- prema formuli
				}
			}
		}
	}

	for (size_t i = 1; i <= gdMap.layerCount; i++) {
		for (size_t j = 0; j < STUC_NN_AT(gdMap, i).w.rows; j++) {
			for (size_t k = 0; k < STUC_NN_AT(gdMap, i).w.cols; k++) {
				STUC_MAT_AT(STUC_NN_AT(gdMap, i).w, j, k) /= sampleCount;
			}
		}
		for (size_t j = 0; j < STUC_NN_AT(gdMap, i).b.cols; j++) {
			STUC_MAT_AT(STUC_NN_AT(gdMap, i).b, 0, j) /= sampleCount;
		}
	}

	return;
}

void stuc__nnFiniteDiff(Stuc_nn nn, Stuc_nn fd, float_t eps, Stuc_mat tInput, Stuc_mat tOutput) {
	STUC_ASSERT(tInput.rows == tOutput.rows);
	STUC_ASSERT(tInput.cols == STUC_NN_INPUT(nn).cols);
	STUC_ASSERT(tOutput.cols == STUC_NN_OUTPUT(nn).cols);

	float_t originalCost = stuc_nnCost(nn, tInput, tOutput);

	for (size_t layer = 1; layer <= nn.layerCount; layer++) {
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

Stuc_nn stuc_nnFiniteDiff(Stuc_nn nn, float_t eps, Stuc_mat tInput, Stuc_mat tOutput) {
	Stuc_nn fd = stuc_nnAlloc(nn.aktivacije, nn.arhitektura, nn.layerCount + 1);
	stuc__nnFiniteDiff(nn, fd, eps, tInput, tOutput);

	return fd;
}

void stuc_nnFiniteDiffNoAlloc(Stuc_nn nn, Stuc_nn fd, float_t eps, Stuc_mat tInput, Stuc_mat tOutput) {
	stuc__nnFiniteDiff(nn, fd, eps, tInput, tOutput);

	return;
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

#ifndef NO_STDIO
uint8_t stuc_nnSaveToFile(Stuc_nn nn, const char* filePath) {
	FILE *fp;
	uint8_t returnFlags = 0;

	fp = fopen(filePath, "wb");
	if (fp == NULL) {
		fprintf(stderr, "\x1b[1;31mError\x1b[0;37m opening file \"%s\": %s\n", filePath, strerror(errno));
		return returnFlags |= STUC_IOFLAG_UNABLE_TO_WRITE;
	}
	
	if (!STUC_SOFT_ASSERT(strlen(STUC_FILE_PREFIX) == 8)) returnFlags |= STUC_IOFLAG_TYPE_MISMATCH;
	fwrite(STUC_FILE_PREFIX, strlen(STUC_FILE_PREFIX), 1, fp);

	if (!STUC_SOFT_ASSERT(sizeof(typeof(nn.layerCount)) == 8)) returnFlags |= STUC_IOFLAG_TYPE_MISMATCH;
	fwrite(&nn.layerCount, sizeof(typeof(nn.layerCount)), 1, fp);

	STUC_ASSERT(sizeof(typeof(nn.arhitektura[0])) == sizeof(size_t));
	fwrite(nn.arhitektura, sizeof(size_t), nn.layerCount + 1, fp);

	STUC_ASSERT(sizeof(typeof(nn.aktivacije[0])) == sizeof(Stuc_activationFunction));
	fwrite(nn.aktivacije, sizeof(Stuc_activationFunction), nn.layerCount, fp);

	for (size_t i = 1; i <= nn.layerCount; i++) {
		STUC_ASSERT(STUC_NN_AT(nn, i).w.cols == STUC_NN_AT(nn, i).b.cols && "Neural network is corrupt!");

		const size_t rows = STUC_NN_AT(nn, i).w.rows;
		const size_t cols = STUC_NN_AT(nn, i).w.cols;

		fwrite(&i, sizeof(i), 1, fp);
		fwrite(&rows, sizeof(rows), 1, fp);
		fwrite(&cols, sizeof(cols), 1, fp);
		fwrite(STUC_NN_AT(nn, i).b.el, sizeof(float_t), cols, fp);
		fwrite(STUC_NN_AT(nn, i).w.el, sizeof(float_t), cols * rows, fp);
	
	}

	fclose(fp);

	return returnFlags;
}

void s_fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t ret = fread(ptr, size, nmemb, stream);
        if (ret != nmemb) {
            fprintf(stderr, "\x1b[1;31mError\x1b[0;37m reading from file: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

	return;
}

Stuc_nn stuc_nnLoadFromFile(const char* filePath, uint8_t* returnFlags) {
	Stuc_nn nn;

	FILE *fp;
	*returnFlags = 0;

	fp = fopen(filePath, "rb");
	if (fp == NULL) {
		fprintf(stderr, "\x1b[1;31mError\x1b[0;37m opening file \"%s\": %s\n", filePath, strerror(errno));
		*returnFlags |= STUC_IOFLAG_UNABLE_TO_READ;
		return nn;
	}
	
	if (!STUC_SOFT_ASSERT(strlen(STUC_FILE_PREFIX) == 8)) {
		*returnFlags |= STUC_IOFLAG_TYPE_MISMATCH;
		return nn;
	}

	char buff[9] = {0};
	s_fread(buff, strlen(STUC_FILE_PREFIX), 1, fp);

	if (strcmp(buff, STUC_FILE_PREFIX)) {
		*returnFlags |= STUC_IOFLAG_WRONG_FILE_TYPE;
		return nn;
	}

	if (!STUC_SOFT_ASSERT(sizeof(typeof(nn.layerCount)) == 8)) {
		*returnFlags |= STUC_IOFLAG_TYPE_MISMATCH;
		return nn;
	}

	s_fread(&nn.layerCount, sizeof(typeof(nn.layerCount)), 1, fp);

	STUC_ASSERT(sizeof(typeof(nn.arhitektura[0])) == sizeof(size_t));
	size_t* temp_arh = (size_t*)STUC_MALLOC(nn.layerCount + 1);
	s_fread(temp_arh, sizeof(size_t), nn.layerCount + 1, fp);

	STUC_ASSERT(sizeof(typeof(nn.aktivacije[0])) == sizeof(Stuc_activationFunction));
	Stuc_activationFunction* temp_act = (Stuc_activationFunction*)STUC_MALLOC(nn.layerCount);
	s_fread(temp_act, sizeof(Stuc_activationFunction), nn.layerCount, fp);

	nn = stuc_nnAlloc(temp_act, temp_arh, nn.layerCount + 1);
	free(temp_act);
	free(temp_arh);

	for (size_t i = 1; i <= nn.layerCount; i++) {
		size_t rows;
		size_t cols;
		size_t tempID;

		s_fread(&tempID, sizeof(tempID), 1, fp);
		(void)STUC_SOFT_ASSERT(tempID == i);

		s_fread(&rows, sizeof(rows), 1, fp);
		s_fread(&cols, sizeof(cols), 1, fp);
		if (!STUC_SOFT_ASSERT(rows == STUC_NN_AT(nn, i).w.rows)) *returnFlags |= STUC_IOFLAG_FILE_ERROR;
		if (!STUC_SOFT_ASSERT(cols == STUC_NN_AT(nn, i).w.cols)) *returnFlags |= STUC_IOFLAG_FILE_ERROR;

		s_fread(STUC_NN_AT(nn, i).b.el, sizeof(float_t), cols , fp);
		s_fread(STUC_NN_AT(nn, i).w.el, sizeof(float_t), cols * rows, fp);
	
	}

	fclose(fp);

	return nn;
}

void stuc_printIOFlags(uint8_t flags) {
	for (size_t i = 0; i < STUC_IOFLAG_COUNT; i++)
	switch (flags & (0x1 << i)) {
		case STUC_IOFLAG_TYPE_MISMATCH: printf("Data type mismatch\n"); break;
		case STUC_IOFLAG_WRONG_FILE_TYPE: printf("Wrong file type\n"); break;
		case STUC_IOFLAG_UNABLE_TO_READ: printf("Unable to Read File\n"); break;
		case STUC_IOFLAG_FILE_ERROR: printf("File is corupted or not right\n"); break;
		case STUC_IOFLAG_UNABLE_TO_WRITE: printf("Unable to write to file\n"); break;
		default: break;
	}

	return;
}

#endif // NO_STDIO

#endif // STUC_IMPLEMENTATION
