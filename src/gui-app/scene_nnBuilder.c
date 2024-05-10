#include "scene_nnBuilder.h"
#include "dynamic-arrays.h"
#include "external/raygui.h"
#include "external/raylib.h"
#include "scene_essentials.h"

#define MAX_N_OF_NEURONS       1000000
#define MAX_N_OF_LAYERS        1000000
struct ErrorPopups g_error_stack = {0};

Scene_nnBuilder scene_nnBuilderInit(void) {
	Scene_nnBuilder s;
	s.leftPad   = 24;
	s.rightPad  = 24;
	s.topPad    = 48;
	s.bottomPad = 24;
	s.layerPad  = 8;
	s.innerLayerPad = 16;

	s.costPanelGroup = initCostFunctionPanelGroup();
	s.neuralNetworkPreview = initNeuralNetworkPreview();
	s.checkResultGroup = initCheckResultGroup();
	s.controlPanelGroup = initControlPanelGroup();

	return s;
}

void scene_nnBuilderUpdate(Scene_nnBuilder *s) {
	size_t leftPad   = s->leftPad;
	size_t rightPad  = s->rightPad;
	size_t topPad    = s->topPad;
	size_t bottomPad = s->bottomPad;
	size_t layerPad  = s->layerPad;
	size_t innerLayerPad = s->innerLayerPad;

	const size_t layer1_min = 176;

	s->aX.l1 = leftPad; s->aX.l2 = leftPad;
	s->aY.s1 =  topPad; s->aY.s2 =  topPad;

	if (s->costPanelGroup.isShown) {
		s->costPanelGroup.boundingBox = (Rectangle) { s->aX.l1, s->aY.s1, 0.55 * g_screenWidth, fmax(0.25 * g_screenHeight, layer1_min) }; // GroupBox: costFunction
		s->aY.s1 += s->costPanelGroup.boundingBox.height + innerLayerPad;
		s->aX.l1 += s->costPanelGroup.boundingBox.width + layerPad;
	} else {
		s->aY.s1 += s->controlPanelGroup.boundingBox.height + innerLayerPad;
	}
	
	{
		size_t width = g_screenWidth - (s->aX.l1 + rightPad);
		size_t height= fmax(0.25 * g_screenHeight, layer1_min);
		s->controlPanelGroup.boundingBox = (Rectangle) { s->aX.l1, s->aY.s2, width, height }; // GroupBox: controlPanel
	}

	if (!s->neuralNetworkPreview.training) {
		s->neuralNetworkPreview.boundingBox = (Rectangle) { s->aX.l2, fmax(s->aY.s1, s->aY.s2), g_screenWidth - rightPad - leftPad, g_screenHeight - (s->aY.s1 + bottomPad) }; //DummyRec: neuralNetwork
	} else {
		s->neuralNetworkPreview.boundingBox = (Rectangle) { s->aX.l2, s->aY.s1, 0.55 * g_screenWidth, g_screenHeight - (s->aY.s1 + bottomPad) }; //DummyRec: neuralNetwork
		s->aY.s1 += s->neuralNetworkPreview.boundingBox.height + innerLayerPad;
	}
	s->aX.l2 += s->neuralNetworkPreview.boundingBox.width + layerPad;

	s->aY.s2 += s->controlPanelGroup.boundingBox.height + innerLayerPad;
	
	if (s->neuralNetworkPreview.training) {
		s->checkResultGroup.boundingBox = (Rectangle) { s->aX.l2, s->aY.s2, g_screenWidth - (s->aX.l2 + rightPad), g_screenHeight - (s->aY.s2 + bottomPad) }; //GroupBox: provjeraRezultata
	}


	updateControlPanelGroup(&s->controlPanelGroup, layerPad);
	updateCostFunctionPanelGroup(&s->costPanelGroup, &s->neuralNetworkPreview, topPad, innerLayerPad);
	updateNeuralNetworkPreview(&s->neuralNetworkPreview, &s->controlPanelGroup);

	if (s->neuralNetworkPreview.training) {
		updateCheckResultGroup(&s->checkResultGroup, layerPad, innerLayerPad);
	}

	if (IsKeyPressed(KEY_L)) {
		log(INFO, "Layer count is: %zu\n", s->controlPanelGroup.layers.count);
		size_t layerCount = s->controlPanelGroup.layers.count;
		size_t neuronSize = 10;
		// size_t nBottmPad = 3 * neuronSize;
		size_t nRightPad = 4 * neuronSize;
		float nnPreviewWidth = (layerCount+1)*neuronSize + (layerCount-1)*(nRightPad);
		NeuralNetworkPreview* nnp = &s->neuralNetworkPreview;
		size_t dx = /*nnp->boundingBox.x +*/ nnp->boundingBox.width/2 - nnPreviewWidth/2;

		log(INFO, "bounding box; width: %.0f, height: %.0f\n", nnp->boundingBox.width, nnp->boundingBox.height);
		log(INFO, "nnPreview; width: %.0f, dx: %zu\n", nnPreviewWidth, dx);
	}

	if (IsKeyPressed(KEY_R)) {
		if (!s->neuralNetworkPreview.training) {
			s->controlPanelGroup.layers.count = 0;
			s->controlPanelGroup.layerSelectedCurrent = 0;
			s->controlPanelGroup.removeCurrLayer = true;
		}
	}

}
void scene_nnBuilderDraw(Scene_nnBuilder *s) {
	// Draw controls
	drawNeuralNetworkPreview(&s->neuralNetworkPreview, &s->controlPanelGroup);
	drawCostFunctionPanelGroup(&s->costPanelGroup);
	if (s->neuralNetworkPreview.training) {
		drawCheckResultGroup(&s->checkResultGroup);
	}
	drawControlPanelGroup(&s->controlPanelGroup, &s->neuralNetworkPreview);

	drawErrorPopup();
}

