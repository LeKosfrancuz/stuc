#ifndef _SCENE_TDATA_H
#define _SCENE_TDATA_H

#include "external/raylib.h"
#include "scene_essentials.h"


typedef struct {
    struct {
        float_t val;
        bool edit_mode;
    }*items;
    size_t count;
    size_t capacity;
} Sample;

typedef struct {
    Sample *items;
    size_t count;
    size_t capacity;

    size_t ins;
    size_t outs;
} tData_samples_t;

typedef struct {
    tData_samples_t td;
    size_t top_pad;
    size_t layer_pad;
    size_t l_pad;
    size_t r_pad;

    /* Top toolbar */
    Rectangle top_tools_rect;

    char* input_spinner_text;
    bool edit_input_spinner;
    Rectangle input_spinner_rect;
    int input_val;

    char* output_spinner_text;
    bool edit_output_spinner;
    Rectangle output_spinner_rect;
    int output_val;

    /* Sample data */
    Rectangle sample_area_rect;
    float sample_height;
    float sample_scroll;

    /* Bottom toolbar */
    Rectangle bot_tools_rect;

    Rectangle add_sample_btn;
    Rectangle move_data_to_builder_btn;
    char *move_data_btn_text;
} Scene_tData;

Scene_tData scene_tDataInit(void);
void scene_tDataDraw(Scene_tData *s);
void scene_tDataUpdate(Scene_tData *s);


#endif // _SCENE_TDATA_H
