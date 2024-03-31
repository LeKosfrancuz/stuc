#include "../stuc/stuc.h"
#include "external/raylib.h"
#include <string.h>
#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"
#include "dynamic-arrays.h"

// raygui embedded styles
#include "./external/styles/style_cyber.h"       // raygui style: cyber
#include "./external/styles/style_jungle.h"      // raygui style: jungle
#include "./external/styles/style_lavanda.h"     // raygui style: lavanda
#include "./external/styles/style_dark.h"        // raygui style: dark
#include "./external/styles/style_ashes.h"	 // raygui style: ashes
#include "./external/styles/style_bluish.h"      // raygui style: bluish
#include "./external/styles/style_terminal.h"    // raygui style: terminal
#include "./external/styles/style_enefete.h"     // raygui style: enfete
#include "./external/styles/style_sunny.h"       // raygui style: sunny
#include "./external/styles/style_cherry.h"      // raygui style: cherry
#include "./external/styles/style_candy.h"       // raygui style: candy

#define INFO  "[\x1b[0;36mINFO\x1b[0m]: "
#define WARN  "[\x1b[0;38;5;208mWARNING\x1b[0m]: "
#define ERROR "[\x1b[0;31mERROR\x1b[0m]: "
#define log(level, ...)           printf(level); printf(__VA_ARGS__);

#define VISUAL_STYLES_COUNT 12
#define ICON_TO_TEXT(icon) TextFormat("#%03i#", (icon) & (0x1ff))

#define SC_NORML_BASE GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL))
#define SC_FOCUS_BASE GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED))
#define SC_PRESS_BASE GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED))

#define SC_NORML_BORDER GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL))
#define SC_FOCUS_BORDER GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_FOCUSED))
#define SC_PRESS_BORDER GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_PRESSED))

#define SC_NORML_TEXT   GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL))
#define SC_FOCUS_TEXT   GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_FOCUSED))
#define SC_PRESS_TEXT   GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_PRESSED))

#define SC_BACKGROUND GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))

typedef struct {
	size_t nOfNeurons;
	int activation;
} Gui_nnLayer;

typedef struct {
	Gui_nnLayer *items;
	size_t count;
	size_t capacity;
} Gui_nnLayers;

typedef struct {
	Rectangle boundingBox;
	Rectangle layerL;
	Rectangle activationL;
	Rectangle layerChoiceTG;
	Rectangle activationChoiceCB;
	Rectangle nOfNeuronsS;
	Rectangle learnRateSB;

	bool  nOfNeuronsEditMode;             // Spinner: 	nOfNeurons
	int   layerChoiceCurrent;             // ToggleGroup: 	layerChoice
	int layerSelectedCurrent;	      // current layer actualy shown
	int   layerChoiceCount;               // ToggleGroup: 	layerChoice
	int   activationFunctionChoiceCurrent;// ComboBox: 	activationChoice
	int   nOfNeuronsValue;                // Spinner: 	nOfNeurons
	float learnRateValue;                 // SliderBar: 	learnRate
	
	Gui_nnLayers layers;

	const char *controlPanelText;                // GROUPBOX:    controlPanel
	const char *layerChoiceText;                 // TOGGLEGROUP: layerChoice
	const char *activationFunctionChoiceText;    // COMBOBOX:    activationFunctionChoice
	const char *learnRateText;                   // SLIDERBAR:   learnRate
	const char *layerLabelText;                  // LABEL:       layerLabel
	const char *activationLabelText;             // LABEL:       activationLabel
	const char *numberOfNeuronsText;             // SPINNER:     numberOfNeurons
}ControlPanelGroup;

typedef struct {
	Rectangle boundingBox;
	Rectangle expandButtonRec;

	bool isShown; // Misli se na cijelu grupu

	const char *costFunctionText;         // GROUPBOX:    costFunction (bounding box)
	const char* costExpandButtonText;     // BUTTON:      expandButton
} CostFunctionPanelGroup;

typedef struct {
	Rectangle boundingBox;

	Stuc_nn nn;

	const char *dummyRecText;
} NeuralNetworkPreview;

void changeVisualStyle(int* prevStyle, const int crntStyle);

ControlPanelGroup initControlPanelGroup(void);
void updateControlPanelGroup(ControlPanelGroup* cpg, size_t xPad);
void drawControlPanelGroup(ControlPanelGroup* cpg);

CostFunctionPanelGroup initCostFunctionPanelGroup(void);
void updateCostFunctionPanelGroup(CostFunctionPanelGroup* cfpg, size_t topPad, size_t expandButtonWidth);
void drawCostFunctionPanelGroup(CostFunctionPanelGroup* cfpg);

