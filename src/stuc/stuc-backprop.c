#include "stuc.h"

float_t stuc__activationDerivative(float_t x, Stuc_activationFunction f);
void stuc__nnBackprop(Stuc_nn nn, Stuc_nn gdMap, Stuc_mat tInput, Stuc_mat tOutput, float_t boost);
void stuc__calculateBackpropForLayerActivation(Stuc_nn nn, Stuc_nn gdMap, size_t layer, size_t act, float_t boost);

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

Stuc_nn stuc_nnBackprop(Stuc_nn nn, Stuc_mat tInput, Stuc_mat tOutput, float_t boost) {
	Stuc_nn gdMap = stuc_nnAlloc(nn.aktivacije, nn.arhitektura, nn.layerCount + 1);
	stuc__nnBackprop(nn, gdMap, tInput, tOutput, boost);

	return gdMap;
}

void stuc_nnBackpropNoAlloc(Stuc_nn nn, Stuc_nn gdMap, Stuc_mat tInput, Stuc_mat tOutput, float_t boost) {
	stuc__nnBackprop(nn, gdMap, tInput, tOutput, boost);

	return;
}


void stuc__calculateBackpropForLayerActivation(Stuc_nn nn, Stuc_nn gdMap, size_t layer, size_t act, float_t boost) {
	// layer -> current layer index
	// act -> current activation index

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
	}
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
			STUC_MAT_AT(STUC_NN_OUTPUT(gdMap), 0, i)
				= boost *  (STUC_MAT_AT(STUC_NN_OUTPUT(nn), 0, i)
					  - STUC_MAT_AT(tOutput, sample, i));
		}

		// layer - current
		for (size_t layer = nn.layerCount; layer > 0; layer--) {
			// activation - current
			for (size_t act = 0; act < STUC_NN_AT(nn, layer).a.cols; act++) {
				stuc__calculateBackpropForLayerActivation(nn, gdMap, layer, act, boost);
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
