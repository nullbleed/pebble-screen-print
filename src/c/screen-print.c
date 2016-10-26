#include "screen-print.h"

// main window
Window *s_main_window;

// initialize the main window and subscribe to services
// push the main window to the screen
void init() {
    // load settings from storage or default settings
    prv_load_settings();

    // open app message callback
    app_message_register_inbox_received(prv_inbox_received_handler);
    app_message_open(128, 128);

    // create main window
    s_main_window = window_create();

    // set window load and unload to main window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
            .load = main_window_load,
            .unload = main_window_unload
            });

    // push main window to stack
    window_stack_push(s_main_window, true);

    // initialize connection state
    s_connection_state = bluetooth_connection_service_peek();

    // initialize callback functions
    update_time(true);
    battery_callback(battery_state_service_peek());
    connection_callback(bluetooth_connection_service_peek());

    // register callbacks
    battery_state_service_subscribe(battery_callback);
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    connection_service_subscribe((ConnectionHandlers) {
            .pebble_app_connection_handler = connection_callback,
    });
}

// deinitialize the main window and free memory
void deinit() {
    window_destroy(s_main_window);
}

// main function which represents the apps life cycle
int main(void) {
    init();
    app_event_loop();
    deinit();
}
