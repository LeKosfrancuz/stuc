#ifndef _SCENE_ESSENTIALS_H
#define _SCENE_ESSENTIALS_H

#include "../stuc/stuc.h"
#include "external/raylib.h"
#include <string.h>
#include "external/raygui.h"
#include "dynamic-arrays.h"
#include "time.h"

extern size_t g_screenWidth;
extern size_t g_screenHeight;

#define INFO  "[\x1b[0;36mINFO\x1b[0m]: "
#define WARN  "[\x1b[0;38;5;208mWARNING\x1b[0m]: "
#define ERROR "[\x1b[0;31mERROR\x1b[0m]: "
#define log(level, ...)           printf(level); printf(__VA_ARGS__);

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

#endif // _SCENE_ESSENTIALS_H
