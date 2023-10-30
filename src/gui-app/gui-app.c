#include "../stuc/stuc.h"
#include <raylib.h>
#include <string.h>
#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"

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

#define VISUAL_STYLES_COUNT 12
#define ICON_TO_TEXT(icon) TextFormat("#%03i#", (icon) & (0x1ff))

#define SC_ACCENT_MID_T GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_PRESSED))
#define SC_ACCENT_LOW_B GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED))

#define SC_PRIMARY_MID_T   GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL))
#define SC_SECUNDARY_MID_T GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_FOCUSED))

#define SC_BACKGROUND GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))

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
	int   layerChoiceCount;               // ToggleGroup: 	layerChoice
	int   activationFunctionChoiceCurrent;// ComboBox: 	activationChoice
	int   nOfNeuronsValue;                // Spinner: 	nOfNeurons
	float learnRateValue;                 // SliderBar: 	learnRate

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

void changeVisualStyle(int* prevStyle, const int crntStyle);

ControlPanelGroup initControlPanelGroup(void);
void updateControlPanelGroup(ControlPanelGroup* cpg, size_t xPad);
void drawControlPanelGroup(ControlPanelGroup* cpg);

CostFunctionPanelGroup initCostFunctionPanelGroup(void);
void updateCostFunctionPanelGroup(CostFunctionPanelGroup* cfpg, size_t topPad, size_t expandButtonWidth);
void drawCostFunctionPanelGroup(CostFunctionPanelGroup* cfpg);


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
	const char *neuralNetworkText               = "Prikaz Neuronske mreze";     // DUMMYREC:    neuralNetwork
	const char *provjeraRezultataText           = "Provjera rezultata";         // GROUPBOX:    provjeraRezultata

	ControlPanelGroup controlPanelGroup   = initControlPanelGroup();
	CostFunctionPanelGroup costPanelGroup = initCostFunctionPanelGroup();

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

		Rectangle neuralNetworkRec = { aX.l2, aY.s1, 0.55 * g_screenWidth, g_screenHeight - (aY.s1 + bottomPad) }; //DummyRec: neuralNetwork
		aY.s1 += neuralNetworkRec.height + innerLayerPad;
		aX.l2 += neuralNetworkRec.width + layerPad;
		
		{
			size_t width = g_screenWidth - (aX.l1 + rightPad);
			size_t height= fmax(0.25 * g_screenHeight, 145);
			controlPanelGroup.boundingBox = (Rectangle) { aX.l1, aY.s2, width, height }; // GroupBox: controlPanel
		}

		aY.s2 += controlPanelGroup.boundingBox.height + innerLayerPad;
		Rectangle testResultRec = { aX.l2, aY.s2, g_screenWidth - (aX.l2 + rightPad), g_screenHeight - (aY.s2 + bottomPad) }; //GroupBox: provjeraRezultata


		updateCostFunctionPanelGroup(&costPanelGroup, topPad, innerLayerPad);
		updateControlPanelGroup(&controlPanelGroup, layerPad);
		Rectangle testResultTypeRec;
		{
			size_t sX = testResultRec.x + 2*layerPad;
			size_t sY = testResultRec.y + innerLayerPad;
			testResultTypeRec = (Rectangle) { sX, sY,  72,  24 }; // ToggleGroup: nacinProvjereRezultata
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
		GuiDummyRec(neuralNetworkRec,  neuralNetworkText);
		drawCostFunctionPanelGroup(&costPanelGroup);
		GuiGroupBox(testResultRec,  provjeraRezultataText);
		drawControlPanelGroup(&controlPanelGroup);

		GuiToggleGroup( testResultTypeRec,  
			TextFormat("%sTEKST;%sSLIKA", 
				ICON_TO_TEXT(ICON_FILETYPE_TEXT), 
				ICON_TO_TEXT(ICON_FILETYPE_IMAGE)), 
			&nacinProvjereRezultataActive);



    // BORDER_COLOR_NORMAL = 0,    // Control border color in STATE_NORMAL
    // BASE_COLOR_NORMAL,          // Control base color in STATE_NORMAL
    // TEXT_COLOR_NORMAL,          // Control text color in STATE_NORMAL
    // BORDER_COLOR_FOCUSED,       // Control border color in STATE_FOCUSED
    // BASE_COLOR_FOCUSED,         // Control base color in STATE_FOCUSED
    // TEXT_COLOR_FOCUSED,         // Control text color in STATE_FOCUSED
    // BORDER_COLOR_PRESSED,       // Control border color in STATE_PRESSED
    // BASE_COLOR_PRESSED,         // Control base color in STATE_PRESSED
    // TEXT_COLOR_PRESSED,         // Control text color in STATE_PRESSED
    // BORDER_COLOR_DISABLED,      // Control border color in STATE_DISABLED
    // BASE_COLOR_DISABLED,        // Control base color in STATE_DISABLED
    // TEXT_COLOR_DISABLED,        // Control text color in STATE_DISABLED
		#define SC_ACCENT_HIG_O GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_PRESSED))
		// TODO: Indeksiranje boja

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
		Color textColor = SC_PRIMARY_MID_T;

		Vector2   textPosition   = {
				  buttonPosition.width - (buttonPosition.width - textMeasure.y) / 2, 
				  buttonPosition.y + buttonPosition.height / 2 - textMeasure.x / 2 
				};
		Vector2   rotationPoint  = { 0, 0 };

		cfpg->isShown = GuiButton(buttonPosition, NULL );

		if (CheckCollisionPointRec(GetMousePosition(), buttonPosition)) {
			textColor = SC_SECUNDARY_MID_T;
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
				textColor = SC_ACCENT_MID_T;
		}

		DrawTextPro(GuiGetFont(), cfpg->costExpandButtonText, textPosition, rotationPoint, 90, textSize, 1, textColor);
	}
}
void updateCostFunctionPanelGroup(CostFunctionPanelGroup* cfpg, size_t topPad, size_t expandButtonWidth) {
	cfpg->expandButtonRec = (Rectangle) { 0, topPad, expandButtonWidth, 0.1 * g_screenHeight };
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
