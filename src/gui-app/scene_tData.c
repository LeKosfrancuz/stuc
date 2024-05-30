#include "scene_tData.h"
#include "dynamic-arrays.h"
#include "external/raygui.h"
#include "external/raylib.h"
#include "scene_essentials.h"
#include "scene_nnBuilder.h"
#include <float.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

int GuiFloatBox(Rectangle bounds, const char* text, float* value, int minValue, int maxValue, bool editMode);

float *tData = NULL;
size_t tData_samples = 0;

#define SET_FONT_SIZE(size) GuiSetStyle(DEFAULT, TEXT_SIZE, fmax(0, (int)(size)))
#define CURR_FONT_SIZE GuiGetStyle(DEFAULT, TEXT_SIZE)

int best_font_size(char *text, float width, float height) {
	int curr_size = CURR_FONT_SIZE;
	const float lower_limit = 4.0/10.0;
	const float middle_limit = 9.0/10.0;
	const float upper_limit = 2.0;

	for (size_t __iter = 0; __iter < 20; __iter++) {

		Vector2 txt_size = MeasureTextEx(GuiGetFont(), text, curr_size, GuiGetStyle(DEFAULT, TEXT_SPACING));
		if (txt_size.x > upper_limit*width) curr_size -= 4;
		else if (txt_size.x > width) curr_size--;
		else if (txt_size.x < lower_limit*width) curr_size += 4;
		else if (txt_size.x < middle_limit*width) curr_size++;

		if (txt_size.y > upper_limit*height) curr_size -= 4;
		else if (txt_size.y > height) curr_size--;
	}

	return curr_size;
}

Scene_tData scene_tDataInit(void) {
	Scene_tData s = {0};
	s.td = (tData_samples_t){0};
	s.top_pad = 48;
	s.l_pad = 24;
	s.r_pad = 24;
	s.layer_pad = 8;
	s.input_spinner_text = "ULAZI:";
	s.output_spinner_text = "IZLAZI:";
	s.move_data_btn_text = "Save training Data";
	s.td.ins = 1;
	s.td.outs = 1;
	return s;
}

Vector2 scroll;
bool error;
void scene_tDataDraw(Scene_tData *s) {
	// DrawRectangleRec(s->top_tools_rect, MAROON);
	int size = CURR_FONT_SIZE;
	SET_FONT_SIZE(best_font_size(s->input_spinner_text, s->input_spinner_rect.width, s->input_spinner_rect.height));

	if (GuiSpinner(s->input_spinner_rect, s->input_spinner_text, &s->input_val, 1, MAX_N_OF_NEURONS, s->edit_input_spinner)) {
		s->edit_input_spinner = !s->edit_input_spinner;
	};

	SET_FONT_SIZE(size);

	size = CURR_FONT_SIZE;
	SET_FONT_SIZE(best_font_size(s->output_spinner_text, s->output_spinner_rect.width, s->output_spinner_rect.height));

	if (GuiSpinner(s->output_spinner_rect, s->output_spinner_text, &s->output_val, 1, MAX_N_OF_NEURONS, s->edit_output_spinner)) {
		s->edit_output_spinner = !s->edit_output_spinner;
	};

	if (s->td.ins != (size_t)s->input_val || s->td.outs != (size_t)s->output_val) {
		for (size_t i = 0; i < s->td.count; i++) {
			if (s->td.ins + s->td.outs > (size_t)(s->input_val + s->output_val)) {
				s->td.items[i].count = s->input_val + s->output_val;
			} else {
				da_append_many(&s->td.items[i], NULL, s->input_val + s->output_val - s->td.ins - s->td.outs);
			}
		}

		s->td.ins = (size_t)s->input_val;
		s->td.outs = (size_t)s->output_val;
	}

	SET_FONT_SIZE(size);

	Rectangle bounds = {
		s->sample_area_rect.x - 7,
		s->sample_area_rect.y,
		s->sample_area_rect.width + 14,
		fmin(s->sample_area_rect.height, 4*s->sample_height)
	};
	Rectangle view = bounds;
	if (bounds.height < s->sample_area_rect.height) {
		GuiScrollPanel(bounds, "Sample data", s->sample_area_rect, &scroll, &view);
	}
	BeginScissorMode(
		view.x,
		view.y,
		view.width,
		view.height
	);
		// DrawRectangleRec(s->sample_area_rect, MAROON);
		for (size_t i = 0; i < s->td.count; i++) {
			size_t sample_cnt = s->td.items[i].count;
			Rectangle samples = {
				view.x + s->l_pad,
				view.y + (s->sample_height+ s->layer_pad)*i + scroll.y,
				view.width - s->l_pad - s->r_pad,
				s->sample_height
			};

			if (sample_cnt == 0) {
				da_append_many(&s->td.items[i], NULL, s->input_val + s->output_val);
			}
			for (size_t j = 0; j < sample_cnt; j++) {
				__auto_type item = s->td.items[i].items[j];

				Rectangle sample = {
					samples.x + j*(samples.width/sample_cnt) + (float)s->l_pad/sample_cnt,
					samples.y,
                                        samples.width / sample_cnt - (float)(s->l_pad + s->r_pad)/sample_cnt,
					samples.height,
				};

				float temp = item.val;
				int decimals = 0;
				do {
					temp -= (int)temp;

				        if (temp > 0) decimals++;

					temp *= 10;
			        } while(temp > 0 && decimals < 7);

				char txt[128];
				snprintf(txt, 128, "%.*f", decimals, item.val);
				int font_size = best_font_size(txt, sample.width, sample.height);
				if (GuiTextBox(sample, txt, font_size, item.edit_mode)) {
					s->td.items[i].items[j].edit_mode = !item.edit_mode;
				}
				if(sscanf(txt, "%f ", &s->td.items[i].items[j].val) != 1) {
					if (errno == 0) errno = EINVAL;
					log(ERROR, "Greška pri čitanju broja opcije ulaza %zu retka %zu : %s", j, i, strerror(errno));
				}

				DrawLineEx(
					(Vector2) {samples.x + s->td.ins*(samples.width/sample_cnt), view.y},
					(Vector2) {samples.x + s->td.ins*(samples.width/sample_cnt), view.y + view.height},
					2,
					SC_PRESS_TEXT
			        );
			}

			size_t btn_width = samples.height/2;
			Rectangle button_rec = {view.x, samples.y + btn_width/2.0, btn_width, btn_width};
			if (GuiButton(button_rec, ICON_TO_TEXT(ICON_BIN))) {
				da_remove(&s->td, i);
			}

		}
	EndScissorMode();


	GuiEnableTooltip();
	GuiSetTooltip("Add sample");

	size = CURR_FONT_SIZE;
	SET_FONT_SIZE(best_font_size("BTN", s->add_sample_btn.width, s->add_sample_btn.height));

	if (GuiButton(s->add_sample_btn, ICON_TO_TEXT(ICON_LAYERS))) {
		da_append(&s->td, (Sample){0});
	};

	SET_FONT_SIZE(size);

	GuiDisableTooltip();


	GuiEnableTooltip();
	GuiSetTooltip("Moves data to nnBuilder scene as input data");

	size = CURR_FONT_SIZE;
	SET_FONT_SIZE(best_font_size(s->move_data_btn_text, s->move_data_to_builder_btn.width, s->move_data_to_builder_btn.height));

	if (GuiButton(s->move_data_to_builder_btn, s->move_data_btn_text)) {
		free(tData);
		tData_samples = 0;

		if (s->td.count == 0) {
			error = true;
		} else {
			tData = malloc((s->td.count*(s->td.ins + s->td.outs))*sizeof(float_t));
			tData_samples = s->td.count;

			for (size_t i = 0; i < s->td.count; i++) {
				for (size_t j = 0; j < s->td.items[i].count; j++) {
					tData[i*s->td.items[i].count+j] = s->td.items[i].items[j].val;
				}
			}
		}
	};

	SET_FONT_SIZE(size);

	GuiDisableTooltip();


	if (error) {
		int ret = GuiMessageBox(
			(Rectangle){
				g_screenWidth/2.0 - 0.15*g_screenWidth,
				g_screenHeight/2.0 - 0.10*g_screenHeight,
				0.30*g_screenWidth,
			        0.20*g_screenHeight
			},
			"Operacija prekinuta!", "Potrebno je dodati ulazne podatke", "OK"
		);
		if (ret != -1) error = false;
	}

	return;
}

