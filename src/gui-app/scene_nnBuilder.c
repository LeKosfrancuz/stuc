#include "scene_nnBuilder.h"

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

	s->aX.l1 = leftPad; s->aX.l2 = leftPad;
	s->aY.s1 =  topPad; s->aY.s2 =  topPad;

	if (s->costPanelGroup.isShown) {
		s->costPanelGroup.boundingBox = (Rectangle) { s->aX.l1, s->aY.s1, 0.55 * g_screenWidth, 0.26 * g_screenHeight }; // GroupBox: costFunction
		s->aY.s1 += s->costPanelGroup.boundingBox.height + innerLayerPad;
		s->aX.l1 += s->costPanelGroup.boundingBox.width + layerPad;
	} else {
		s->aY.s1 += s->controlPanelGroup.boundingBox.height + innerLayerPad;
	}

	s->neuralNetworkPreview.boundingBox = (Rectangle) { s->aX.l2, s->aY.s1, 0.55 * g_screenWidth, g_screenHeight - (s->aY.s1 + bottomPad) }; //DummyRec: neuralNetwork
	s->aY.s1 += s->neuralNetworkPreview.boundingBox.height + innerLayerPad;
	s->aX.l2 += s->neuralNetworkPreview.boundingBox.width + layerPad;
	
	{
		size_t width = g_screenWidth - (s->aX.l1 + rightPad);
		size_t height= fmax(0.25 * g_screenHeight, 145);
		s->controlPanelGroup.boundingBox = (Rectangle) { s->aX.l1, s->aY.s2, width, height }; // GroupBox: controlPanel
	}

	s->aY.s2 += s->controlPanelGroup.boundingBox.height + innerLayerPad;
	s->checkResultGroup.boundingBox = (Rectangle) { s->aX.l2, s->aY.s2, g_screenWidth - (s->aX.l2 + rightPad), g_screenHeight - (s->aY.s2 + bottomPad) }; //GroupBox: provjeraRezultata


	updateControlPanelGroup(&s->controlPanelGroup, layerPad);
	updateCostFunctionPanelGroup(&s->costPanelGroup, topPad, innerLayerPad);
	updateNeuralNetworkPreview(&s->neuralNetworkPreview, &s->controlPanelGroup);
	updateCheckResultGroup(&s->checkResultGroup, layerPad, innerLayerPad);

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

}
void scene_nnBuilderDraw(Scene_nnBuilder *s) {
	// Draw controls
	drawNeuralNetworkPreview(&s->neuralNetworkPreview, &s->controlPanelGroup);
	drawCostFunctionPanelGroup(&s->costPanelGroup);
	drawCheckResultGroup(&s->checkResultGroup);
	drawControlPanelGroup(&s->controlPanelGroup);
}