void drawErrorPopup(void) {
	if (g_error_stack.count > 0) {
		g_error_stack.error_shown = true;
		ErrorPopup *error = &g_error_stack.items[0];

		size_t font_size = 40;
		Vector2 txt_size = MeasureTextEx(GuiGetFont(), error->text, font_size, 1);

		size_t padding = 20;
		size_t border_width = 5;
		float inner_box_x = g_screenWidth/2.0 - txt_size.x/2;
		float inner_box_y = g_screenHeight/2.0 - txt_size.y/2;
		Rectangle  popup_rect = {inner_box_x - padding, inner_box_y - padding, txt_size.x + 2*padding, txt_size.y + 2*padding};
		Rectangle  popup_border = {popup_rect.x + border_width, popup_rect.y + border_width, popup_rect.width - 2*border_width, popup_rect.height - 2*border_width};

		if (error->initTimeSec == 0) error->initTimeSec = GetTime();

		switch (error->opt) {
		case EOPT_OK: assert(0 && "Not implemented!"); break;
		case EOPT_YES_NO: assert(0 && "Not implemented!"); break;
		case EOPT_OK_CANCLE: assert(0 && "Not implemented!"); break;
		case EOPT_TIME: {
			DrawRectangleRounded(popup_rect, 0.3, 15, WHITE);
			DrawRectangleRoundedLines(popup_border, 0.3, 15, border_width/2.0, GRAY);
			DrawTextEx(GuiGetFont(), error->text, (Vector2){inner_box_x, inner_box_y}, font_size, 1, error->error_level == ELEVEL_ERROR ? MAROON : ORANGE);
			if (GetTime() - error->initTimeSec > error->waitTimeSec) {
				g_error_stack.error_shown = false;
				log(INFO, "REMOVED Error: %s.\n", error->text);
				free(error->text);
				da_remove(&g_error_stack, 0);
			}
		} break;
		}
	}
}