void scene_tDataUpdate(Scene_tData *s) {
	s->l_pad = g_screenWidth / 25;
	s->r_pad = g_screenWidth / 25;
	s->layer_pad = g_screenHeight / 50;

	s->top_tools_rect = (Rectangle) {s->l_pad, s->top_pad, g_screenWidth - s->l_pad - s->r_pad, g_screenHeight*0.12};
	s->input_spinner_rect = (Rectangle){
		s->top_tools_rect.x + s->top_tools_rect.width/4,
		s->top_tools_rect.y,
		0.25*s->top_tools_rect.width - s->r_pad,
		s->top_tools_rect.height
	};

	s->output_spinner_rect = (Rectangle){
		s->top_tools_rect.x + 3*s->top_tools_rect.width/4,
		s->top_tools_rect.y,
		0.25*s->top_tools_rect.width - s->r_pad,
		s->top_tools_rect.height
	};

	s->sample_height = 0.12*g_screenHeight;
	s->sample_area_rect = (Rectangle) {
		s->l_pad,
		s->top_tools_rect.y + s->top_tools_rect.height + s->layer_pad,
		g_screenWidth - s->l_pad - s->r_pad,
		(s->sample_height + s->layer_pad) * s->td.count,
	};

	s->bot_tools_rect = (Rectangle) {
		s->l_pad,
		s->sample_area_rect.y + fmin(s->sample_area_rect.height, 4*s->sample_height) + s->layer_pad,
		g_screenWidth - s->l_pad - s->r_pad,
		g_screenHeight*0.12,
	};

	s->add_sample_btn = (Rectangle) {
		s->bot_tools_rect.x + 2*s->bot_tools_rect.width/5 - s->add_sample_btn.width/2,
		s->bot_tools_rect.y,
		s->bot_tools_rect.height,
		s->add_sample_btn.width,
	};

	s->move_data_to_builder_btn = (Rectangle) {
		s->bot_tools_rect.x + 3*s->bot_tools_rect.width/5 - s->add_sample_btn.width/2,
		s->bot_tools_rect.y,
		2*s->bot_tools_rect.width/5,
		s->bot_tools_rect.height,
	};
	return;
}


#undef SET_FONT_SIZE


