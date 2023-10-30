#include "stuc.h"

void stuc__nnFiniteDiff(Stuc_nn nn, Stuc_nn fd, float_t eps, Stuc_mat tInput, Stuc_mat tOutput);

Stuc_nn stuc_nnFiniteDiff(Stuc_nn nn, float_t eps, Stuc_mat tInput, Stuc_mat tOutput) {
	Stuc_nn fd = stuc_nnAlloc(nn.aktivacije, nn.arhitektura, nn.layerCount + 1);
	stuc__nnFiniteDiff(nn, fd, eps, tInput, tOutput);

	return fd;
}

void stuc_nnFiniteDiffNoAlloc(Stuc_nn nn, Stuc_nn fd, float_t eps, Stuc_mat tInput, Stuc_mat tOutput) {
	stuc__nnFiniteDiff(nn, fd, eps, tInput, tOutput);

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
