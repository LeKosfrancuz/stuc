#include "stuc.h"
#include <raylib.h>
#include <string.h>
#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"
#define GUI_STUC_IMPLEMENTATION
#include "external/gui_Stuc.h"


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

void changeVisualStyle(int* prevStyle, const int crntStyle);
void updateControlPanelGroup(ControlPanelGroup* cpg, size_t xPad);
ControlPanelGroup initControlPanelGroup(void);
void drawControlPanelGroup(ControlPanelGroup* cpg);

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
	int   nacinProvjereRezultataActive   = 0;            // ToggleGroup: nacinProvjereRezultata

	// Init controls rectangles
	struct allignX { size_t l1,l2; } aX = {0};
	struct allignY { size_t s1,s2; } aY = {0};
	size_t leftPad   = 24;
	size_t rightPad  = 24;
	size_t topPad    = 48;
	size_t bottomPad = 24;
	size_t layerPad = 8;
	size_t innerLayerPad = 16;
	

	SetTargetFPS(60);

	// bool WindowBoxIsActive = true;
	// char *inputBoxText = malloc(25);
	// strcpy(inputBoxText, "Input Text");

	// Const text
	const char* stylesPossibleText 		    = "default;Jungle;Lavanda;Dark;Ashes;Bluish;Cyber;Terminal;Enefete;Sunny;Cherry;Candy"; // STYLE CHOOSER
	const char *neuralNetworkText               = "Prikaz Neuronske mreze";     // DUMMYREC:    neuralNetwork
	const char *costFunctionText                = "Graf tocnosti meze";         // GROUPBOX:    costFunction
	const char *provjeraRezultataText           = "Provjera rezultata";         // GROUPBOX:    provjeraRezultata

	ControlPanelGroup controlPanelGroup = initControlPanelGroup();

	while (!WindowShouldClose()) {
		g_screenHeight = GetScreenHeight();
		g_screenWidth  = GetScreenWidth();

		changeVisualStyle(&prevActiveVisualStyle, activeVisualStyle);
		
		aX.l1 = leftPad; aX.l2 = leftPad;
		aY.s1 =  topPad; aY.s2 =  topPad;

		Rectangle costFunctionRec = { aX.l1, aY.s1, 0.55 * g_screenWidth, 0.26 * g_screenHeight }; // GroupBox: costFunction
		aY.s1 += costFunctionRec.height + innerLayerPad;
		aX.l1 += costFunctionRec.width + layerPad;
		Rectangle neuralNetworkRec = { aX.l2, aY.s1, 0.55 * g_screenWidth, g_screenHeight - (aY.s1 + bottomPad) }; //DummyRec: neuralNetwork
		aY.s1 += neuralNetworkRec.height + innerLayerPad;
		aX.l2 += neuralNetworkRec.width + layerPad;
		
		{
			size_t width = g_screenWidth - (aX.l1 + rightPad);
			size_t height= fmax(0.26 * g_screenHeight, 156);
			controlPanelGroup.boundingBox = (Rectangle) { aX.l1, aY.s2, width, height }; // GroupBox: controlPanel
		}

		aY.s2 += controlPanelGroup.boundingBox.height + innerLayerPad;
		Rectangle testResultRec = { aX.l2, aY.s2, g_screenWidth - (aX.l2 + rightPad), g_screenHeight - (aY.s2 + bottomPad) }; //GroupBox: provjeraRezultata


		updateControlPanelGroup(&controlPanelGroup, layerPad);
		Rectangle testResultTypeRec;
		{
			size_t sX = testResultRec.x + 2*layerPad;
			size_t sY = testResultRec.y + innerLayerPad;
			testResultTypeRec = (Rectangle) { sX, sY,  72,  24 }; // ToggleGroup: nacinProvjereRezultata
		}


		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		GuiLabel((Rectangle){ leftPad, 10, 60, 24 }, "Style:");
            	GuiComboBox((Rectangle){ leftPad + 42 ,10, 120, 24 }, stylesPossibleText, &activeVisualStyle);

		// Draw controls
		GuiDummyRec(    neuralNetworkRec,  neuralNetworkText);
		GuiGroupBox(    costFunctionRec,  costFunctionText);
		GuiGroupBox(    testResultRec,  provjeraRezultataText);
		drawControlPanelGroup(&controlPanelGroup);

		// U prvom je NULL, a u drugom "" zato da ne ispisuje istu sliku dva puta:
		// To je problem do izvedbe funkcije te je ovo korisni "hack" da u istom redu ispiše dvije razlicite slike
		GuiToggleGroup( testResultTypeRec,  
			TextFormat("%sTEKST;%sSLIKA", 
			  GuiIconText(ICON_FILETYPE_TEXT, NULL), 
			  GuiIconText(ICON_FILETYPE_IMAGE, "")), 
			&nacinProvjereRezultataActive);



		EndDrawing();
	}

	CloseWindow();
	return 0;
}

