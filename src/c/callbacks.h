#pragma once
#include "constants.h"

// char buffer for drawings
extern char s_time_buffer[6];
extern char s_date_day_buffer[4];
extern char s_date_num_buffer[3];
extern char s_battery_buffer[4];
extern char s_temp_num_buffer[4];
extern char s_temp_unit_buffer[2];
extern char s_steps_buffer[6];

// main window
extern Window* s_window;

// draw degree sign
extern bool s_draw_degree;

// offset for battery percent sign
extern int s_percent_offset;

void update_time(bool);
void request_weather();
void tick_handler(struct tm*, TimeUnits);
void connection_callback(bool);
void battery_callback(BatteryChargeState);
void accelerometer_callback(AccelAxisType, int32_t);
void reset_timer(void*);
void weather_received_callback(DictionaryIterator*, void*);
void weather_dropped_callback(AppMessageResult, void*);
void outbox_failed_callback(DictionaryIterator*, AppMessageResult, void*);
void outbox_sent_callback(DictionaryIterator*, void*);
