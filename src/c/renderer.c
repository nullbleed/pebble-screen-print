#include "renderer.h"

// font
FFont *s_render_font;

// some constants and offsets for different pebble models
#if defined(PBL_ROUND)
static GTextAlignment s_date_align = GTextAlignmentRight;
static GTextAlignment s_temp_align = GTextAlignmentLeft;

static int s_date_render_offset = 32;
static int s_temp_render_offset = 118;
static int s_temp_unit_offset = 0;
#elif defined(PBL_RECT)
static GTextAlignment s_date_align = GTextAlignmentLeft;
static GTextAlignment s_temp_align = GTextAlignmentRight;

static int s_date_render_offset = -14;
#if defined(PBL_PLATFORM_EMERY)
static int s_temp_render_offset = 165;
#else
static int s_temp_render_offset = 110;
#endif
static int s_temp_unit_offset = 2;
#endif

// render font
void render_text(GContext *ctx, char *buffer, int offset_x, int offset_y, int font_size, GTextAlignment alignment) {
    // create new fontcontext and initialize it
    FContext fctx;
    fctx_init_context(&fctx, ctx);
    fctx_begin_fill(&fctx);

    // set font size and font to the context
    fctx_set_text_em_height(&fctx, s_render_font, font_size);

    // set fill color to context
    fctx_set_fill_color(&fctx, s_foreground_color);

    // set transform method
    fctx_set_pivot(&fctx, FPointZero);

    // set offset to text layer
    fctx_set_offset(&fctx, FPointI(offset_x, offset_y));

    // draw the text into the context
    fctx_draw_string(&fctx, buffer, s_render_font, alignment, FTextAnchorCapTop);

    // end drawing procedure and render font to gcontext
    fctx_end_fill(&fctx);
    fctx_deinit_context(&fctx);
}

// time render proc
void time_render_proc(Layer *layer, GContext *ctx) {
    // get layer bounds
    GRect bounds = layer_get_bounds(layer);
    
    // render the given text
    // (context, text_string, offset_x, offset_y, font_size, GTextAlignment)
#if defined(PBL_ROUND)
    render_text(ctx, s_time_buffer, bounds.size.w / 2, 50, 50, GTextAlignmentCenter);
#else
    render_text(ctx, s_time_buffer, bounds.size.w / 2, 52, 47, GTextAlignmentCenter);
#endif
}

// battery render proc
void battery_render_proc(Layer *layer, GContext *ctx) {
    // get layer bounds
    GRect bounds = layer_get_bounds(layer);

    // render the given text
    // (context, text_string, offset_x, offset_y, font_size, GTextAlignment)
    render_text(ctx, s_battery_buffer, bounds.size.w / 2, 17, 19, GTextAlignmentCenter);
}

// date render proc
void date_render_proc(Layer *layer, GContext *ctx) {
    // get layer bounds
    GRect bounds = layer_get_bounds(layer);

    // render the given text
    // (context, text_string, offset_x, offset_y, font_size, GTextAlignment)
    render_text(ctx, s_date_day_buffer, (bounds.size.w / 2) + s_date_render_offset, 107, 19, s_date_align);
    render_text(ctx, s_date_num_buffer, (bounds.size.w / 2) + s_date_render_offset, 125, 19, s_date_align);
}

// temp render proc
void temp_render_proc(Layer *layer, GContext *ctx) {
    // get layer bounds
    GRect bounds = layer_get_bounds(layer);

    //another offset for round watches because the text is left aligned
    //TODO: improve dynamic offset
    int size_offset;
#if defined(PBL_ROUND)
    size_offset = 13 * (strlen(s_temp_num_buffer) - 2);
#else
    size_offset = 0;
#endif

    // render the given text
    // (context, text_string, offset_x, offset_y, font_size, GTextAlignment)
    render_text(ctx, s_temp_num_buffer, (bounds.size.w / 2) + s_temp_render_offset - s_temp_unit_offset - size_offset, 107, 19, s_temp_align);
    render_text(ctx, s_temp_unit_buffer, (bounds.size.w / 2) + s_temp_render_offset - size_offset, 125, 19, s_temp_align);
}

// initialize procedure for background graphics
void init_proc(Layer *layer, GContext *ctx) {
    // get layer boundaries
    GRect bounds = layer_get_bounds(layer);

    // draw background
    if (s_connection_state == true) {
        graphics_context_set_fill_color(ctx, s_background_color);
    } else { // if connection is lost, draw it white
        graphics_context_set_fill_color(ctx, GColorWhite);
    }
    graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, bounds.size.h), 0, 0);

#if defined(PBL_ROUND)
    // draw outline
    graphics_context_set_fill_color(ctx, s_foreground_color);
    graphics_fill_circle(ctx, GPoint(bounds.size.w / 2, bounds.size.h / 2), (bounds.size.w / 2) - 5);

    // draw inner circle
    graphics_context_set_fill_color(ctx, s_background_color);
    graphics_fill_circle(ctx, GPoint(bounds.size.w / 2, bounds.size.h / 2), (bounds.size.w / 2) - 9);

    //draw lines in the middle
    graphics_context_set_fill_color(ctx, s_foreground_color);
    graphics_fill_rect(ctx, GRect(35, 37, bounds.size.w - 70, 4), 2, GCornersAll);

    graphics_context_set_fill_color(ctx, s_foreground_color);
    graphics_fill_rect(ctx, GRect(15, 95, bounds.size.w - 30, 4), 2, GCornersAll);

    // draw degree sign
    if (s_draw_degree) {
        graphics_context_set_fill_color(ctx, s_foreground_color);
        graphics_fill_circle(ctx, GPoint(bounds.size.w - s_temp_offset + 18, 108), 1);
    }
#elif defined(PBL_RECT)
    // draw outline
    graphics_context_set_fill_color(ctx, s_foreground_color);
    graphics_fill_rect(ctx, GRect(2, 2, bounds.size.w - 4, bounds.size.h - 4), 4, GCornersAll);

    // draw inner rectangle
    graphics_context_set_fill_color(ctx, s_background_color);
    graphics_fill_rect(ctx, GRect(6, 6, bounds.size.w - 12, bounds.size.h - 12), 4, GCornersAll);

    //draw lines in the middle
    graphics_context_set_fill_color(ctx, s_foreground_color);
    graphics_fill_rect(ctx, GRect(25, 40, bounds.size.w - 50, 4), 2, GCornersAll);

    graphics_context_set_fill_color(ctx, s_foreground_color);
    graphics_fill_rect(ctx, GRect(10, 95, bounds.size.w - 20, 4), 2, GCornersAll);
    
    // draw degree sign
    if (s_draw_degree) {
        graphics_context_set_fill_color(ctx, s_foreground_color);
        graphics_fill_circle(ctx, GPoint(bounds.size.w - s_temp_offset + 39, 108), 1);
    }
#endif
    
    // draw percent sign
    graphics_context_set_fill_color(ctx, s_foreground_color);
    graphics_context_set_stroke_color(ctx, s_foreground_color);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_fill_circle(ctx, GPoint((bounds.size.w / 2) + 17 + s_percent_offset, 24), 1);
    graphics_fill_circle(ctx, GPoint((bounds.size.w / 2) + 23 + s_percent_offset, 29), 1);
    graphics_draw_line(ctx, GPoint((bounds.size.w / 2) + 23 + s_percent_offset, 24), GPoint((bounds.size.w / 2) + 17 + s_percent_offset, 29));
}