void push_time_error(const char *str, double time_sec) {
	log(ERROR, "%s\n", str);
	char *dup_str = calloc(256, 1);
	dup_str = memcpy(dup_str, str, 256);
	ErrorPopup error = {0};
	error.opt = EOPT_TIME;
	error.text = dup_str;
	error.waitTimeSec = time_sec;
	error.error_level = ELEVEL_ERROR;
	da_append(&g_error_stack, error);
}

void push_time_warn(const char *str, double time_sec)  {
	log(WARN, "%s\n", str);
	char *dup_str = calloc(256, 1);
	dup_str = memcpy(dup_str, str, 256);
	ErrorPopup error = {0};
	error.opt = EOPT_TIME;
	error.text = dup_str;
	error.waitTimeSec = time_sec;
	error.error_level = ELEVEL_WARN;
	da_append(&g_error_stack, error);
}



ControlPanelGroup initControlPanelGroup(void) {

	ControlPanelGroup cpg = {
		.controlPanelText               = "Kontrole",                    // GROUPBOX:    controlPanel
		.layerChoiceText                = "ONE;TWO;THREE;FOUR;FIVE;SIX", // TOGGLEGROUP: layerChoice
		.activationFunctionChoiceText   = "SIGMOID;RE:LU;SOFTMAX;TANH;SIN",      // COMBOBOX:    activationFunctionChoice
		.learnRateText                  = "Learn rate",                  // SLIDERBAR:   learnRate
		.layerLabelText                 = "Layer",                       // LABEL:       layerLabel
		.activationLabelText            = "Activation",                  // LABEL:       activationLabel
		.numberOfNeuronsText            = "Number of N ",                 // SPINNER:     numberOfNeurons
		.trainingToggleText             = "Start training ",                 // TOGGLE:     trainingToggleText
	};
	assert(STUC_LENP(cpg.activationLookup) == 5);
	cpg.activationLookup[0] = STUC_ACTIVATE_SIGMOID;
	cpg.activationLookup[1] = STUC_ACTIVATE_RELU;
	cpg.activationLookup[2] = STUC_ACTIVATE_SOFTMAX;
	cpg.activationLookup[3] = STUC_ACTIVATE_TANH;
	cpg.activationLookup[4] = STUC_ACTIVATE_SIN;
	
	cpg.layerChoiceCurrent              = 0;            // ToggleGroup: layerChoice
	cpg.activationFunctionChoiceCurrent = 0;            // ComboBox:    activationChoice
	cpg.nOfNeuronsEditMode              = false;        // Spinner:     nOfNeurons
	cpg.layerChoiceEditMode             = false;        // GuiLayerSelector
	cpg.nOfNeuronsValue                 = 0;            // Spinner:     nOfNeurons
	cpg.learnRateValue                  = 0.0f;         // SliderBar:   learnRate
	
	return cpg;
}

