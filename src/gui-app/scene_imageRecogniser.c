#include "scene_imageRecogniser.h"
#include "dynamic-arrays.h"
#include "external/raylib.h"
#include "external/raylib/raylib.h"
#include "scene_essentials.h"
#include "scene_nnBuilder.h"
#include <stdint.h>
#include <string.h>
#include "dirent.h"
#include "errno.h"
#include "time.h"
#include "stdlib.h"
#include "external/raylib/external/stb_image.h"
#define img_w 28

#define MODEL_DATA_PATH "./trainedNNs/image-recog/"

#ifdef TRAINING
#include "rlgl.h"
static size_t counter = 0;
static size_t batch_size = 16;
static float  learn_rate = 1e-4;
static float  learn_boost = 1e-3;
#endif


static bool paused = false;

Scene_imageRecog scene_imageRecogInit(void) {
	Scene_imageRecog s;
	s.canvas = GenImageColor(500, 500, WHITE);
	uint8_t ret = stuc_nnLoadFromFile(&s.nn, MODEL_DATA_PATH "ImageRecog.snn");
	if (ret) stuc_printIOFlags(ret);


#ifdef TRAINING
	srand(14);

	Categories cats = {0};
	int number_of_samples = parseTrainData(&cats);
	const int real_img_w = cats.items[0].images.items[0].width;
	if (real_img_w != img_w) {
		log(ERROR, "Slike nisu dimenzija %dx%d već %dx%d",
			      img_w, img_w, real_img_w, cats.items[0].images.items[0].height);
		// TODO: resizing
		exit(EXIT_FAILURE);
	}

	s.cats = cats;

	size_t arh[] = {img_w*img_w, 277, 64, 47, 43, cats.count};
	Stuc_activationFunction act[STUC_LENP(arh)-1] = {STUC_ACTIVATE_RELU, STUC_ACTIVATE_RELU, STUC_ACTIVATE_RELU, STUC_ACTIVATE_RELU, STUC_ACTIVATE_SOFTMAX};

	stuc_setActivation(act, STUC_LENP(act), STUC_ACTIVATE_RELU);
	s.nn = stuc_nnAlloc(act, arh, STUC_LENP(arh));
	s.gd = stuc_nnAlloc(act, arh, STUC_LENP(arh));
	stuc_nnRand(s.nn, -1, 10);

	s.tInput = stuc_matAlloc(number_of_samples, img_w*img_w);
	s.tOutput = stuc_matAlloc(number_of_samples, cats.count);
	s.batch_input = stuc_matAlloc(batch_size, s.tInput.cols);
	s.batch_output = stuc_matAlloc(batch_size, s.tOutput.cols);

	randomiseInputData(&s);
#endif
	return s;
}

void randomiseInputData(Scene_imageRecog *s) {
	size_t number_of_samples = s->tInput.rows;
	size_t i = 0;
	for (size_t j = 0; j < s->cats.items[i].images.count; j++) {
		for (; i < s->cats.count; i++) {
			Image img = ImageCopy(s->cats.items[i].images.items[j]);

			// ImageResize(&img, img_w, img_w);
			ImageResizeCanvas(&img, img.width*2, img.height*2, img.width/2.0 + 4.0*rand()/RAND_MAX, img.height/2.0 + 4.0*rand()/RAND_MAX, WHITE);
			ImageRotate(&img, 14*(float)rand()/RAND_MAX);
			ImageColorContrast(&img, 1.5);
			Rectangle crop = {img.width/2.0 - 14, img.height/2.0 - 14, img_w, img_w};
			ImageCrop(&img, crop);

			number_of_samples--;
			uint8_t* data = img.data;
			for (size_t row = 0; row < (size_t)img.height; row++) {
				for (size_t col = 0; col < (size_t)img.width; col++) {
					uint8_t val = *(data+(row*img.width+col));
					STUC_MAT_AT(s->tInput, number_of_samples, row*img.width+col) = (float)val / 255;
				}
			}
			STUC_MAT_AT(s->tOutput, number_of_samples, i) = 1;
		}
		i = 0;
	}
}

void nnForwardImage(Scene_imageRecog *s, Image img) {
	STUC_ASSERT(s->nn.arhitektura[0] == (size_t)img.width*img.height);
	STUC_ASSERT(img.width == img.height);

	// uint8_t* data = img.data;
	Color *colors = LoadImageColors(img);
	for (size_t row = 0; row < (size_t)img.height; row++) {
		for (size_t col = 0; col < (size_t)img.width; col++) {
			Color val = colors[row*img.width+col];
			STUC_AT_INPUT(s->nn, (row*img.width+col)) = (float)val.r /255;
		}
	}

	stuc_nnForward(s->nn);
	UnloadImageColors(colors);
}

