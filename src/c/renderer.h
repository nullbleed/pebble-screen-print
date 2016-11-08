#pragma once
#include "constants.h"
#include "callbacks.h"

// font
extern FFont *s_render_font;

int get_font_offset(char*);
void render_text(GContext*, char*, int, int, int, GTextAlignment);
void time_render_proc(Layer*, GContext*); 
void battery_render_proc(Layer*, GContext*); 
void date_render_proc(Layer*, GContext*); 
void temp_render_proc(Layer*, GContext*); 
void steps_render_proc(Layer*, GContext*); 
void init_proc(Layer*, GContext*); 
