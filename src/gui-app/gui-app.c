#include "../stuc/stuc.h"
#include "external/raylib.h"
#include <string.h>
#include "scene_essentials.h"

enum {
	SC_NN_BUILDER,
	SC_IMAGE_RECOGNISER,
	SC_COUNT
} Scenes;
#include "scene_nnBuilder.h"
#include "scene_imageRecogniser.h"

#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"
#include "dynamic-arrays.h"


#define VISUAL_STYLES_COUNT 12
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
void changeVisualStyle(int *prevStyle, const int crntStyle);

size_t g_screenWidth = 800;
size_t g_screenHeight = 600;

void scene_demoDraw(void) {
	DrawRectangle(50, 50, 100, 100, MAROON);

	return;
}

int main(void) {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(g_screenWidth, g_screenHeight, "Stuc");
	Image window_icon = LoadImage(RESOURCES_PATH "window_icon.png");
	ImageFormat(&window_icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
	SetWindowIcon(window_icon);

	// Load default style
    	GuiLoadStyleBluish();
    	int activeVisualStyle = 5;
    	int prevActiveVisualStyle = 5;

	int activeScene = SC_NN_BUILDER;

	size_t leftPad   = 24;
	size_t layerPad  = 8;

	SetTargetFPS(60);

	const char* styleChooserText 		    = "default;Jungle;Lavanda;Dark;Ashes;Bluish;Cyber;Terminal;Enefete;Sunny;Cherry;Candy"; // STYLE CHOOSER
	const char* sceneChooserText 		    = "Neural Network Builder;Image Recogniser"; // STYLE CHOOSER

	Scene_nnBuilder scene_nnBuilder = scene_nnBuilderInit();
	Scene_imageRecog scene_imageRecog = scene_imageRecogInit();

	while (!WindowShouldClose()) {
		g_screenHeight = (size_t) GetScreenHeight();
		g_screenWidth  = (size_t) GetScreenWidth();

		changeVisualStyle(&prevActiveVisualStyle, activeVisualStyle);
		
		switch (activeScene) {
		case SC_NN_BUILDER: scene_nnBuilderUpdate(&scene_nnBuilder); break;
		case SC_IMAGE_RECOGNISER: scene_imageRecogUpdate(&scene_imageRecog); break;
		default: log(ERROR, "Unknown scene %d\n", activeScene);
		}
		

		BeginDrawing();
			ClearBackground(SC_BACKGROUND);

			{
				GuiLabel((Rectangle){ leftPad, 10, (int)(g_screenWidth / 10), 24 }, "Style:");
				Rectangle styleChooserRec = { leftPad + 42 ,10, 120, 24 };
				GuiComboBox(styleChooserRec, styleChooserText, &activeVisualStyle);
				if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && CheckCollisionPointRec(GetMousePosition(), styleChooserRec)) {
					activeVisualStyle = ((activeVisualStyle + VISUAL_STYLES_COUNT) - 1) % VISUAL_STYLES_COUNT;
				}
			}

			{
				const char* text = "Scene:";
				Vector2 txtM = MeasureTextEx(GuiGetFont(), text, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING));

				size_t width = 180;
				Rectangle sceneChooserRec = { g_screenWidth - leftPad - width ,10, width, 24 };

				GuiLabel((Rectangle)
					{ 
					  g_screenWidth - leftPad - layerPad - sceneChooserRec.width - txtM.x, 
					  10, 
					  (int)(g_screenWidth / 10), 
					  24 
					}
				, text);
				GuiComboBox(sceneChooserRec, sceneChooserText, &activeScene);
				if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && CheckCollisionPointRec(GetMousePosition(), sceneChooserRec)) {
					activeScene = ((activeScene + SC_COUNT) - 1) % SC_COUNT;
				}
			}

			switch (activeScene) {
			case SC_NN_BUILDER: scene_nnBuilderDraw(&scene_nnBuilder); break;
			case SC_IMAGE_RECOGNISER: scene_imageRecogDraw(&scene_imageRecog); break;
			default: log(ERROR, "Unknown scene %d\n", activeScene);
			} 

		EndDrawing();
	}

	CloseWindow();
	return 0;
}


void changeVisualStyle(int *prevVisualStyle, const int visualStyle) {
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

