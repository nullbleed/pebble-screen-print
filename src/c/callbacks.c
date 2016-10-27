#include "callbacks.h"

// create buffers
char s_time_buffer[6];
char s_date_day_buffer[4];
char s_date_num_buffer[3];
char s_battery_buffer[4];
char s_temp_num_buffer[4];
char s_temp_unit_buffer[2];
char s_steps_buffer[6];

// states
int s_battery_level;
bool s_connection_state;

// vibration pattern
static const uint32_t const s_short_vibe[] = { 80 };
static const uint32_t const s_double_vibe[] = { 50, 100, 80 };

// root window
Window *s_window;

bool s_draw_degree;

// offset for battery percent sign
int s_percent_offset = 0;

// updates the different time layers
void update_time(bool init) {
    // get time
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    // get actual timestamp
    strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M", tick_time);

    // print time to buffer
    layer_mark_dirty(s_time_render_layer);

    // get minutes as int
    char minute_buffer[8];
    int minutes;
    strftime(minute_buffer, sizeof(minute_buffer), "%M", tick_time);
    minutes = atoi(minute_buffer);

    if (minutes == 0 || init) {
        // get hours as int
        char hours_buffer[8];
        int hours;
        strftime(hours_buffer, sizeof(hours_buffer), "%H", tick_time);
        hours = atoi(hours_buffer);

        // refresh date only on day change
        if (hours == 0 || init) {
            // set day
            strftime(s_date_day_buffer, sizeof(s_date_day_buffer), "%a", tick_time);

            // set date
            strftime(s_date_num_buffer, sizeof(s_date_num_buffer), "%e", tick_time);
            layer_mark_dirty(s_date_render_layer);
        }
    }
}

// callback for the time service which marks the time layer as dirty to refresh it
void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time(false);
}

// update bt connection status
void connection_callback(bool connected) {
    if (connected == (s_connection_state == true)) { // no change
        return;
    }

    if (!connected) {
        // set connection state
        s_connection_state = false;
        
        // refresh background layer for disconnection indicator
        layer_mark_dirty(s_background_layer);

        // vibrate on disconnect
        if (settings.VibrateOnDisconnect == true) {
            vibes_enqueue_custom_pattern((VibePattern) {
                    .durations = s_double_vibe,
                    .num_segments = ARRAY_LENGTH(s_double_vibe),
            });
        }
    } else {
        // set connection state
        s_connection_state = true;

        // refresh background layer for disconnection indicator
        layer_mark_dirty(s_background_layer);

        // vibrate on disconnect
        if (settings.VibrateOnDisconnect == true) {
            vibes_enqueue_custom_pattern((VibePattern) {
                    .durations = s_short_vibe,
                    .num_segments = ARRAY_LENGTH(s_short_vibe),
            });
        }
    }
}

// update battery level
void battery_callback(BatteryChargeState state) {
    if (s_battery_level != state.charge_percent) {
        // print battery level to battery buffer
        s_battery_level = state.charge_percent;
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d", s_battery_level);

        // set percent offset for self drawn percent sign on 100% battery
        if (s_battery_level == 100) {
            s_percent_offset = 2;
        } else if (s_battery_level == 10){
            s_percent_offset = -5;
        } else if (s_battery_level == 0){
            s_percent_offset = -9;
        } else {
            s_percent_offset = -2;
        }

        // refresh battery render layer
        layer_mark_dirty(s_battery_render_layer);
        
        // redraw the layer with the percent sign
        layer_mark_dirty(s_background_layer);
    }
}

// accelerometer callback
void accelerometer_callback(AccelAxisType axis, int32_t direction) {
    // remove temperature layer from main window
    layer_remove_from_parent(s_temp_render_layer);
    layer_remove_from_parent(bitmap_layer_get_layer(s_background_bm_layer));
    layer_remove_from_parent(bitmap_layer_get_layer(s_background_wbm_layer));

    // remove degree sign
    s_draw_degree = false;
    layer_mark_dirty(s_background_layer);

    // add steps layer to main window
    //layer_add_child(window_layer, s_steps_render_layer);

    // start reset timer
    app_timer_register(3000, (AppTimerCallback) reset_timer, NULL);
}

// timer that reverts to display the temperature
void reset_timer(void *data) {
    Layer *window_layer = window_get_root_layer(s_window);

    //TODO: remove steps layer from main window
    //layer_pop_child(window_layer, s_steps_render_layer);

    // add degree sign
    s_draw_degree = true;
    layer_mark_dirty(s_background_layer);

    // add temperature layer back to the main window
    layer_add_child(window_layer, s_temp_render_layer);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_background_wbm_layer));
    layer_add_child(window_layer, bitmap_layer_get_layer(s_background_bm_layer));
}