NeuralNetworkPreview initNeuralNetworkPreview(void);
void updateNeuralNetworkPreview(NeuralNetworkPreview* nnp, ControlPanelGroup* cpg);
void drawNeuralNetworkPreview(NeuralNetworkPreview* nnp, ControlPanelGroup* cpg);


size_t g_screenWidth = 800;
size_t g_screenHeight = 600;

int main(void) {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(g_screenWidth, g_screenHeight, "Stuc");

	// Load default style
    	GuiLoadStyleBluish();
    	int activeVisualStyle = 5;
    	int prevActiveVisualStyle = 5;

	// Initilize controls variables
	int  nacinProvjereRezultataActive = 0;            // ToggleGroup: nacinProvjereRezultata

	// Init controls rectangles
	struct allignX { size_t l1,l2; } aX = {0};
	struct allignY { size_t s1,s2; } aY = {0};
	size_t leftPad   = 24;
	size_t rightPad  = 24;
	size_t topPad    = 48;
	size_t bottomPad = 24;
	size_t layerPad  = 8;
	size_t innerLayerPad = 16;
	

	SetTargetFPS(60);

	// bool WindowBoxIsActive = true;
	// char *inputBoxText = malloc(25);
	// strcpy(inputBoxText, "Input Text");

	// Const text
	const char* styleChooserText 		    = "default;Jungle;Lavanda;Dark;Ashes;Bluish;Cyber;Terminal;Enefete;Sunny;Cherry;Candy"; // STYLE CHOOSER
	const char *provjeraRezultataText           = "Provjera rezultata";         // GROUPBOX:    provjeraRezultata

	ControlPanelGroup controlPanelGroup   = initControlPanelGroup();
	CostFunctionPanelGroup costPanelGroup = initCostFunctionPanelGroup();
	NeuralNetworkPreview neuralNetworkPreview = initNeuralNetworkPreview();

	while (!WindowShouldClose()) {
		g_screenHeight = (size_t) GetScreenHeight();
		g_screenWidth  = (size_t) GetScreenWidth();

		changeVisualStyle(&prevActiveVisualStyle, activeVisualStyle);
		
		aX.l1 = leftPad; aX.l2 = leftPad;
		aY.s1 =  topPad; aY.s2 =  topPad;


		if (costPanelGroup.isShown) {
			costPanelGroup.boundingBox = (Rectangle) { aX.l1, aY.s1, 0.55 * g_screenWidth, 0.26 * g_screenHeight }; // GroupBox: costFunction
			aY.s1 += costPanelGroup.boundingBox.height + innerLayerPad;
			aX.l1 += costPanelGroup.boundingBox.width + layerPad;
		} else {
			aY.s1 += controlPanelGroup.boundingBox.height + innerLayerPad;
		}

		neuralNetworkPreview.boundingBox = (Rectangle) { aX.l2, aY.s1, 0.55 * g_screenWidth, g_screenHeight - (aY.s1 + bottomPad) }; //DummyRec: neuralNetwork
		aY.s1 += neuralNetworkPreview.boundingBox.height + innerLayerPad;
		aX.l2 += neuralNetworkPreview.boundingBox.width + layerPad;
		
		{
			size_t width = g_screenWidth - (aX.l1 + rightPad);
			size_t height= fmax(0.25 * g_screenHeight, 145);
			controlPanelGroup.boundingBox = (Rectangle) { aX.l1, aY.s2, width, height }; // GroupBox: controlPanel
		}

		aY.s2 += controlPanelGroup.boundingBox.height + innerLayerPad;
		Rectangle testResultRec = { aX.l2, aY.s2, g_screenWidth - (aX.l2 + rightPad), g_screenHeight - (aY.s2 + bottomPad) }; //GroupBox: provjeraRezultata


		updateControlPanelGroup(&controlPanelGroup, layerPad);
		updateCostFunctionPanelGroup(&costPanelGroup, topPad, innerLayerPad);
		updateNeuralNetworkPreview(&neuralNetworkPreview, &controlPanelGroup);
		Rectangle testResultTypeRec;
		{
			size_t sX = testResultRec.x + 2*layerPad;
			size_t sY = testResultRec.y + innerLayerPad;
			testResultTypeRec = (Rectangle) { sX, sY,  72,  24 }; // ToggleGroup: nacinProvjereRezultata
		}

		if (IsKeyPressed(KEY_L)) {
			log(INFO, "Layer count is: %zu\n", controlPanelGroup.layers.count);
			size_t layerCount = controlPanelGroup.layers.count;
			size_t neuronSize = 10;
			// size_t nBottmPad = 3 * neuronSize;
			size_t nRightPad = 4 * neuronSize;
			float nnPreviewWidth = (layerCount+1)*neuronSize + (layerCount-1)*(nRightPad);
			NeuralNetworkPreview* nnp = &neuralNetworkPreview;
			size_t dx = /*nnp->boundingBox.x +*/ nnp->boundingBox.width/2 - nnPreviewWidth/2;

			log(INFO, "bounding box; width: %.0f, height: %.0f\n", nnp->boundingBox.width, nnp->boundingBox.height);
			log(INFO, "nnPreview; width: %.0f, dx: %zu\n", nnPreviewWidth, dx);
		}

		BeginDrawing();
		ClearBackground(SC_BACKGROUND);

		GuiLabel((Rectangle){ leftPad, 10, (int)(g_screenWidth / 10), 24 }, "Style:");
		Rectangle styleChooserRec = { leftPad + 42 ,10, 120, 24 };
            	GuiComboBox(styleChooserRec, styleChooserText, &activeVisualStyle);
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && CheckCollisionPointRec(GetMousePosition(), styleChooserRec)) {
			activeVisualStyle = ((activeVisualStyle + VISUAL_STYLES_COUNT) - 1) % VISUAL_STYLES_COUNT;
		}

		// Draw controls
		drawNeuralNetworkPreview(&neuralNetworkPreview, &controlPanelGroup);
		drawCostFunctionPanelGroup(&costPanelGroup);
		GuiGroupBox(testResultRec,  provjeraRezultataText);
		drawControlPanelGroup(&controlPanelGroup);

		GuiToggleGroup( testResultTypeRec,  
			TextFormat("%sTEKST;%sSLIKA", 
				ICON_TO_TEXT(ICON_FILETYPE_TEXT), 
				ICON_TO_TEXT(ICON_FILETYPE_IMAGE)), 
			&nacinProvjereRezultataActive);

		EndDrawing();
	}

	CloseWindow();
	return 0;
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
	cpg.layerChoiceCount                = 6;            // ToggleGroup: layerChoice
	cpg.activationFunctionChoiceCurrent = 0;            // ComboBox:    activationChoice
	cpg.nOfNeuronsEditMode              = false;        // Spinner:     nOfNeurons
	cpg.nOfNeuronsValue                 = 0;            // Spinner:     nOfNeurons
	cpg.learnRateValue                  = 0.0f;         // SliderBar:   learnRate
	
	return cpg;
}

