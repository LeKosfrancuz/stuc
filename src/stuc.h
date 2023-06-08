#ifndef STUC_H
#define STUC_H

#include <stddef.h>
#include <math.h>
#include <stdbool.h>

#define STUC_MAT_AT(mat, i, j) mat.el[(i)*(mat).stride + (j)]
#define MAT_PRINT(mat) stuc_matPrint((mat), (#mat), 0)
#define NN_PRINT(nn) stuc_nnPrint((nn), (#nn))
#define STUC_NN_OUTPUT(nn) (nn).ap[(nn).count].el
#define STUC_NN_INPUT(nn) (nn).ap[0].el
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


typedef struct {
	size_t count;
	stuc_mat *wp; // weights
	stuc_mat *bp; // biases
	stuc_mat *ap; // count + 1 (jer je a0 input)
} stuc_nn;

void stuc_nnForward(stuc_nn nn);
void stuc_nnFill(stuc_nn nn, float_t number);
void stuc_nnRand(stuc_nn nn, float_t low, float_t high);
void stuc_nnPrint(stuc_nn nn, char* name);
float_t stuc_nnCost(stuc_nn nn, stuc_mat tInput, stuc_mat tOutput);
void stuc_nnFiniteDiff(stuc_nn fd, stuc_nn nn, float_t eps, stuc_mat tInput, stuc_mat tOutput);
void stuc_nnApplyDiff(stuc_nn nn, stuc_nn fd, float_t learningRate);
stuc_nn stuc_nnAlloc(size_t* arhitektura, size_t arhCount);
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
#endif


#endif // STUC_H


#ifdef STUC_IMPLEMENTATION

float_t stuc__randFloat() {
	return rand() / (float_t) RAND_MAX;
}

