#include "calendar.h"
#include "calendar_view.h"
#include "layout.h"
#include "theme.h"
#include "locale.h"
#include "settings.h"
#include "debug.h"
#include "ui.h"
#include <pebble.h>

static Layer *s_calendar_layer;


static void calendar_render(Layer *me, GContext* ctx) {
    (void)me;
    TimelyLayout L = layout_get();
    const int CAL_WIDTH = L.cal_cell_w, CAL_HEIGHT = L.cal_cell_h, DEVICE_WIDTH = L.statusbar.w;
    const int CAL_DAYS = 7, CAL_GAP = 1, CAL_LEFT = 2;

    (void)me;

    CalGrid grid = calendar_build(currentTime->tm_year + 1900,
                                  currentTime->tm_mon,
                                  currentTime->tm_mday,
                                  currentTime->tm_wday,
                                  settings_get()->dayOfWeekOffset,
                                  adv_settings_get()->week_pattern);
    int *calendar = grid.days;
    int specialDay = grid.special_col;
    Palette pal = theme_palette();
    if (debug_get()->general) { app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, "Calendar - sCol: %d, sRow: %d", grid.special_col, grid.special_row); }

// ---------------------------
// Now that we've calculated which days go where, we'll move on to the display logic.
// ---------------------------


    int weeks = L.cal_weeks;

    // Use the palette's subdued grid role on color displays. Monochrome
    // platforms cannot lower contrast without losing the one-pixel grid.
    if (settings_get()->grid) {
#ifdef PBL_COLOR
      graphics_context_set_fill_color(ctx, pal.grid);
#else
      graphics_context_set_fill_color(ctx, pal.fg);
#endif
      graphics_fill_rect(
          ctx,
          GRect(CAL_LEFT + CAL_GAP, CAL_HEIGHT - CAL_GAP,
                DEVICE_WIDTH - 2 * (CAL_LEFT + CAL_GAP), CAL_HEIGHT * weeks),
          0, GCornerNone);
      setColors(ctx);
    }

    for (int col = 0; col < CAL_DAYS; col++) {
      int weekday = col + settings_get()->dayOfWeekOffset;
      if (weekday > 6) { weekday -= 7; }
      bool weekend = weekday == 0 || weekday == 6;
      graphics_context_set_text_color(ctx, weekend ? pal.weekend : pal.fg);

      GFont current = col == specialDay ? cal_bold : cal_normal;
      const char *label = lang_gen_get()->abbrDaysOfWeek[weekday];
      GSize ts = graphics_text_layout_get_content_size(
          label, current, GRect(0, 0, CAL_WIDTH, CAL_HEIGHT),
          GTextOverflowModeWordWrap, GTextAlignmentCenter);
      int ty = (CAL_HEIGHT - CAL_GAP - ts.h) / 2 - ts.h / 6;
      graphics_draw_text(
          ctx, label, current,
          GRect(CAL_WIDTH * col + CAL_LEFT + CAL_GAP, ty, CAL_WIDTH, CAL_HEIGHT),
          GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }

    int specialRow = grid.special_row;
    for (int row = 1; row <= weeks; row++) {
      for (int col = 0; col < CAL_DAYS; col++) {
        bool today = row == specialRow && col == specialDay;
        if (today && settings_get()->day_invert) {
          setTodayColors(ctx);
        }

        GFont current = today ? cal_bold : cal_normal;
        GRect cell = GRect(
            CAL_WIDTH * col + CAL_LEFT + CAL_GAP, CAL_HEIGHT * row,
            CAL_WIDTH - CAL_GAP, CAL_HEIGHT - CAL_GAP);
        graphics_fill_rect(ctx, cell, 0, GCornerNone);

        if (!today) {
          int weekday = col + settings_get()->dayOfWeekOffset;
          if (weekday > 6) { weekday -= 7; }
          graphics_context_set_text_color(
              ctx, (weekday == 0 || weekday == 6) ? pal.weekend : pal.fg);
        }

        char date_text[3];
        snprintf(date_text, sizeof(date_text), "%d",
                 calendar[col + CAL_DAYS * (row - 1)]);
        // All dates share the regular face's line metrics so the heavier
        // current day stays on the same baseline as the rest of its row.
        GSize ts = graphics_text_layout_get_content_size(
            date_text, cal_normal, GRect(0, 0, CAL_WIDTH, CAL_HEIGHT),
            GTextOverflowModeWordWrap, GTextAlignmentCenter);
        int ty = CAL_HEIGHT * row
               + (CAL_HEIGHT - CAL_GAP - ts.h) / 2
               - ts.h / 6;
        graphics_draw_text(
            ctx, date_text, current,
            GRect(cell.origin.x, ty, cell.size.w, CAL_HEIGHT),
            GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

        if (today) {
          setColors(ctx);
        }
      }
    }
}

void calendar_create(Layer *parent, GRect frame) {
  s_calendar_layer = layer_create(frame);
  layer_set_update_proc(s_calendar_layer, calendar_render);
  layer_add_child(parent, s_calendar_layer);
  layer_set_hidden(s_calendar_layer, true);
}
void calendar_destroy(void)           { layer_destroy(s_calendar_layer); }
void calendar_set_hidden(bool hidden) { layer_set_hidden(s_calendar_layer, hidden); }
void calendar_set_frame(GRect frame)  { layer_set_frame(s_calendar_layer, frame); }
void calendar_mark_dirty(void)        { layer_mark_dirty(s_calendar_layer); }
