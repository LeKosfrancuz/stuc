#include "stuc.h"

void stuc__matActivate(Stuc_mat a, Stuc_activationFunction f);
float_t stuc__activationDerivative(float_t x, Stuc_activationFunction f);
void stuc__matAddSub(Stuc_mat a, Stuc_mat b, int addSub);

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
	a.el = (float_t *)STUC_MALLOC(rows * cols * sizeof *a.el);

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

Stuc_mat stuc_matRow(Stuc_mat a, size_t row) {
	Stuc_mat mat;
	mat.rows = 1;
	mat.cols = a.cols;
	mat.stride = a.stride;
	mat.el = &STUC_MAT_AT(a, row, 0);

	return mat;
}

#ifndef NO_STDIO
void stuc_matPrint(Stuc_mat a, char *name, int indent) {
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
#endif // NO_STDIO