void updateControlPanelGroup(ControlPanelGroup* cpg, size_t layerPad) {
	// Određivanje dinamičkih pozicija elemenata u grupi
	size_t leftPad = 11*layerPad;
	size_t rightPad = 2*layerPad;

	size_t sX = cpg->boundingBox.x + leftPad;
	size_t controlsWidth = cpg->boundingBox.width - (leftPad + rightPad);
	
	size_t sY = 64;
	size_t spacing = 32;
	const float lcc = cpg->layerChoiceCount;
	cpg->layerL 		 = (Rectangle){ sX - 42,  sY,  40,  24 };			    // Label: layerLabel
	cpg->activationL 	 = (Rectangle){ sX - 65,  sY + spacing,  56,  24 };		    // Label: activationLabel
	cpg->layerChoiceTG  	 = (Rectangle){ sX, sY, controlsWidth/lcc - 1, 24 }; sY += spacing; // ToggleGroup: layerChoice
	cpg->activationChoiceCB  = (Rectangle){ sX, sY, controlsWidth,         24 }; sY += spacing; // ComboBox: activationFunctionChoice
	cpg->nOfNeuronsS  	 = (Rectangle){ sX, sY, controlsWidth,         24 }; sY += spacing; // Spinner: numberOfNeurons
	cpg->learnRateSB  	 = (Rectangle){ sX, sY, controlsWidth,         16 }; sY += spacing; // SliderBar: learnRate

	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && CheckCollisionPointRec(GetMousePosition(), cpg->activationChoiceCB)) {
		cpg->activationFunctionChoiceCurrent = ((cpg->activationFunctionChoiceCurrent + STUC_ACTIVATIONS_COUNT) - 1) % STUC_ACTIVATIONS_COUNT;
	}

	if (cpg->layers.count < (size_t)cpg->layerChoiceCurrent + 1) {
		da_append(&cpg->layers, (Gui_nnLayer){0});
		log(INFO, "Dodan layer %zu!\n", cpg->layers.count);
	} else {
		if (cpg->layerChoiceCurrent == cpg->layerSelectedCurrent) {
			cpg->layers.items[cpg->layerChoiceCurrent].nOfNeurons = cpg->nOfNeuronsValue;
			cpg->layers.items[cpg->layerChoiceCurrent].activation = cpg->activationFunctionChoiceCurrent;
		} else {
			cpg->layerSelectedCurrent = cpg->layerChoiceCurrent;
			cpg->nOfNeuronsValue = cpg->layers.items[cpg->layerChoiceCurrent].nOfNeurons;
			cpg->activationFunctionChoiceCurrent = cpg->layers.items[cpg->layerChoiceCurrent].activation;
		}
	}
		
	return;
}

