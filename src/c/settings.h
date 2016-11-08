#pragma once
#include "constants.h"
#include "callbacks.h"

#include "gbitmap-colour-palette-manipulator.h"

void prv_default_settings();
void prv_load_settings(void);
void prv_update_display();
void prv_save_settings();
void prv_inbox_received_handler(DictionaryIterator*, void*);
void init_after_callback();
