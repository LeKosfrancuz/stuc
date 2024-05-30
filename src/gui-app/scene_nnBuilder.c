#include "scene_nnBuilder.h"
#include "dynamic-arrays.h"
#include "external/raygui.h"
#include "external/raylib.h"
#include "external/raymath.h"
#include "scene_essentials.h"

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


	updateControlPanelGroup(&s->controlPanelGroup, &s->neuralNetworkPreview, layerPad);
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
		log(INFO, "Input data has %zu samples.\n", tData_samples);
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
	if (g_error_stack.error_shown) GuiLock();
	else GuiUnlock();

	drawNeuralNetworkPreview(&s->neuralNetworkPreview, &s->controlPanelGroup);
	drawCostFunctionPanelGroup(&s->costPanelGroup);
	if (s->neuralNetworkPreview.training) {
		drawCheckResultGroup(&s->checkResultGroup, &s->neuralNetworkPreview);
	}
	drawControlPanelGroup(&s->controlPanelGroup, &s->neuralNetworkPreview, &s->costPanelGroup);

	drawErrorPopup();
}

void drawErrorPopup(void) {
	if (g_error_stack.count > 0) {
		g_error_stack.error_shown = true;
		ErrorPopup *error = &g_error_stack.items[0];
		GuiUnlock();

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
		case EOPT_YES_NO: {
			int ret = GuiMessageBox(
				(Rectangle){
					g_screenWidth/2.0 - 0.15*g_screenWidth,
					g_screenHeight/2.0 - 0.10*g_screenHeight,
					0.30*g_screenWidth,
					0.20*g_screenHeight
				},
				"Do you wish to abort?", error->text, "YES;NO"
			);
			if (ret == 1) { // "YES"
				log(INFO, "Error resolved!\n");
				error->resolve();
			}
			if (ret != -1) {
				g_error_stack.error_shown = false;
				log(INFO, "REMOVED Error: %s.\n", error->text);
				free(error->text);
				da_remove(&g_error_stack, 0);
			}
		} break;
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

void push_yesno_warn(const char *str, void (*resolve)(void))  {
	log(WARN, "%s\n", str);
	char *dup_str = calloc(256, 1);
	dup_str = memcpy(dup_str, str, 256);
	ErrorPopup error = {0};
	error.opt = EOPT_YES_NO;
	error.text = dup_str;
	error.resolve = resolve;
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
		.trainingToggleText             = "Training Mode",                 // TOGGLE:     trainingToggleText
		.startTrainToggleText           = "Start Training",                 // TOGGLE:     trainingToggleText
		.stopTrainToggleText            = "Stop Training",                 // TOGGLE:     trainingToggleText
		.batchSizeText                  = "Batch Size ",                 // TOGGLE:     trainingToggleText
		.epochCountText                 = "Gen Count ",                 // TOGGLE:     trainingToggleText
		.saveToFileText                 = "Save To File",                 // TOGGLE:     trainingToggleText
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
	cpg.learnRateValue                  = 0.5f;         // SliderBar:   learnRate

	return cpg;
}

void updateControlPanelGroup(ControlPanelGroup *cpg, NeuralNetworkPreview *nnp, size_t layerPad) {
	// Određivanje dinamičkih pozicija elemenata u grupi
	size_t leftPad = 11*layerPad;
	size_t rightPad = 2*layerPad;

	size_t sX = cpg->boundingBox.x + leftPad;
	size_t controlsWidth = cpg->boundingBox.width - (leftPad + rightPad);
	size_t btn = cpg->layerChoiceTG.height;

	size_t sY = 64;
	size_t spacing = 32;
	if (!nnp->learning_enabled){
		cpg->layerL 		 = (Rectangle){ sX - 42,  sY,  40,  24 };			    // Label: layerLabel
		cpg->activationL 	 = (Rectangle){ sX - 65,  sY + spacing, 56, 24 };		    // Label: activationLabel
		cpg->layerChoiceTG  	 = (Rectangle){ sX, sY, controlsWidth - btn - rightPad, 24 };	    // ToggleGroup: layerChoice

		sX +=  controlsWidth - btn;
			cpg->removeLayerBT = (Rectangle){ sX, sY, btn, btn };  // Remove layer button
		sX -=  controlsWidth - btn;

		sY += spacing;
		cpg->activationChoiceCB  = (Rectangle){ sX, sY, controlsWidth,       24 }; sY += spacing; // ComboBox: activationFunctionChoice
		cpg->nOfNeuronsS  	 = (Rectangle){ sX, sY, controlsWidth,       24 }; sY += spacing; // Spinner: numberOfNeurons
	}
	cpg->learnRateSB  	 = (Rectangle){ sX, sY, controlsWidth,       16 }; sY += spacing; // SliderBar: learnRate
	cpg->toggleTrainBT  	 = (Rectangle){ sX, sY, controlsWidth,       16 }; // Toggle: start nn training process
	cpg->clearGraphBT	 = (Rectangle){ sX - rightPad - btn, sY, btn, 16}; sY += spacing; // Button: clear graph

	if (nnp->learning_enabled) {
		cpg->startStopTrainBT = (Rectangle){ sX, sY, controlsWidth,       16 }; sY += spacing; // Toggle: enable7disable training process
		cpg->batchSizeS = (Rectangle){ sX, sY, controlsWidth,       16 }; sY += spacing; // Spinner: size of training data in batch
		cpg->epochCountL = (Rectangle){ sX, sY, controlsWidth,       16 }; // Spinner: size of training data in batch
		Vector2 epochTxt_size = MeasureTextEx(GuiGetFont(), cpg->epochCountText, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING));
		cpg->epochCountLL = (Rectangle){ sX - epochTxt_size.x - rightPad, sY, epochTxt_size.x, 16 }; sY += spacing; // Spinner: size of training data in batch
		cpg->saveToFileBT = (Rectangle){ sX, sY, controlsWidth, 16 }; sY += spacing; // Spinner: size of training data in batch
	}

	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && CheckCollisionPointRec(GetMousePosition(), cpg->activationChoiceCB)) {
		cpg->activationFunctionChoiceCurrent = ((cpg->activationFunctionChoiceCurrent + STUC_ACTIVATIONS_COUNT) - 1) % STUC_ACTIVATIONS_COUNT;
	}

	if (cpg->nOfNeuronsValue > MAX_N_OF_NEURONS) cpg->nOfNeuronsValue = MAX_N_OF_NEURONS;
	if (cpg->layerChoiceCurrent > MAX_N_OF_LAYERS) cpg->layerChoiceCurrent = MAX_N_OF_LAYERS;
	if (nnp->batch_size > MAX_BATCH_SIZE) nnp->batch_size = MAX_BATCH_SIZE;

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

void drawControlPanelGroup(ControlPanelGroup *cpg, NeuralNetworkPreview *nnp, CostFunctionPanelGroup *cfpg) {

	GuiGroupBox(    cpg->boundingBox,	 cpg->controlPanelText);

	if (!nnp->learning_enabled) {
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
		GuiLabel(       cpg->layerL,		 cpg->layerLabelText);
		GuiLabel(       cpg->activationL,	 cpg->activationLabelText);
	} else {
		nnp->learning_paused = !nnp->learning_paused;
		if (!nnp->learning_paused) {
			GuiToggle(cpg->startStopTrainBT, cpg->startTrainToggleText, &nnp->learning_paused);
		} else {
			GuiToggle(cpg->startStopTrainBT, cpg->stopTrainToggleText, &nnp->learning_paused);
		}
		nnp->learning_paused = !nnp->learning_paused;

		if (GuiSpinner(cpg->batchSizeS, cpg->batchSizeText, &nnp->batch_size, 4, MAX_BATCH_SIZE, cpg->batchSizeEditMode)) {
			cpg->batchSizeEditMode = !cpg->batchSizeEditMode;
		}

		GuiLabel(cpg->epochCountL, TextFormat("%zu", nnp->epoch_count));
		GuiLabel(cpg->epochCountLL, cpg->epochCountText);
		if ((cpg->boundingBox.y + cpg->boundingBox.height) - (cpg->saveToFileBT.y + cpg->saveToFileBT.height) > 0) {
			if (GuiButton(cpg->saveToFileBT, cpg->saveToFileText)) {
				uint8_t ret = stuc_nnSaveToFile(*nnp->nn, RESOURCES_PATH"saved_nn.snn");
				if (ret) {
					stuc_printIOFlags(ret);
					return;
				}
				push_time_warn("NN saved to file, succesfully", 2);
			}
		}
	}

	if (CheckCollisionPointRec(GetMousePosition(), cpg->learnRateSB)) SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
	else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

	GuiSliderBar(   cpg->learnRateSB,	 cpg->learnRateText, NULL, &cpg->learnRateValue, 0, 100);
	GuiToggle(cpg->toggleTrainBT, cpg->trainingToggleText, &nnp->training);

	if(GuiButton(cpg->clearGraphBT, ICON_TO_TEXT(ICON_FILE_NEW))) cfpg->clearGraph = true;
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

		float_t max_cost = 0;
		for (size_t i = 0; i < cfpg->graph.count; i++) {
			max_cost = fmax(cfpg->graph.items[i], max_cost);
		}

		if (max_cost == 0 || cfpg->graph.count <= 1) return;


		for (size_t i = 0; i < cfpg->graph.count - 1; i++) {
			float dx = cfpg->graphRect.width/cfpg->graph.count;
			float curr_x = cfpg->graphRect.x;
			float curr_y = cfpg->graphRect.y;
			size_t thickness = 5;

			Vector2 curr_point = { .x=curr_x + i*dx, .y=Remap(cfpg->graph.items[i], max_cost, 0, curr_y, curr_y + cfpg->graphRect.height)};
			Vector2 next_point = { .x=curr_x + (i+1)*dx, .y=Remap(cfpg->graph.items[i+1], max_cost, 0, curr_y, curr_y + cfpg->graphRect.height)};
			// log(INFO, "curr_point = %.2f, %.2f\n", curr_point.x, curr_point.y);
			DrawLineEx(curr_point, next_point, thickness, SC_PRESS_TEXT);
		}
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
	size_t innerPad = 10;
	cfpg->graphRect = (Rectangle) {cfpg->boundingBox.x + innerPad, cfpg->boundingBox.y + innerPad, cfpg->boundingBox.width - 2*innerPad, cfpg->boundingBox.height - 2*innerPad};

	if (nnp->training) {
		if (!nnp->prepared) cfpg->isShown = true;
		if (nnp->prepared && nnp->learning_enabled && !nnp->learning_paused) {
			da_append(&cfpg->graph, nnp->cost);
			log(INFO, "cost = %f\n", nnp->cost);
		}
	}

	if (cfpg->clearGraph) {
		cfpg->graph.count = 0;
		cfpg->clearGraph = false;
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
	nnp.gd_map = malloc(sizeof(Stuc_nn));
	nnp.batch_size = 32;
	return nnp;
}

int g_resolved = -1;
void setResolve(void) { g_resolved = true; }
void updateNeuralNetworkPreview(NeuralNetworkPreview *nnp, ControlPanelGroup *cpg) {
	// nnp->boundingBox.width = 0.55 * g_screenWidth;
	if (!nnp->training) {
		if (nnp->prepared) {
			stuc_nnFree(*nnp->nn);
			stuc_nnFree(*nnp->gd_map);
			nnp->prepared = false;
			nnp->learning_enabled = false;
			nnp->learning_paused = false;
			nnp->epoch_count = 0;
			g_resolved = -1;
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

		if (tData_samples == 0 && g_resolved == -1 && g_error_stack.count == 0) {
			g_resolved = false;
			push_yesno_warn("Training Data is not defined!", setResolve);
		} else if (tData_samples > 0) g_resolved = false;

		if (g_resolved) {
			nnp->training = false;
			g_resolved = -1;
			return;
		} else if (g_error_stack.count > 0) { return; };

		g_resolved = true;


		for (size_t i = 1; i < cpg->layers.count; i++) {
			arch[i] = cpg->layers.items[i].nOfNeurons;
			act[i-1] = cpg->activationLookup[cpg->layers.items[i-1].activation];


			if (arch[i] == 0) {
				push_time_error(TextFormat("Sloj %zu nema neurona!\n", i+1), 2);
				nnp->training = false;
				return;
			}
		}

		*nnp->nn = stuc_nnAlloc(act, arch, cpg->layers.count);
		*nnp->gd_map = stuc_nnAlloc(act, arch, cpg->layers.count);

		nnp->tInput  = (Stuc_mat) {.rows = tData_samples, .cols=arch[0], .stride=arch[0]+arch[cpg->layers.count-1], .el=tData};
		nnp->tOutput = (Stuc_mat) {.rows = tData_samples, .cols=arch[cpg->layers.count-1], .stride=arch[0]+arch[cpg->layers.count-1], .el=tData + nnp->tInput.cols};

		free(arch);
		free(act);

		srand(time(0));
		stuc_nnRand(*nnp->nn, -1, 1);

		log(INFO, "NN Alloced!\n");

		nnp->prepared = true;
		nnp->learning_enabled = true;
		nnp->learning_paused = true;
	}

	if (nnp->prepared && nnp->learning_enabled && !nnp->learning_paused) {
		for (size_t i = 0; i < (size_t)nnp->batch_size; i++) {
			stuc_nnBackpropNoAlloc(*nnp->nn, *nnp->gd_map, nnp->tInput, nnp->tOutput, 1);
			stuc_nnApplyDiff(*nnp->nn, *nnp->gd_map, cpg->learnRateValue/100);
		}

		nnp->cost = stuc_nnCost(*nnp->nn, nnp->tInput, nnp->tOutput);
		nnp->epoch_count++;
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
	crg->provjeraRezToggleRect = (Rectangle) { sX, sY,  72,  24 }; // ToggleGroup: nacinProvjereRezultata
	sY += 25;

	sY += innerLayerPad;
	crg->resultsRect = (Rectangle) { sX, sY, crg->boundingBox.width - 4*layerPad, crg->boundingBox.height - innerLayerPad - (sY - crg->boundingBox.y) };
}
void drawCheckResultGroup(CheckResultGroup *crg, NeuralNetworkPreview *nnp) {
	GuiGroupBox(crg->boundingBox,  crg->provjeraRezultataText);
	GuiToggleGroup( crg->provjeraRezToggleRect,
		// TextFormat("%sTEKST", ICON_TO_TEXT(ICON_FILETYPE_TEXT)),
		TextFormat("%sTEKST;%sSLIKA",
			ICON_TO_TEXT(ICON_FILETYPE_TEXT),
			ICON_TO_TEXT(ICON_FILETYPE_IMAGE)),
		&crg->nacinProvjereRezultataActive);

	if (crg->nacinProvjereRezultataActive == 0) {
		Font font = GuiGetFont();
		int txt_size = GuiGetStyle(DEFAULT, TEXT_SIZE);
		int txt_space = GuiGetStyle(DEFAULT, TEXT_SPACING);
		Color txt_colour = SC_NORML_TEXT;

		size_t top_padding = 25;
		size_t element_padding = 10;
		size_t layer_padding = 15;
		for (size_t i = 0; i < tData_samples; i++) {
			Vector2 pos = {crg->resultsRect.x, crg->resultsRect.y+layer_padding*i + top_padding};
			for (size_t j = 0; j < STUC_NN_INPUT(*nnp->nn).cols; j++) {
				pos.x += element_padding;
				DrawTextEx(font, TextFormat("%.0f", STUC_MAT_AT(nnp->tInput, i, j)), pos, txt_size, txt_space, txt_colour);
				STUC_AT_INPUT(*nnp->nn, j) = STUC_MAT_AT(nnp->tInput, i, j);
			}
			stuc_nnForward(*nnp->nn);

			pos.x += 2*element_padding;
			DrawTextEx(font, "rezultat ->", pos, txt_size, txt_space, txt_colour);
			Vector2 result_text_size = MeasureTextEx(font, "rezultat ->", txt_size, txt_space);
			pos.x += result_text_size.x + 2*element_padding;
			for (size_t k = 0; k < STUC_NN_OUTPUT(*nnp->nn).cols; k++) {
				pos.x += 3*element_padding;
				DrawTextEx(font, TextFormat("%.2f", STUC_AT_OUTPUT(*nnp->nn, k)), pos, txt_size, txt_space, txt_colour);
			}
		}
	} else if (crg->nacinProvjereRezultataActive == 1) {
	        size_t outputs = STUC_NN_OUTPUT(*nnp->nn).cols;

		size_t padding = 10;

		if (outputs > 10 || tData_samples > 10) padding = 1;
		if (outputs > 69 || tData_samples > 69) padding = 0;

	        float block_w = (crg->resultsRect.width - (outputs - 1)*padding)/outputs;
	        float block_h = (crg->resultsRect.height - (tData_samples - 1)*padding)/tData_samples;

		for (size_t i = 0; i < tData_samples; i++) {
			for (size_t j = 0; j < STUC_NN_INPUT(*nnp->nn).cols; j++) {
				STUC_AT_INPUT(*nnp->nn, j) = STUC_MAT_AT(nnp->tInput, i, j);
			}

			stuc_nnForward(*nnp->nn);


			for (size_t k = 0; k < outputs; k++) {
				Vector2 pos = {
					crg->resultsRect.x + block_w*k + k*padding,
					crg->resultsRect.y + block_h*i + i*padding
				};

				float_t out = STUC_AT_OUTPUT(*nnp->nn, k);
				float_t expected = STUC_MAT_AT(nnp->tOutput, i, k);

				Color block_colour = Fade(SC_NORML_TEXT, fabs(out/expected));
				DrawRectangleV(pos, (Vector2) {block_w, block_h}, block_colour);
			}
		}

	}
}
