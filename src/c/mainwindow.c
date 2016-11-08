#include "mainwindow.h"

// layers
Layer *s_background_layer;
Layer *s_time_render_layer;
Layer *s_battery_render_layer;
Layer *s_date_render_layer;
Layer *s_temp_render_layer;
Layer *s_steps_render_layer;

BitmapLayer *s_background_bm_layer;
GBitmap *s_background_bitmap;

BitmapLayer *s_background_wbm_layer;
GBitmap *s_white_bitmap;

// colors
#if defined(PBL_COLOR)
GColor s_background_color = GColorPictonBlue;
GColor s_foreground_color = GColorOxfordBlue;
#else
GColor s_background_color = GColorWhite;
GColor s_foreground_color = GColorBlack;
#endif

// weather offset
#if defined(PBL_ROUND)
int s_temp_offset = 37;
int s_icon_offset_x = 45;
int s_icon_offset_y = 10;
#elif defined(PBL_RECT)
int s_temp_offset = 53;
int s_icon_offset_x = 45;
int s_icon_offset_y = 12;
#endif

// initialize the different layers when the window is created
void main_window_load(Window *window) {
    // get root layer and boundaries of the watch
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_window = window;

    // set the background color for the root window
    window_set_background_color(s_main_window, s_background_color);

    // set initial weather string
    snprintf(s_temp_num_buffer, sizeof(s_temp_num_buffer), "-");

    // get font from resource
    s_render_font = ffont_create_from_resource(RESOURCE_ID_TEST_FFONT);

    // create the layer for displaying the drawings
    s_draw_degree = true;
    s_background_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
    layer_set_update_proc(s_background_layer, init_proc);

    // time render layer
    s_time_render_layer = layer_create(GRect(0, 40, bounds.size.w, 50));
    layer_set_update_proc(s_time_render_layer, time_render_proc);

    // date render layer
    s_date_render_layer = layer_create(GRect(15, 120, 50, 25));
    layer_set_update_proc(s_date_render_layer, date_render_proc);

    // battery render layer
    s_battery_render_layer = layer_create(GRect(0, 12, bounds.size.w, 25));
    layer_set_update_proc(s_battery_render_layer, battery_render_proc);

    // temperature render layer
    s_temp_render_layer = layer_create(GRect(bounds.size.w - s_temp_offset, 102, 40, 25));
    layer_set_update_proc(s_temp_render_layer, temp_render_proc);

    // steps render layer
    s_steps_render_layer = layer_create(GRect(bounds.size.w - s_temp_offset, 102, 40, 25));
    layer_set_update_proc(s_steps_render_layer, steps_render_proc);

    // create GBitmap
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WEATHER);

    snprintf(s_steps_buffer, sizeof(s_steps_buffer), "-");

    // create BitmapLayer to display the GBitmap
    s_background_bm_layer = bitmap_layer_create(GRect(bounds.size.w / 2 - s_icon_offset_x, bounds.size.h / 2 + s_icon_offset_y, 100, 70));
    bitmap_layer_set_compositing_mode(s_background_bm_layer, GCompOpSet);
    bitmap_layer_set_bitmap(s_background_bm_layer, s_background_bitmap);

#if defined(PBL_COLOR)
    replace_gbitmap_color(GColorBlack, s_foreground_color, s_background_bitmap, s_background_bm_layer);
#endif

    // create GBitmap
    s_white_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WEATHER_WHITE);

    // create BitmapLayer to display the GBitmap
    s_background_wbm_layer = bitmap_layer_create(GRect(bounds.size.w / 2 - s_icon_offset_x, bounds.size.h / 2 + s_icon_offset_y, 100, 70));
    bitmap_layer_set_compositing_mode(s_background_wbm_layer, GCompOpSet);
    bitmap_layer_set_bitmap(s_background_wbm_layer, s_white_bitmap);

    // add the layers to the main window
    layer_add_child(window_layer, s_background_layer);
    layer_add_child(window_layer, s_time_render_layer);
    layer_add_child(window_layer, s_date_render_layer);
    layer_add_child(window_layer, s_battery_render_layer);
    layer_add_child(window_layer, s_temp_render_layer);

    layer_add_child(window_layer, bitmap_layer_get_layer(s_background_wbm_layer));
    layer_add_child(window_layer, bitmap_layer_get_layer(s_background_bm_layer));

    //NOTE: remove this when implemented
    snprintf(s_temp_num_buffer, sizeof(s_temp_num_buffer), s_temp_num_buffer);
    snprintf(s_temp_unit_buffer, sizeof(s_temp_num_buffer), "C");

    prv_update_display();
}

// free layers on destroy
void main_window_unload(Window *window) {
    layer_destroy(s_background_layer);
    bitmap_layer_destroy(s_background_bm_layer);
    layer_destroy(s_time_render_layer);
    layer_destroy(s_date_render_layer);
    layer_destroy(s_battery_render_layer);
    layer_destroy(s_temp_render_layer);
}