void updateControlPanelGroup(ControlPanelGroup *cpg, size_t layerPad) {
	// Određivanje dinamičkih pozicija elemenata u grupi
	size_t leftPad = 11*layerPad;
	size_t rightPad = 2*layerPad;

	size_t sX = cpg->boundingBox.x + leftPad;
	size_t controlsWidth = cpg->boundingBox.width - (leftPad + rightPad);
	size_t btn = cpg->layerChoiceTG.height;
	
	size_t sY = 64;
	size_t spacing = 32;
	cpg->layerL 		 = (Rectangle){ sX - 42,  sY,  40,  24 };			    // Label: layerLabel
	cpg->activationL 	 = (Rectangle){ sX - 65,  sY + spacing, 56, 24 };		    // Label: activationLabel
	cpg->layerChoiceTG  	 = (Rectangle){ sX, sY, controlsWidth - btn - rightPad, 24 };	    // ToggleGroup: layerChoice
	
	sX +=  controlsWidth - btn;
		cpg->removeLayerBT = (Rectangle){ sX, sY, btn, btn };  // Remove layer button
	sX -=  controlsWidth - btn;
	
	sY += spacing;
	cpg->activationChoiceCB  = (Rectangle){ sX, sY, controlsWidth,       24 }; sY += spacing; // ComboBox: activationFunctionChoice
	cpg->nOfNeuronsS  	 = (Rectangle){ sX, sY, controlsWidth,       24 }; sY += spacing; // Spinner: numberOfNeurons
	cpg->learnRateSB  	 = (Rectangle){ sX, sY, controlsWidth,       16 }; sY += spacing; // SliderBar: learnRate
	cpg->toggleTrainBT  	 = (Rectangle){ sX, sY, controlsWidth,       16 }; sY += spacing; // Toggle: start nn training process

	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && CheckCollisionPointRec(GetMousePosition(), cpg->activationChoiceCB)) {
		cpg->activationFunctionChoiceCurrent = ((cpg->activationFunctionChoiceCurrent + STUC_ACTIVATIONS_COUNT) - 1) % STUC_ACTIVATIONS_COUNT;
	}

	if (cpg->nOfNeuronsValue > MAX_N_OF_NEURONS) cpg->nOfNeuronsValue = MAX_N_OF_NEURONS;
	if (cpg->layerChoiceCurrent > MAX_N_OF_LAYERS) cpg->layerChoiceCurrent = MAX_N_OF_LAYERS;

	if (cpg->layerChoiceCurrent > (int)cpg->layers.count) {
		if (cpg->layerChoiceCurrent - (int)cpg->layers.count < 5) {
			da_append(&cpg->layers, (Gui_nnLayer){0});
			log(INFO, "Dodan layer %zu!\n", cpg->layers.count);
		} else {
			da_append_many(&cpg->layers, NULL, cpg->layerChoiceCurrent - (int)cpg->layers.count);
		}
	} else if (cpg->layers.count > 0){
		if (cpg->layerChoiceCurrent == cpg->layerSelectedCurrent + 1) {
			cpg->layers.items[cpg->layerSelectedCurrent].nOfNeurons = cpg->nOfNeuronsValue;
			cpg->layers.items[cpg->layerSelectedCurrent].activation = cpg->activationFunctionChoiceCurrent;
		} else {
			cpg->layerSelectedCurrent = cpg->layerChoiceCurrent - 1;
			cpg->nOfNeuronsValue = cpg->layers.items[cpg->layerSelectedCurrent].nOfNeurons;
			cpg->activationFunctionChoiceCurrent = cpg->layers.items[cpg->layerSelectedCurrent].activation;
		}
	}
		
	return;
}

void drawControlPanelGroup(ControlPanelGroup *cpg, NeuralNetworkPreview *nnp) {

	GuiGroupBox(    cpg->boundingBox,	 cpg->controlPanelText);
	
	if (GuiSpinner( cpg->layerChoiceTG, NULL, 
			     &cpg->layerChoiceCurrent, 1, MAX_N_OF_LAYERS, cpg->layerChoiceEditMode)) {
		cpg->layerChoiceEditMode = !cpg->layerChoiceEditMode;
	} 
	if (GuiButton(cpg->removeLayerBT, ICON_TO_TEXT(ICON_BIN))) {
		cpg->removeCurrLayer = true;
	}

	GuiComboBox(    cpg->activationChoiceCB, cpg->activationFunctionChoiceText, &cpg->activationFunctionChoiceCurrent);
	if (GuiSpinner( cpg->nOfNeuronsS,	 cpg->numberOfNeuronsText, 
			&cpg->nOfNeuronsValue, 0, MAX_N_OF_NEURONS, cpg->nOfNeuronsEditMode)) 
			cpg->nOfNeuronsEditMode = !cpg->nOfNeuronsEditMode;
	GuiSliderBar(   cpg->learnRateSB,	 cpg->learnRateText, NULL, &cpg->learnRateValue, 0, 100);
	GuiLabel(       cpg->layerL,		 cpg->layerLabelText);
	GuiLabel(       cpg->activationL,	 cpg->activationLabelText);
	GuiToggle(cpg->toggleTrainBT, cpg->trainingToggleText, &nnp->training);
}