ControlPanelGroup initControlPanelGroup(void) {
	ControlPanelGroup cpg = {
		.controlPanelText                = "Kontrole",                   // GROUPBOX:    controlPanel
		.layerChoiceText                 = "ONE;TWO;THREE",              // TOGGLEGROUP: layerChoice
		.activationFunctionChoiceText    = "SIGMOID;RE:LU;TANH;SIN",     // COMBOBOX:    activationFunctionChoice
		.learnRateText                   = "Learn rate",                 // SLIDERBAR:   learnRate
		.layerLabelText                  = "Layer",                      // LABEL:       layerLabel
		.activationLabelText             = "Activation",                 // LABEL:       activationLabel
		.numberOfNeuronsText             = "Number of N "                // SPINNER:     numberOfNeurons
	};
	
	cpg.layerChoiceCurrent              = 0;            // ToggleGroup: layerChoice
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

	cpg->layerL 		 = (Rectangle){ sX - 42,  sY,  40,  24 }; // Label: layerLabel
	cpg->activationL 	 = (Rectangle){ sX - 65,  sY + 32,  56,  24 }; // Label: activationLabel
	cpg->layerChoiceTG  	 = (Rectangle){ sX,  sY, controlsWidth/3.0, 24 }; sY += 32; // ToggleGroup: layerChoice
	cpg->activationChoiceCB  = (Rectangle){ sX, sY, controlsWidth,      24 }; sY += 32; // ComboBox: activationFunctionChoice
	cpg->nOfNeuronsS  	 = (Rectangle){ sX, sY, controlsWidth,      24 }; sY += 32; // Spinner: numberOfNeurons
	cpg->learnRateSB  	 = (Rectangle){ sX, sY, controlsWidth,      16 }; sY += 32; // SliderBar: learnRate

	return;
}

void drawControlPanelGroup(ControlPanelGroup* cpg) {

	GuiGroupBox(    cpg->boundingBox,  cpg->controlPanelText);
	GuiToggleGroup( cpg->layerChoiceTG,  cpg->layerChoiceText, &cpg->layerChoiceCurrent);
	GuiComboBox(    cpg->activationChoiceCB,  cpg->activationFunctionChoiceText, &cpg->activationFunctionChoiceCurrent);
	if (GuiSpinner( cpg->nOfNeuronsS, cpg->numberOfNeuronsText, &cpg->nOfNeuronsValue, 0, 100, cpg->nOfNeuronsEditMode)) cpg->nOfNeuronsEditMode = !cpg->nOfNeuronsEditMode;
	GuiSliderBar(   cpg->learnRateSB,  cpg->learnRateText, NULL, &cpg->learnRateValue, 0, 100);
	GuiLabel(       cpg->layerL, cpg->layerLabelText);
	GuiLabel(       cpg->activationL, cpg->activationLabelText);
}


void changeVisualStyle(int* prevVisualStyle, const int visualStyle) {
	if (visualStyle != *prevVisualStyle)
        {
            GuiLoadStyleDefault();

            switch (visualStyle)
            {
                case  0: break;      // Default style
                case  1: GuiLoadStyleJungle(); break;
                case  2: GuiLoadStyleLavanda(); break;
                case  3: GuiLoadStyleDark(); break;
		case  4: GuiLoadStyleAshes(); break;
                case  5: GuiLoadStyleBluish(); break;
                case  6: GuiLoadStyleCyber(); break;
                case  7: GuiLoadStyleTerminal(); break;
		case  8: GuiLoadStyleEnefete(); break;
		case  9: GuiLoadStyleSunny(); break;
		case 10: GuiLoadStyleCherry(); break;
		case 11: GuiLoadStyleCandy(); break;
                default: break;
            }

            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);

            *prevVisualStyle = visualStyle;
        }
}