ControlPanelGroup initControlPanelGroup(void) {
	ControlPanelGroup cpg = {
		.controlPanelText               = "Kontrole",                    // GROUPBOX:    controlPanel
		.layerChoiceText                = "ONE;TWO;THREE;FOUR;FIVE;SIX", // TOGGLEGROUP: layerChoice
		.activationFunctionChoiceText   = "SIGMOID;RE:LU;TANH;SIN",      // COMBOBOX:    activationFunctionChoice
		.learnRateText                  = "Learn rate",                  // SLIDERBAR:   learnRate
		.layerLabelText                 = "Layer",                       // LABEL:       layerLabel
		.activationLabelText            = "Activation",                  // LABEL:       activationLabel
		.numberOfNeuronsText            = "Number of N "                 // SPINNER:     numberOfNeurons
	};
	
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

	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && CheckCollisionPointRec(GetMousePosition(), cpg->activationChoiceCB)) {
		cpg->activationFunctionChoiceCurrent = ((cpg->activationFunctionChoiceCurrent + STUC_ACTIVATIONS_COUNT) - 1) % STUC_ACTIVATIONS_COUNT;
	}

	if (cpg->layerChoiceCurrent > (int)cpg->layers.count) {
		da_append(&cpg->layers, (Gui_nnLayer){0});
		log(INFO, "Dodan layer %zu!\n", cpg->layers.count);
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

void drawControlPanelGroup(ControlPanelGroup *cpg) {

	GuiGroupBox(    cpg->boundingBox,	 cpg->controlPanelText);
	
	if (GuiSpinner( cpg->layerChoiceTG, NULL, 
			     &cpg->layerChoiceCurrent, 1, 1000, cpg->layerChoiceEditMode)) {
		cpg->layerChoiceEditMode = !cpg->layerChoiceEditMode;
	} 
	if (GuiButton(cpg->removeLayerBT, ICON_TO_TEXT(ICON_BIN))) {
		cpg->removeCurrLayer = true;
	}

	GuiComboBox(    cpg->activationChoiceCB, cpg->activationFunctionChoiceText, &cpg->activationFunctionChoiceCurrent);
	if (GuiSpinner( cpg->nOfNeuronsS,	 cpg->numberOfNeuronsText, 
			&cpg->nOfNeuronsValue, 0, 1000, cpg->nOfNeuronsEditMode)) 
			cpg->nOfNeuronsEditMode = !cpg->nOfNeuronsEditMode;
	GuiSliderBar(   cpg->learnRateSB,	 cpg->learnRateText, NULL, &cpg->learnRateValue, 0, 100);
	GuiLabel(       cpg->layerL,		 cpg->layerLabelText);
	GuiLabel(       cpg->activationL,	 cpg->activationLabelText);
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
void updateCostFunctionPanelGroup(CostFunctionPanelGroup *cfpg, size_t topPad, size_t expandButtonWidth) {
	cfpg->expandButtonRec = (Rectangle) { 0, topPad, expandButtonWidth, 0.1 * g_screenHeight };
}

NeuralNetworkPreview initNeuralNetworkPreview(void) {
	NeuralNetworkPreview nnp = {
		.dummyRecText = "Prikaz Neuronske mreze"     // DUMMYREC:    neuralNetwork
	};
	return nnp;
}

void updateNeuralNetworkPreview(NeuralNetworkPreview *nnp, ControlPanelGroup *cpg) {
	nnp->boundingBox.width = 0.55 * g_screenWidth;
	if (CheckCollisionPointRec(GetMousePosition(), nnp->boundingBox) || cpg->removeCurrLayer) {
		if (IsKeyPressed(KEY_LEFT)) {
			if (cpg->layerSelectedCurrent > 0) {
				cpg->layerChoiceCurrent--;
			}
		}
		
		if (IsKeyPressed(KEY_RIGHT)) {
			if ((size_t)cpg->layerSelectedCurrent < 1000) {
				cpg->layerChoiceCurrent++;
			} else {
				log(WARN, "Can not add another layer!\n");
			}
		}

		if (IsKeyPressed(KEY_UP)) {
			cpg->nOfNeuronsValue++;
		}

		if (IsKeyPressed(KEY_DOWN)) {
			if (cpg->nOfNeuronsValue > 0) {
				cpg->nOfNeuronsValue--;
			} else {
				da_remove(&cpg->layers, cpg->layerSelectedCurrent);
				log(INFO, "Izbrisan layer %d!\n", cpg->layerSelectedCurrent);
				cpg->layerChoiceCurrent--;
			}
		}

		if (IsKeyPressed(KEY_DELETE) || cpg->removeCurrLayer) {
			// TODO: napraviti da kad se briše layer 0, da ne obriše i prvi
			cpg->removeCurrLayer = false;
			cpg->nOfNeuronsValue = 0;
			da_remove(&cpg->layers, cpg->layerSelectedCurrent);
			log(INFO, "Izbrisan layer %d!\n", cpg->layerSelectedCurrent);
			cpg->layerChoiceCurrent--;
		}

		if (IsKeyPressed(KEY_INSERT)) {
			da_insert(&cpg->layers, cpg->layerSelectedCurrent, (Gui_nnLayer){0});
			log(INFO, "Umetnut layer %d!\n", cpg->layerSelectedCurrent);
		}
	}
	return;
}
void drawNeuralNetworkPreview(NeuralNetworkPreview *nnp, ControlPanelGroup *cpg) {
	GuiDummyRec(nnp->boundingBox,  nnp->dummyRecText);

	size_t layerCount = cpg->layers.count;
	size_t neuronSize = 10;
	size_t nBottmPad = 3 * neuronSize;
	size_t nRightPad = 4 * neuronSize;
	float neuronMapInnerWidth = neuronSize*(layerCount - 1) + nRightPad*(layerCount - 1);
	/*
	 * This is the width of the neuron map calculated from the circle centers instead of 
	 * boxes arround them. The real width is this width + 2*neuronSize
	 */
	size_t dx = nnp->boundingBox.x + nnp->boundingBox.width/2 - (neuronMapInnerWidth)/2;

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
		float layerInnerHeight = nOfNeurons*(neuronSize - 1) + nBottmPad*(nOfNeurons - 1);
		size_t dy = nnp->boundingBox.y + nnp->boundingBox.height/2 - (layerInnerHeight)/2;

		for (size_t j = 0; j < nOfNeurons; j++) {
			DrawCircle(dx, dy, neuronSize, SC_PRESS_BORDER);

			if (i < layerCount - 1) {
				size_t nextLayerNOfN = cpg->layers.items[i+1].nOfNeurons;
				float nextLIHeight = nextLayerNOfN*(neuronSize - 1) + nBottmPad*(nextLayerNOfN - 1);
				size_t y = nnp->boundingBox.y + nnp->boundingBox.height/2 - (nextLIHeight)/2;
				for (size_t k = 0; k < nextLayerNOfN; k++) {
					size_t x = dx + nRightPad + neuronSize;
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
