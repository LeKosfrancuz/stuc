#include "stuc.h"

float_t stuc__activationDerivative(Stuc_nn nn, size_t layer, size_t neuron);
void stuc__nnBackprop(Stuc_nn nn, Stuc_nn gd_map, Stuc_mat tInput, Stuc_mat tOutput, float_t boost);
void stuc__calculateBackpropForLayerActivation(Stuc_nn nn, Stuc_nn gd_map, size_t layer, size_t act, float_t boost);
float_t stuc__derivSoftmax(Stuc_nn nn, size_t layer, size_t neuron);

float_t stuc__activationDerivative(Stuc_nn nn, size_t layer, size_t neuron) {
	//x - aktiviran neuron
	float_t x = STUC_MAT_AT(STUC_NN_AT(nn, layer).a, 0, neuron);
	switch(STUC_NN_AT(nn, layer).activation) {
		case STUC_ACTIVATE_RELU:	return (x > 0) ? 1 : STUC_LRELU_FACT;
		case STUC_ACTIVATE_SIGMOID:	return x * (1 - x);
		case STUC_ACTIVATE_TANH:	return 1 - x * x;
		case STUC_ACTIVATE_SIN:		return cos(asin(x));
		case STUC_ACTIVATE_SILU:	return 0; // TODO
		case STUC_ACTIVATE_SOFTMAX:	return stuc__derivSoftmax(nn, layer, neuron);
		default: STUC_ASSERT(0 && "The activation function is not supported!");
	}
	
	return 0.0f;
}

float_t stuc__derivSoftmax(Stuc_nn nn, size_t layer, size_t neuron) {
	// x je već aktiviran, prvit ću se da nije
	double expSum = 0;
	Stuc_mat curr_layer_act = STUC_NN_AT(nn, layer).a;
	for (size_t i = 0; i < curr_layer_act.cols ; i++)
	{
		expSum += exp(STUC_MAT_AT(curr_layer_act, 0, i));
	}
	double ex = exp(STUC_MAT_AT(curr_layer_act, 0, neuron));
	return (ex * expSum - ex * ex) / (expSum * expSum);
}

Stuc_nn stuc_nnBackprop(Stuc_nn nn, Stuc_mat tInput, Stuc_mat tOutput, float_t boost) {
	Stuc_nn gd_map = stuc_nnAlloc(nn.aktivacije, nn.arhitektura, nn.layer_count);
	stuc__nnBackprop(nn, gd_map, tInput, tOutput, boost);

	return gd_map;
}

void stuc_nnBackpropNoAlloc(Stuc_nn nn, Stuc_nn gd_map, Stuc_mat tInput, Stuc_mat tOutput, float_t boost) {
	stuc__nnBackprop(nn, gd_map, tInput, tOutput, boost);
	return;
}


void stuc__calculateBackpropForLayerActivation(Stuc_nn nn, Stuc_nn gd_map, size_t layer, size_t act, float_t boost) {
	// layer -> current layer index
	// act -> current activation index

	float_t deltaAct = STUC_MAT_AT(STUC_NN_AT(gd_map, layer).a, 0, act);
	float_t derivAct = stuc__activationDerivative(nn, layer, act);

	STUC_MAT_AT(STUC_NN_AT(gd_map, layer).b, 0, act) += boost * deltaAct * derivAct;

	for (size_t prevAct = 0; prevAct < STUC_NN_AT(nn, layer - 1).a.cols; prevAct++) {
		// act - weight matrix col
		// prevAct - weight matrix row
		float_t previousAct = STUC_MAT_AT(STUC_NN_AT(nn, layer - 1).a, 0, prevAct);
		float_t currentWeight = STUC_MAT_AT(STUC_NN_AT(nn, layer).w, prevAct, act);

		STUC_MAT_AT(STUC_NN_AT(gd_map, layer).w, prevAct, act) += boost * deltaAct * derivAct * previousAct;
		STUC_MAT_AT(STUC_NN_AT(gd_map, layer - 1).a, 0, prevAct) += boost * deltaAct * derivAct * currentWeight;
	}
}

// Matematički izvor: 
// https://github.com/tsoding/ml-notes/blob/master/papers/grad.pdf
void stuc__nnBackprop(Stuc_nn nn, Stuc_nn gd_map, Stuc_mat tInput, Stuc_mat tOutput, float_t boost) {
	STUC_ASSERT(tInput.rows == tOutput.rows);
	STUC_ASSERT(tInput.cols == STUC_NN_INPUT(nn).cols);
	STUC_ASSERT(tOutput.cols == STUC_NN_OUTPUT(nn).cols);

	STUC_ASSERT(gd_map.layer_count == nn.layer_count);
	// STUC_SOFT_ASSERT(gd_map.arhitektura == nn.arhitektura);

	size_t sampleCount = tInput.rows;
	stuc_nnFill(gd_map, 0.0);

	for (size_t sample = 0; sample < sampleCount; sample++) {
		stuc_matCpy(STUC_NN_INPUT(nn), stuc_matRowView(tInput, sample));
		stuc_nnForward(nn);

		for (size_t i = 0; i < gd_map.layer_count; i++) {
			stuc_matFill(STUC_NN_AT(gd_map, i).a, 0);
		}

		for (size_t i = 0; i < tOutput.cols; i++) {
			STUC_MAT_AT(STUC_NN_OUTPUT(gd_map), 0, i)
				= boost *  (STUC_MAT_AT(STUC_NN_OUTPUT(nn), 0, i)
					  - STUC_MAT_AT(tOutput, sample, i));
		}

		// layer - current
		for (size_t layer = nn.layer_count - 1; layer > 0; layer--) {
			// activation - current
			for (size_t act = 0; act < STUC_NN_AT(nn, layer).a.cols; act++) {
				stuc__calculateBackpropForLayerActivation(nn, gd_map, layer, act, boost);
			}
		}
	}

	for (size_t i = 1; i < gd_map.layer_count; i++) {
		for (size_t j = 0; j < STUC_NN_AT(gd_map, i).w.rows; j++) {
			for (size_t k = 0; k < STUC_NN_AT(gd_map, i).w.cols; k++) {
				STUC_MAT_AT(STUC_NN_AT(gd_map, i).w, j, k) /= sampleCount;
			}
		}
		for (size_t j = 0; j < STUC_NN_AT(gd_map, i).b.cols; j++) {
			STUC_MAT_AT(STUC_NN_AT(gd_map, i).b, 0, j) /= sampleCount;
		}
	}

	return;
}
