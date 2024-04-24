#ifndef _SCENE_IMAGE_RECOGNISER_H
#define _SCENE_IMAGE_RECOGNISER_H

#include "scene_essentials.h"

typedef struct {
	Image *items;
	size_t count;
	size_t capacity;
} ImageList;

typedef struct {
	ImageList images;
	char   *cat_name;
} Category;

typedef struct {
	Category *items;
	size_t    count;
	size_t capacity;
} Categories;

typedef struct {
    Categories cats;
    Stuc_nn nn;
    Stuc_nn gd;
    Stuc_mat tInput;
    Stuc_mat tOutput;
    Image canvas;
    Texture canvas_texture;
    Stuc_mat batch_input;
    Stuc_mat batch_output;
} Scene_imageRecog;

Scene_imageRecog scene_imageRecogInit(void);
void scene_imageRecogUpdate(Scene_imageRecog *s);
void scene_imageRecogDraw(Scene_imageRecog *s);
int parseTrainData(Categories *cats);
void randomiseInputData(Scene_imageRecog *s);

// NeuralNetworkPreview initNeuralNetworkPreview(void);
// void updateNeuralNetworkPreview(NeuralNetworkPreview *nnp, ControlPanelGroup *cpg);
// void drawNeuralNetworkPreview(NeuralNetworkPreview *nnp, ControlPanelGroup *cpg);

#endif // _SCENE_IMAGE_RECOGNISER_H