CostFunctionPanelGroup initCostFunctionPanelGroup(void) {
	CostFunctionPanelGroup cfpg = {
		.costFunctionText = "Graf tocnosti meze",
		.costExpandButtonText = "COST"
	};

	cfpg.isShown = false;


	return cfpg;
}

void drawCostFunctionPanelGroup(CostFunctionPanelGroup *cfpg) {

	if (cfpg->isShown) { 
		Rectangle costFunctionRec = cfpg->boundingBox;
		GuiGroupBox(costFunctionRec, cfpg->costFunctionText); 

		size_t exitButtonWidth = 16;
		Rectangle exitButtonRec = { 
					costFunctionRec.x + costFunctionRec.width - exitButtonWidth, 
					costFunctionRec.y, 
					exitButtonWidth, 
					exitButtonWidth
				       };

		cfpg->isShown = !GuiButton(exitButtonRec, GuiIconText(ICON_CROSS, NULL));
	} else {
		Rectangle buttonPosition = cfpg->expandButtonRec;
		float textSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
		Vector2 textMeasure = MeasureTextEx(GuiGetFont(), cfpg->costExpandButtonText, textSize, 1);
		Color textColor = SC_NORML_TEXT;

		Vector2   textPosition   = {
				  buttonPosition.width - (buttonPosition.width - textMeasure.y) / 2, 
				  buttonPosition.y + buttonPosition.height / 2 - textMeasure.x / 2 
				};
		Vector2   rotationPoint  = { 0, 0 };

		cfpg->isShown = GuiButton(buttonPosition, NULL );

		if (CheckCollisionPointRec(GetMousePosition(), buttonPosition)) {
			textColor = SC_FOCUS_TEXT;
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
				textColor = SC_PRESS_TEXT;
		}

		DrawTextPro(GuiGetFont(), cfpg->costExpandButtonText, textPosition, rotationPoint, 90, textSize, 1, textColor);
	}
}
void updateCostFunctionPanelGroup(CostFunctionPanelGroup *cfpg, NeuralNetworkPreview *nnp, size_t topPad, size_t expandButtonWidth) {
	cfpg->expandButtonRec = (Rectangle) { 0, topPad, expandButtonWidth, 0.1 * g_screenHeight };

	if (nnp->training) {
		if (!nnp->prepared) cfpg->isShown = true;
	}
}

NeuralNetworkPreview initNeuralNetworkPreview(void) {
	NeuralNetworkPreview nnp = {
		.dummyRecText = "Prikaz Neuronske mreze\n\n"
		"strelice za dodavanje slojeva i neurona, \n"
		"INS i DEL za umetanje i brisanje sloja, \n"
		"R za resetiranje mreze"// DUMMYREC:    neuralNetwork
	};

	nnp.training                        = false;        // Training_mode:   training
	nnp.prepared                        = false;        // Training_mode:   prepraring for training
	nnp.nn = malloc(sizeof(Stuc_nn));
	return nnp;
}