void drawControlPanelGroup(ControlPanelGroup* cpg) {

	GuiGroupBox(    cpg->boundingBox,	 cpg->controlPanelText);
	GuiToggleGroup( cpg->layerChoiceTG,	 cpg->layerChoiceText, &cpg->layerChoiceCurrent);
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

void drawCostFunctionPanelGroup(CostFunctionPanelGroup* cfpg) {

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
void updateCostFunctionPanelGroup(CostFunctionPanelGroup* cfpg, size_t topPad, size_t expandButtonWidth) {
	cfpg->expandButtonRec = (Rectangle) { 0, topPad, expandButtonWidth, 0.1 * g_screenHeight };
}

NeuralNetworkPreview initNeuralNetworkPreview(void) {
	NeuralNetworkPreview nnp = {
		.dummyRecText = "Prikaz Neuronske mreze"     // DUMMYREC:    neuralNetwork
	};
	return nnp;
}

void updateNeuralNetworkPreview(NeuralNetworkPreview* nnp, ControlPanelGroup* cpg) {
	nnp->boundingBox.width = 0.55 * g_screenWidth;
	if (CheckCollisionPointRec(GetMousePosition(), nnp->boundingBox)) {
		if (IsKeyPressed(KEY_LEFT)) {
			if (cpg->layerChoiceCurrent > 0) {
				cpg->layerChoiceCurrent--;
			}
		}
		
		if (IsKeyPressed(KEY_RIGHT)) {
			if (cpg->layerChoiceCurrent < cpg->layerChoiceCount - 1) {
				cpg->layerChoiceCurrent++;
			}
		}

		if (IsKeyPressed(KEY_UP)) {
			cpg->nOfNeuronsValue++;
		}

		if (IsKeyPressed(KEY_DOWN)) {
			if (cpg->nOfNeuronsValue > 0) {
				cpg->nOfNeuronsValue--;
			}
		}
	}
	return;
}
void drawNeuralNetworkPreview(NeuralNetworkPreview* nnp, ControlPanelGroup* cpg) {
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

		size_t focusBar_originX = dx + cpg->layerChoiceCurrent*(neuronSize + nRightPad) - neuronSize - nRightPad/2; 
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


void changeVisualStyle(int* prevVisualStyle, const int visualStyle) {
	float scaleDown = 0.005;
	const size_t styleSpecificMinFontSize[VISUAL_STYLES_COUNT] = {13, 12, 16, 17, 17, 10, 15, 18, 18, 18, 15, 17};
	#define SET_FONT_SIZE(size) GuiSetStyle(DEFAULT, TEXT_SIZE, fmax((styleSpecificMinFontSize[visualStyle]), (int)(size)))

	if (visualStyle != *prevVisualStyle)
        {
            GuiLoadStyleDefault();
            switch (visualStyle)
            {	
                case  0: break; // Default style
                case  1: GuiLoadStyleJungle();	 break;
                case  2: GuiLoadStyleLavanda();  break;
                case  3: GuiLoadStyleDark();	 break;
		case  4: GuiLoadStyleAshes();	 break;
                case  5: GuiLoadStyleBluish();	 break;
                case  6: GuiLoadStyleCyber();	 break;
                case  7: GuiLoadStyleTerminal(); break;
		case  8: GuiLoadStyleEnefete();	 break;
		case  9: GuiLoadStyleSunny();	 break;
		case 10: GuiLoadStyleCherry();	 break;
		case 11: GuiLoadStyleCandy();	 break;
                default: break;
            }

            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);

            *prevVisualStyle = visualStyle;
        }
	SET_FONT_SIZE(scaleDown * g_screenWidth);

	#undef SET_FONT_SIZE
}
