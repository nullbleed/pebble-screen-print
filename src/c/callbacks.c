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

        if (!init) {
            request_weather();
        }
    } 
}

// get weather information
void request_weather() {
    // create dict that is going to be send
    DictionaryIterator *iter;

    // register dict to the outbox handler
    app_message_outbox_begin(&iter);

    // write information to dict
    //dict_write_begin(iter, buffer, sizeof(buffer));
    if (settings.WeatherUseGPS) {
        dict_write_cstring(iter, MESSAGE_KEY_Location, "gps");
    } else {
        if (strncmp(settings.Location, "", strlen(settings.Location)) > 0 ) {
            dict_write_cstring(iter, MESSAGE_KEY_Location, settings.Location);
        } else {
            dict_write_cstring(iter, MESSAGE_KEY_Location, settings.Location);
        }
    }

    // write last element of dict
    dict_write_uint8(iter, 0, 0);

    // send dict to pkjs
    app_message_outbox_send();
    
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
    Layer *window_layer = window_get_root_layer(s_window);

    // remove temperature layer from main window
    layer_remove_from_parent(s_temp_render_layer);
    layer_remove_from_parent(bitmap_layer_get_layer(s_background_bm_layer));
    layer_remove_from_parent(bitmap_layer_get_layer(s_background_wbm_layer));

    // remove degree sign
    s_draw_degree = false;
    layer_mark_dirty(s_background_layer);

#if defined(PBL_HEALTH)
            if(!health_service_events_subscribe(steps_callback, NULL)) {
                APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
            }
#endif

    // add steps layer to main window
    layer_add_child(window_layer, s_steps_render_layer);

    // start reset timer
    app_timer_register(3000, (AppTimerCallback) reset_timer, NULL);
}

// timer that reverts to display the temperature
void reset_timer(void *data) {
    Layer *window_layer = window_get_root_layer(s_window);

    //TODO: remove steps layer from main window
    //layer_pop_child(window_layer, s_steps_render_layer);
    layer_remove_from_parent(s_steps_render_layer);

    // add degree sign
    s_draw_degree = true;
    layer_mark_dirty(s_background_layer);

    // add temperature layer back to the main window
    layer_add_child(window_layer, s_temp_render_layer);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_background_wbm_layer));
    layer_add_child(window_layer, bitmap_layer_get_layer(s_background_bm_layer));
}

void weather_received_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Message received");

    // Read tuples for data
    Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_Temperature);
    //Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_Conditions);

    snprintf(s_temp_num_buffer, sizeof(s_temp_num_buffer), "%d", (int) temp_tuple->value->int32);
    layer_mark_dirty(s_temp_render_layer);
}

void weather_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

// steps callback
void steps_callback(HealthEventType event, void *context) {
    HealthMetric metric = HealthMetricStepCount;
    time_t start = time_start_of_today();
    time_t end = time(NULL);

    // Check the metric has data available for today
    HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, 
      start, end);

    if(mask & HealthServiceAccessibilityMaskAvailable) {
        // Data is available!
        APP_LOG(APP_LOG_LEVEL_INFO, "Steps today: %d", 
              (int)health_service_sum_today(metric));
        snprintf(s_steps_buffer, sizeof(s_steps_buffer), "%d", (int) health_service_sum_today(metric));
    } else {
        // No data recorded yet today
        APP_LOG(APP_LOG_LEVEL_ERROR, "Data unavailable!");
    }

    layer_mark_dirty(s_steps_render_layer);
}
