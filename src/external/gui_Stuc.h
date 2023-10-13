/*******************************************************************************************
*
*   Stuc v1.0.2 - Program za kreiranje neuronskih mreza
*
*   MODULE USAGE:
*       #define GUI_STUC_IMPLEMENTATION
*       #include "gui_Stuc.h"
*
*       INIT: GuiStucState state = InitGuiStuc();
*       DRAW: GuiStuc(&state);
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 Mateo Kos. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <string.h>     // Required for: strcpy()

#ifndef GUI_STUC_H
#define GUI_STUC_H

typedef struct {
    // Define anchors
    Vector2 screenSize;            // ANCHOR ID:1
    
    // Define controls variables
    bool InitWindowActive;            // WindowBox: InitWindow
    int nacinProvjereRezultataActive;            // ToggleGroup: nacinProvjereRezultata
    int layerChoiceActive;            // ToggleGroup: layerChoice
    int activationFunctionChoiceActive;            // ComboBox: activationFunctionChoice
    bool numberOfNeuronsEditMode;
    int numberOfNeuronsValue;            // Spinner: numberOfNeurons
    float learnRateValue;            // SliderBar: learnRate

    // Define rectangles
    Rectangle layoutRecs[12];

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

} GuiStucState;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiStucState InitGuiStuc(void);
void GuiStuc(GuiStucState *state);


#ifdef __cplusplus
}
#endif

#endif // GUI_STUC_H

/***********************************************************************************
*
*   GUI_STUC IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_STUC_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiStucState InitGuiStuc(void)
{
    GuiStucState state = { 0 };

    // Init anchors
    state.screenSize = (Vector2){ 0, 0 };            // ANCHOR ID:1
    
    // Initilize controls variables
    state.nacinProvjereRezultataActive   = 0;            // ToggleGroup: nacinProvjereRezultata
    state.layerChoiceActive              = 0;            // ToggleGroup: layerChoice
    state.activationFunctionChoiceActive = 0;            // ComboBox: activationFunctionChoice
    state.numberOfNeuronsEditMode        = false;        // Spinner: numberOfNeurons
    state.numberOfNeuronsValue           = 0;            // Spinner: numberOfNeurons
    state.learnRateValue                 = 0.0f;         // SliderBar: learnRate

    // Init controls rectangles
    state.layoutRecs[0]  = (Rectangle){ state.screenSize.x +   0, state.screenSize.y +   0, 760, 528 }; // WindowBox: InitWindow
    state.layoutRecs[1]  = (Rectangle){ state.screenSize.x +  24, state.screenSize.y + 224, 440, 280 }; // DummyRec: neuralNetwork
    state.layoutRecs[2]  = (Rectangle){ state.screenSize.x +  24, state.screenSize.y +  48, 440, 160 }; // GroupBox: costFunction
    state.layoutRecs[3]  = (Rectangle){ state.screenSize.x + 472, state.screenSize.y +  48, 248, 160 }; // GroupBox: controlPanel
    state.layoutRecs[4]  = (Rectangle){ state.screenSize.x + 472, state.screenSize.y + 224, 248, 280 }; // GroupBox: provjeraRezultata
    state.layoutRecs[5]  = (Rectangle){ state.screenSize.x + 488, state.screenSize.y + 240,  72,  24 }; // ToggleGroup: nacinProvjereRezultata
    state.layoutRecs[6]  = (Rectangle){ state.screenSize.x + 544, state.screenSize.y +  80,  48,  24 }; // ToggleGroup: layerChoice
    state.layoutRecs[7]  = (Rectangle){ state.screenSize.x + 544, state.screenSize.y + 112, 144,  24 }; // ComboBox: activationFunctionChoice
    state.layoutRecs[8]  = (Rectangle){ state.screenSize.x + 544, state.screenSize.y + 144, 144,  24 }; // Spinner: numberOfNeurons
    state.layoutRecs[9]  = (Rectangle){ state.screenSize.x + 544, state.screenSize.y + 176, 144,  16 }; // SliderBar: learnRate
    state.layoutRecs[10] = (Rectangle){ state.screenSize.x + 504, state.screenSize.y +  80,  40,  24 }; // Label: layerLabel
    state.layoutRecs[11] = (Rectangle){ state.screenSize.x + 488, state.screenSize.y + 112,  56,  24 }; // Label: activationLabel

    // Custom variables initialization

    return state;
}


void GuiStuc(GuiStucState *state)
{
    // Const text
    const char *neuralNetworkText               = "Prikaz Neuronske mreze";     // DUMMYREC:    neuralNetwork
    const char *costFunctionText                = "Graf tocnosti meze";         // GROUPBOX:    costFunction
    const char *controlPanelText                = "Kontrole";                   // GROUPBOX:    controlPanel
    const char *provjeraRezultataText           = "Provjera rezultata";         // GROUPBOX:    provjeraRezultata
    const char *nacinProvjereRezultataText      = "TEKST;SLIKE";                // TOGGLEGROUP: nacinProvjereRezultata
    const char *layerChoiceText                 = "ONE;TWO;THREE";              // TOGGLEGROUP: layerChoice
    const char *activationFunctionChoiceText    = "SIGMOID;RE:LU;TANH;SIN";     // COMBOBOX:    activationFunctionChoice
    const char *learnRateText                   = "Learn rate";                 // SLIDERBAR:   learnRate
    const char *layerLabelText                  = "Layer";                      // LABEL:       layerLabel
    const char *activationLabelText             = "Activation";                 // LABEL:       activationLabel
    const char *numberOfNeuronsText             = "Number of N ";               // SPINNER:     numberOfNeurons
    
    // Draw controls
   DrawRectangleRec(state->layoutRecs[0],  GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    GuiDummyRec(    state->layoutRecs[1],  neuralNetworkText);
    GuiGroupBox(    state->layoutRecs[2],  costFunctionText);
    GuiGroupBox(    state->layoutRecs[3],  controlPanelText);
    GuiGroupBox(    state->layoutRecs[4],  provjeraRezultataText);
    GuiToggleGroup( state->layoutRecs[5],  nacinProvjereRezultataText, &state->nacinProvjereRezultataActive);
    GuiToggleGroup( state->layoutRecs[6],  layerChoiceText, &state->layerChoiceActive);
    GuiComboBox(    state->layoutRecs[7],  activationFunctionChoiceText, &state->activationFunctionChoiceActive);
    if (GuiSpinner( state->layoutRecs[8],  numberOfNeuronsText, &state->numberOfNeuronsValue, 0, 100, state->numberOfNeuronsEditMode)) state->numberOfNeuronsEditMode = !state->numberOfNeuronsEditMode;
    GuiSliderBar(   state->layoutRecs[9],  learnRateText, NULL, &state->learnRateValue, 0, 100);
    GuiLabel(       state->layoutRecs[10], layerLabelText);
    GuiLabel(       state->layoutRecs[11], activationLabelText);
}

#endif // GUI_STUC_IMPLEMENTATION