void updateNeuralNetworkPreview(NeuralNetworkPreview *nnp, ControlPanelGroup *cpg) {
	// nnp->boundingBox.width = 0.55 * g_screenWidth;
	if (!nnp->training) {
		if (nnp->prepared) {
			stuc_nnFree(*nnp->nn);
			nnp->prepared = false;
			log(INFO, "NN dealloced!\n");
		}

		if (IsKeyPressed(KEY_LEFT)) {
			if (cpg->layerSelectedCurrent > 0) {
				cpg->layerChoiceCurrent--;
			}
		}
		
		if (IsKeyPressed(KEY_RIGHT)) {
			if ((size_t)cpg->layerSelectedCurrent < MAX_N_OF_LAYERS) {
				cpg->layerChoiceCurrent++;
			} else {
				push_time_warn("Ne moze se dodati vise slojeva!", 1);
				// log(WARN, "Can not add another layer!\n");
			}
		}

		if (IsKeyPressed(KEY_UP)) {
			if ((size_t)cpg->nOfNeuronsValue < MAX_N_OF_NEURONS) {
				cpg->nOfNeuronsValue++;
			} else {
				push_time_warn(TextFormat("Ne moze se dodati vise neurona!"), 1);
				// log(WARN, "Can not add another neuron!\n");
			}
		}

		if (IsKeyPressed(KEY_DOWN)) {
			if (cpg->nOfNeuronsValue > 0) {
				cpg->nOfNeuronsValue--;
			} else {
				cpg->removeCurrLayer = true;
			}
		}

		if (IsKeyPressed(KEY_DELETE) || cpg->removeCurrLayer) {
			cpg->removeCurrLayer = false;
			da_remove(&cpg->layers, cpg->layerSelectedCurrent);
			log(INFO, "Izbrisan layer %d!\n", cpg->layerSelectedCurrent);
			cpg->layerSelectedCurrent--;
			cpg->layerChoiceCurrent = cpg->layerSelectedCurrent + 1;

			cpg->nOfNeuronsValue = cpg->layers.items[cpg->layerSelectedCurrent].nOfNeurons;
			cpg->activationFunctionChoiceCurrent = cpg->layers.items[cpg->layerSelectedCurrent].activation;
		}

		if (IsKeyPressed(KEY_INSERT)) {
			da_insert(&cpg->layers, cpg->layerSelectedCurrent, (Gui_nnLayer){0});
			log(INFO, "Umetnut layer %d!\n", cpg->layerSelectedCurrent);
		}

		return;
	}

	if (!nnp->prepared) {
		size_t *arch = calloc(cpg->layers.count, sizeof(size_t));
		Stuc_activationFunction *act = calloc(cpg->layers.count - 1, sizeof(Stuc_activationFunction));

		arch[0] = cpg->layers.items[0].nOfNeurons;
		if (arch[0] == 0) {
			push_time_error("Sloj 1 nema neurona!", 2);
			nnp->training = false;
			return;
		}

		for (size_t i = 1; i < cpg->layers.count; i++) {
			arch[i] = cpg->layers.items[i].nOfNeurons;			
			act[i-1] = cpg->layers.items[i].activation;			


			if (arch[i] == 0) {
				push_time_error(TextFormat("Sloj %zu nema neurona!\n", i+1), 2);
				nnp->training = false;
				return;
			}
		}

		*nnp->nn = stuc_nnAlloc(act, arch, cpg->layers.count);

		free(arch);
		free(act);

		log(INFO, "NN Alloced!\n");

		nnp->prepared = true;
	}

	return;
}
void drawNeuralNetworkPreview(NeuralNetworkPreview *nnp, ControlPanelGroup *cpg) {
	GuiDummyRec(nnp->boundingBox,  nnp->dummyRecText);

	size_t max_neurons = 0;
	for (size_t layer = 0; layer < cpg->layers.count; layer++) {
		size_t curr_neurons = cpg->layers.items[layer].nOfNeurons;
		max_neurons = curr_neurons > max_neurons ? curr_neurons : max_neurons;
	}

	if (max_neurons == 0) return;
	
	size_t layerCount = cpg->layers.count;
	float neuronSize = 50;
	float nBottmPad = 3 * neuronSize;
	float nRightPad = 4 * neuronSize;
	float neuronMapInnerWidth = neuronSize*(layerCount - 1) + nRightPad*(layerCount - 1);
	float neuronMapInnerHeight = neuronSize*max_neurons + nBottmPad*(max_neurons - 1);
	/*
	 * This is the width of the neuron map calculated from the circle centers instead of 
	 * boxes arround them. The real width is this width + 2*neuronSize
	 */

	for (size_t i = 0; i < 1000; i++) {
		neuronMapInnerWidth = neuronSize*(layerCount - 1) + nRightPad*(layerCount - 1);
		size_t width = neuronMapInnerWidth + 2*neuronSize + nRightPad;

		neuronMapInnerHeight = neuronSize*max_neurons + nBottmPad*max_neurons;
		size_t height = neuronMapInnerHeight + 2*neuronSize + nBottmPad;

		nBottmPad = 3 * neuronSize;
		nRightPad = 4 * neuronSize;

		if (neuronSize < 0) {
			return;
		}

		if (width > nnp->boundingBox.width) neuronSize -= 0.15;
		if (height > nnp->boundingBox.height) neuronSize -= 0.15;
	}


	float dx = nnp->boundingBox.x + nnp->boundingBox.width/2 - (neuronMapInnerWidth)/2;

	if ((layerCount > 0 && cpg->layers.items[0].nOfNeurons >= 1) || layerCount > 1) {
		DrawRectangleRec(nnp->boundingBox, SC_NORML_BASE);

		size_t focusBar_originX = dx + cpg->layerSelectedCurrent*(neuronSize + nRightPad) - neuronSize - nRightPad/2; 
		size_t focusBar_originY = nnp->boundingBox.y; 
		size_t focusBar_width   = 2*neuronSize + nRightPad; 
		size_t focusBar_height  = nnp->boundingBox.height; 
		DrawRectangle(focusBar_originX, focusBar_originY, focusBar_width, focusBar_height, SC_FOCUS_BASE);
	}


	for (size_t i = 0; i < layerCount; i++) {
		size_t nOfNeurons = cpg->layers.items[i].nOfNeurons;
		float layerInnerHeight = neuronSize*(nOfNeurons - 1) + nBottmPad*(nOfNeurons - 1);
		float dy = nnp->boundingBox.y + nnp->boundingBox.height/2 - (layerInnerHeight)/2;

		for (size_t j = 0; j < nOfNeurons; j++) {
			DrawCircle(dx, dy, neuronSize, SC_PRESS_BORDER);

			if (i < layerCount - 1) {
				size_t nextLayerNOfN = cpg->layers.items[i+1].nOfNeurons;
				float nextLIHeight = neuronSize*(nextLayerNOfN - 1) + nBottmPad*(nextLayerNOfN - 1);
				float y = nnp->boundingBox.y + nnp->boundingBox.height/2 - (nextLIHeight)/2;
				for (size_t k = 0; k < nextLayerNOfN; k++) {
					float x = dx + nRightPad + neuronSize;
					DrawLine(dx, dy, x, y, SC_PRESS_BORDER);
					y += nBottmPad + neuronSize;
				}
			}


			dy += nBottmPad + neuronSize;
		}

		dx += nRightPad + neuronSize;
	}

	return;
}

CheckResultGroup initCheckResultGroup(void) {
	CheckResultGroup crg = {
		.provjeraRezultataText           = "Provjera rezultata"         // GROUPBOX:    provjeraRezultata
	};

	// Initilize controls variables
	crg.nacinProvjereRezultataActive = 0;            // ToggleGroup: nacinProvjereRezultata

	return crg;
}
void updateCheckResultGroup(CheckResultGroup *crg, size_t layerPad, size_t innerLayerPad) {
	size_t sX = crg->boundingBox.x + 2*layerPad;
	size_t sY = crg->boundingBox.y + innerLayerPad;
	crg->resultsRect = (Rectangle) { sX, sY,  72,  24 }; // ToggleGroup: nacinProvjereRezultata
}
void drawCheckResultGroup(CheckResultGroup *crg) {
	GuiGroupBox(crg->boundingBox,  crg->provjeraRezultataText);
	GuiToggleGroup( crg->resultsRect,  
		TextFormat("%sTEKST;%sSLIKA", 
			ICON_TO_TEXT(ICON_FILETYPE_TEXT), 
			ICON_TO_TEXT(ICON_FILETYPE_IMAGE)), 
		&crg->nacinProvjereRezultataActive);
}