void scene_imageRecogUpdate(Scene_imageRecog *s) {

	if (IsKeyPressed(KEY_SPACE)) paused = !paused;

	if (IsKeyPressed(KEY_DELETE) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
		ImageClearBackground(&s->canvas, WHITE);
		s->canvas_texture = LoadTextureFromImage(s->canvas);
	}

#ifdef TRAINING
	if (IsKeyPressed(KEY_LEFT)) learn_rate /= 10;
	if (IsKeyPressed(KEY_RIGHT)) learn_rate *= 10;
#endif
}

void scene_imageRecogDraw(Scene_imageRecog *s) {
	srand(time(0));


	if (paused) {
		DrawText("Paused", 50, 50, 25, MAROON);
		Rectangle canvasRect = {50, 100, s->canvas.width, s->canvas.width};
		DrawTexture(s->canvas_texture, canvasRect.x, canvasRect.y, WHITE);
		return;
	}

	{
		DrawText("Draw", 50, 50, 25, SC_NORML_TEXT);
		Rectangle canvasRect = {50, 100, s->canvas.width, s->canvas.width};

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), canvasRect)) {
			ImageDrawCircle(&s->canvas, GetMousePosition().x - canvasRect.x, GetMousePosition().y - canvasRect.y, 14, BLACK);
			s->canvas_texture = LoadTextureFromImage(s->canvas);
			Image small_img = ImageCopy(s->canvas);
			ImageColorInvert(&small_img);
			ImageResize(&small_img, img_w, img_w);
			nnForwardImage(s, small_img);
			MAT_PRINT(STUC_NN_OUTPUT(s->nn));
			DrawTexture(LoadTextureFromImage(small_img), 600, 100, WHITE);
		}
		DrawTexture(s->canvas_texture, canvasRect.x, canvasRect.y, WHITE);
	}

	// ImageResize(&s->cats.items[0].images.items[2], 500, 500);
	// DrawTexture(LoadTextureFromImage(s->cats.items[0].images.items[2]), 100, 100, WHITE);
}


int parseTrainData(Categories *cats) {
	DIR *dir;
	struct dirent *in_dir;
	char *tData_path = MODEL_DATA_PATH "training_data/";

	if (NULL == (dir = opendir(tData_path)))
    	{
		log(ERROR, "Failed to open input directory: %s\n", strerror(errno));
		return false;
    	}

	while ((in_dir = readdir(dir))) {
		#ifndef WIN32

		if (!strcmp (in_dir->d_name, ".")) continue;
        	if (!strcmp (in_dir->d_name, "..")) continue;

	// s.tInput = stuc_matAlloc(number_of_samples, img_w*img_w);
	// s.tOutput = stuc_matAlloc(number_of_samples, cats.count);
		#endif

		// if (in_dir->d_type != DT_DIR) {
		// 	log(WARN, "Found file %s instead of dir!\n", in_dir->d_name);
		// 	return false;
		// }

		DIR *category_dir;
		struct dirent *in_file;
		char full_path[512];
		snprintf(full_path, 512, "%s%s/", tData_path, in_dir->d_name);

		Category cat = { .cat_name = in_dir->d_name };
		da_append(cats, cat);

		log(INFO, "Opening dir: %s\n", full_path);
		category_dir = opendir(full_path);
		if (NULL == category_dir) {
			log(ERROR, "Failed to open input subdirectory: %s\n", strerror(errno));
			return false;
		}

		while ((in_file = readdir(category_dir))) {
			#ifndef WIN32

			if (!strcmp(in_file->d_name, ".")) continue;
			if (!strcmp(in_file->d_name, "..")) continue;

			#endif

			// if (in_file->d_type != DT_REG) {
			// 	printf("Skiping: %s, not a file!", in_file->d_name);
			// 	continue;
			// }

			char image_path[1024];
			snprintf(image_path, 1024, "%s%s", full_path, in_file->d_name);

			Image in_image = {0};
			in_image.data = stbi_load(image_path, &in_image.width, &in_image.height, NULL, 1);

			if (in_image.data != NULL) {
				in_image.mipmaps = 1;

				in_image.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
			}

			da_append(&cats->items[cats->count-1].images, in_image);
		}
	}

	int number_of_samples = 0;
	for (size_t i = 0; i < cats->count; i++) {
		log(INFO, "Found %zu images in %s\n", cats->items[i].images.count, cats->items[i].cat_name);
		number_of_samples += cats->items[i].images.count;
	}

	return number_of_samples;
}