float_t stuc__sigmoid(float_t x) {
	return 1.f / (1 + exp(-x));
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



void stuc_nnForward(stuc_nn nn) {
	
	for (size_t layer = 0; layer < nn.count; layer++) {
		stuc_matDot(nn.ap[layer + 1], nn.ap[layer], nn.wp[layer]);
		stuc_matAdd(nn.ap[layer + 1], nn.bp[layer]);
		for (size_t i = 0; i < nn.ap[layer + 1].cols; i++)
			STUC_MAT_AT(nn.ap[layer + 1], 0, i) = stuc__sigmoid(STUC_MAT_AT(nn.ap[layer + 1], 0, i)); 
	}

	return;
}

void stuc_nnPrint(stuc_nn nn, char* name) {
	printf("\n%s = [\n", name);

	stuc_matPrint(nn.ap[0], "input", 4);
	for (size_t i = 0; i < nn.count; i++) {
		char layerName[3][30] = {0};
		snprintf(layerName[0], 30, "w%zu", i + 1);
		snprintf(layerName[1], 30, "b%zu", i + 1);
		snprintf(layerName[2], 30, "a%zu", i + 1);
		stuc_matPrint(nn.wp[i], layerName[0], 4);
		stuc_matPrint(nn.bp[i], layerName[1], 4);
		stuc_matPrint(nn.ap[i + 1], layerName[2], 4);
	}
		#undef LAYER_NUM

	printf("]\n");
}

void stuc_nnFill(stuc_nn nn, float_t number) {
	stuc_matFill(nn.ap[0], number);
	for (size_t i = 0; i < nn.count; i++) {
		stuc_matFill(nn.wp[i], number);
		stuc_matFill(nn.bp[i], number);
		stuc_matFill(nn.ap[i + 1], number);
	}

	return;
}

void stuc_nnRand(stuc_nn nn, float_t low, float_t high) {
	stuc__matRand(nn.ap[0], low, high);
	for (size_t i = 0; i < nn.count; i++) {
		stuc__matRand(nn.ap[i + 1], low, high);
		stuc__matRand(nn.wp[i], low, high);
		stuc__matRand(nn.bp[i], low, high);
	}

	return;
}

float_t stuc_nnCost(stuc_nn nn, stuc_mat tInput, stuc_mat tOutput) {
	STUC_ASSERT(tInput.rows == tOutput.rows);
	STUC_ASSERT(tInput.cols == nn.ap[0].cols);
	STUC_ASSERT(tOutput.cols == nn.ap[nn.count].cols);

	size_t samples = tInput.rows;
	size_t inputs  = tInput.cols;
	size_t outputs = tOutput.cols;
	
	float_t cost = 0.f;

	for (size_t i = 0; i < samples; i++) {
		for (size_t input = 0; input < inputs; input++) {
			STUC_NN_INPUT(nn)[input] = STUC_MAT_AT(tInput, i, input);
		}
		stuc_nnForward(nn);
		for (size_t output = 0; output < outputs; output++) {
			float_t dif = STUC_MAT_AT(tOutput, i, output) - STUC_NN_OUTPUT(nn)[output];
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

	for (size_t layer = 0; layer < nn.count; layer++) {
		for (size_t i = 0; i < nn.wp[layer].rows; i++) {
			for (size_t j = 0; j < nn.wp[layer].cols; j++) {
				float_t savedW = STUC_MAT_AT(nn.wp[layer], i, j);
				
				STUC_MAT_AT(nn.wp[layer], i, j) += eps;
				float_t costPlusEps = stuc_nnCost(nn, tInput, tOutput);

				STUC_MAT_AT(fd.wp[layer], i, j) = (costPlusEps - originalCost) / eps; // Finite Difference

				STUC_MAT_AT(nn.wp[layer], i, j) = savedW;
			}
		}

		for (size_t i = 0; i < nn.bp[layer].cols; i++) {
			float_t savedB = STUC_MAT_AT(nn.bp[layer], 0, i);
			
			STUC_MAT_AT(nn.bp[layer], 0, i) += eps;
			float_t costPlusEps = stuc_nnCost(nn, tInput, tOutput);

			STUC_MAT_AT(fd.bp[layer], 0, i) = (costPlusEps - originalCost) / eps; // Finite Difference

			STUC_MAT_AT(nn.bp[layer], 0, i) = savedB;
		}
	}

	return;

}

void stuc_nnApplyDiff(stuc_nn nn, stuc_nn fd, float_t learningRate) {
	STUC_ASSERT(fd.count == nn.count);
	
	for (size_t layer = 0; layer < nn.count; layer++) {
		for (size_t i = 0; i < nn.wp[layer].rows; i++) {
			for (size_t j = 0; j < nn.wp[layer].cols; j++) {
				STUC_MAT_AT(nn.wp[layer], i, j) -= learningRate*STUC_MAT_AT(fd.wp[layer], i, j);
			}
		}
		for (size_t i = 0; i < nn.bp[layer].cols; i++) {
			STUC_MAT_AT(nn.bp[layer], 0, i) -= learningRate*STUC_MAT_AT(fd.bp[layer], 0, i);
		}
	
	}

	return;
}

stuc_nn stuc_nnAlloc(size_t* arhitektura, size_t arhCount) {
	
	stuc_nn nn;
	nn.count = arhCount - 1; // nn.count ne ukljucuje a[0];
	nn.wp    = STUC_MALLOC(sizeof (stuc_mat) * (arhCount - 1));
	nn.bp    = STUC_MALLOC(sizeof (stuc_mat) * (arhCount - 1));
	nn.ap    = STUC_MALLOC(sizeof (stuc_mat) * (arhCount));

	nn.ap[0] = stuc_matAlloc(1, arhitektura[0]);
	for (size_t layer = 1; layer < arhCount; layer++) {
		nn.ap[layer] = stuc_matAlloc(1, arhitektura[layer]);
		nn.bp[layer - 1] = stuc_matAlloc(1, arhitektura[layer]);
		nn.wp[layer - 1] = stuc_matAlloc(nn.ap[layer - 1].cols, nn.ap[layer].cols);
	}

	return nn;
}

void stuc_nnFree(stuc_nn nn) {
	stuc_matFree(nn.ap[0]);
	for (size_t layer = 1; layer < nn.count + 1; layer++) {
		stuc_matFree(nn.ap[layer]);
		stuc_matFree(nn.bp[layer - 1]);
		stuc_matFree(nn.wp[layer - 1]);
	}

	STUC_FREE(nn.wp);
	STUC_FREE(nn.bp);
	STUC_FREE(nn.ap);

	return;
}

#endif // STUC_IMPLEMENTATION
