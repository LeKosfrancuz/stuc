#ifndef _SCENE_NNBUILDER_H
#define _SCENE_NNBUILDER_H

#include "scene_essentials.h"
#include "dirent.h"

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
	Rectangle removeLayerBT;

	bool removeCurrLayer;		      // Button:	removeLayer
	bool  nOfNeuronsEditMode;             // Spinner: 	nOfNeurons
	int   layerChoiceCurrent;             // ToggleGroup: 	layerChoice
	int layerSelectedCurrent;	      // current layer actualy shown
	int   activationFunctionChoiceCurrent;// ComboBox: 	activationChoice
	bool layerChoiceEditMode;
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

typedef struct {
	Rectangle boundingBox;
	Rectangle resultsRect;
	// TODO: prevedi na engleski
	const char *provjeraRezultataText;        // GROUPBOX:    provjeraRezultata
	int nacinProvjereRezultataActive;
} CheckResultGroup;

typedef struct {
	ControlPanelGroup controlPanelGroup;
	CostFunctionPanelGroup costPanelGroup;
	NeuralNetworkPreview neuralNetworkPreview;
	CheckResultGroup checkResultGroup;

	struct allignX { size_t l1,l2; } aX;
	struct allignY { size_t s1,s2; } aY;
	size_t leftPad;
	size_t rightPad;
	size_t topPad;
	size_t bottomPad;
	size_t layerPad;
	size_t innerLayerPad;
} Scene_nnBuilder;

Scene_nnBuilder scene_nnBuilderInit(void);
void scene_nnBuilderUpdate(Scene_nnBuilder *s);
void scene_nnBuilderDraw(Scene_nnBuilder *s);

ControlPanelGroup initControlPanelGroup(void);
void updateControlPanelGroup(ControlPanelGroup *cpg, size_t xPad);
void drawControlPanelGroup(ControlPanelGroup *cpg);

CostFunctionPanelGroup initCostFunctionPanelGroup(void);
void updateCostFunctionPanelGroup(CostFunctionPanelGroup *cfpg, size_t topPad, size_t expandButtonWidth);
void drawCostFunctionPanelGroup(CostFunctionPanelGroup *cfpg);

NeuralNetworkPreview initNeuralNetworkPreview(void);
void updateNeuralNetworkPreview(NeuralNetworkPreview *nnp, ControlPanelGroup *cpg);
void drawNeuralNetworkPreview(NeuralNetworkPreview *nnp, ControlPanelGroup *cpg);

CheckResultGroup initCheckResultGroup(void);
void updateCheckResultGroup(CheckResultGroup *crg, size_t layerPad, size_t innerLayerPad);
void drawCheckResultGroup(CheckResultGroup *crg);

#endif // _SCENE_NNBUILDER_H
