#include "stuc.h"

void stuc__nnFiniteDiff(Stuc_nn nn, Stuc_nn fd, float_t eps, Stuc_mat tInput, Stuc_mat tOutput);

Stuc_nn stuc_nnFiniteDiff(Stuc_nn nn, float_t eps, Stuc_mat tInput, Stuc_mat tOutput) {
	Stuc_nn fd = stuc_nnAlloc(nn.aktivacije, nn.arhitektura, nn.layer_count);
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

	float_t original_cost = stuc_nnCost(nn, tInput, tOutput);

	for (size_t layer = 1; layer < nn.layer_count; layer++) {
		for (size_t i = 0; i < STUC_NN_AT(nn, layer).w.rows; i++) {
			for (size_t j = 0; j < STUC_NN_AT(nn, layer).w.cols; j++) {
				float_t saved_w = STUC_MAT_AT(STUC_NN_AT(nn, layer).w, i, j);

				STUC_MAT_AT(STUC_NN_AT(nn, layer).w, i, j) += eps;
				float_t cost_plus_eps = stuc_nnCost(nn, tInput, tOutput);

				STUC_MAT_AT(STUC_NN_AT(fd, layer).w, i, j) = (cost_plus_eps - original_cost) / eps; // Finite Difference

				STUC_MAT_AT(STUC_NN_AT(nn, layer).w, i, j) = saved_w;
			}
		}

		for (size_t i = 0; i < STUC_NN_AT(nn, layer).b.cols; i++) {
			float_t saved_b = STUC_MAT_AT(STUC_NN_AT(nn, layer).b, 0, i);

			STUC_MAT_AT(STUC_NN_AT(nn, layer).b, 0, i) += eps;
			float_t cost_plus_eps = stuc_nnCost(nn, tInput, tOutput);

			STUC_MAT_AT(STUC_NN_AT(fd, layer).b, 0, i) = (cost_plus_eps - original_cost) / eps; // Finite Difference

			STUC_MAT_AT(STUC_NN_AT(nn, layer).b, 0, i) = saved_b;
		}
	}

	return;

}
