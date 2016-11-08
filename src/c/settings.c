#include "settings.h"

// create settings instance
ClaySettings settings;

// set default settings
void prv_default_settings() {
#if defined(PBL_COLOR)
    settings.BackgroundColor = GColorPictonBlue;
    settings.ForegroundColor = GColorOxfordBlue;
#else
    settings.BackgroundColor = GColorWhite;
    settings.ForegroundColor = GColorBlack;
#endif
    settings.VibrateOnDisconnect = false;
    settings.ShakeForSteps = true;
    settings.WeatherUseGPS = true;
    settings.Location = "";
}

// load settings
void prv_load_settings(void) {
    // load the default settings
    prv_default_settings();

    // read settings from persistent storage, if they exist
    persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// update display according to the new settings
void prv_update_display() {
    // backup old foreground color
    GColor foreground_color_old = s_foreground_color;

    // set new colors
    s_background_color = settings.BackgroundColor;
    s_foreground_color = settings.ForegroundColor;

    // redraw graphics layer
    layer_mark_dirty(s_background_layer);

    // set text color
    layer_mark_dirty(s_time_render_layer);
    layer_mark_dirty(s_battery_render_layer);
    layer_mark_dirty(s_date_render_layer);
    layer_mark_dirty(s_temp_render_layer);

    // redraw bitmap
#if defined(PBL_COLOR)
    replace_gbitmap_color(foreground_color_old, s_foreground_color, s_background_bitmap, s_background_bm_layer);
#endif

    // apply accelerometer state
    if (settings.ShakeForSteps) {
        accel_tap_service_subscribe(accelerometer_callback);
    } else {
        accel_tap_service_unsubscribe();
    }
}

// save the settings to persistent storage
void prv_save_settings() {
    persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));

    prv_update_display();
}

// received settings callback
void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
    // read color preferences
    // get background color from settings dict
    Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
    if(bg_color_t) {
        settings.BackgroundColor = GColorFromHEX(bg_color_t->value->int32);
    }

    // get foreground color from settings dict
    Tuple *fg_color_t = dict_find(iter, MESSAGE_KEY_ForegroundColor);
    if(fg_color_t) {
        settings.ForegroundColor = GColorFromHEX(fg_color_t->value->int32);
    }

    // get vibrate on disconnect settings
    Tuple *vibe_on_disconnect_t = dict_find(iter, MESSAGE_KEY_VibrateOnDisconnect);
    if (vibe_on_disconnect_t) {
        settings.VibrateOnDisconnect = vibe_on_disconnect_t->value->int32 == 1;
    }

    // get shake for steps settings
    Tuple *shake_for_steps_t = dict_find(iter, MESSAGE_KEY_ShakeForSteps);
    if (shake_for_steps_t) {
        settings.ShakeForSteps = shake_for_steps_t->value->int32 == 1;

        if (settings.ShakeForSteps) {
#if defined(PBL_HEALTH)
            if(!health_service_events_subscribe(steps_callback, NULL)) {
                APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
            }
#endif
        }
    }

    // get use gps settings 
    Tuple *use_gps_t = dict_find(iter, MESSAGE_KEY_WeatherUseGPS);
    if (use_gps_t) {
        settings.WeatherUseGPS = use_gps_t->value->int32 == 1;
    }

    // Is the location name inside this message?
    Tuple *location_tuple = dict_find(iter, MESSAGE_KEY_WeatherLocation);
    if(location_tuple) {
        // This value was stored as JS String, which is stored here as a char string
        char *location_name = location_tuple->value->cstring;

        // Use a static buffer to store the string for display
        snprintf(settings.Location, 32, "%s", location_name);

        APP_LOG(APP_LOG_LEVEL_INFO, settings.Location);
    }

    // save the new settings
    prv_save_settings();

    Tuple *init_tuple = dict_find(iter, MESSAGE_KEY_Init);
    if (init_tuple) {
        init_after_callback();
    }

    // Read tuples for weather data
    Tuple *temp_tuple = dict_find(iter, MESSAGE_KEY_Temperature);
    //Tuple *conditions_tuple = dict_find(iter, MESSAGE_KEY_Conditions);
    
    if(temp_tuple) {
        if (((int) temp_tuple->value->int32) == -300) {
            snprintf(s_temp_num_buffer, sizeof(s_temp_num_buffer), "10");
        } else {
            snprintf(s_temp_num_buffer, sizeof(s_temp_num_buffer), "%d", (int) temp_tuple->value->int32);
        }
    }
    layer_mark_dirty(s_temp_render_layer);
}

void init_after_callback() {
    request_weather();
}
