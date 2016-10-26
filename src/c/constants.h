#pragma once
#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>

// persistent storage key
#define SETTINGS_KEY 1

// define our settings struct
typedef struct ClaySettings {
    GColor BackgroundColor;
    GColor ForegroundColor;
    bool SecondTick;
    bool Animations;
    bool VibrateOnDisconnect;
    bool ShakeForSteps;
} ClaySettings;

// an instance of the struct
extern ClaySettings settings;

// colors
extern GColor s_background_color;
extern GColor s_foreground_color;

// temperature offset
extern int s_temp_offset;

// states
extern int s_battery_level;
extern bool s_connection_state;

// layers
extern Window *s_main_window;

extern Layer *s_background_layer;
extern Layer *s_time_render_layer;
extern Layer *s_battery_render_layer;
extern Layer *s_date_render_layer;
extern Layer *s_temp_render_layer;

extern BitmapLayer *s_background_wbm_layer;
extern GBitmap *s_background_bitmap;

extern BitmapLayer *s_background_bm_layer;
extern GBitmap *s_white_bitmap;
