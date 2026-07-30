#include "layout.h"

// Adaptive vertical layout. Three optional rows — status bar (TOP), the band
// above the time (CENTER), and the band above the calendar (BOTTOM) — each take
// a fixed slice only when enabled. Whatever is left is shared between the
// time/weather band and the calendar, so disabling a row makes the rest grow.
#define STATUSBAR_H       24
#define CENTER_H          26
#define BOTTOM_H          22
#define CAL_COLS       7
#define CAL_GAP        1
#define CAL_LEFT       2
#define FOUR_WEEK_CENTER_H 20
#define FOUR_WEEK_TIME_H   56
#define FOUR_WEEK_BOTTOM_H 18
#define FOUR_WEEK_CAL_H   110

static int clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

TimelyLayout layout_compute_rows(int width, int height, int has_top,
                                 int has_center, int has_bottom,
                                 int calendar_weeks) {
  TimelyLayout L;
  int four_weeks = width >= 180 && calendar_weeks >= 4;
  int stat_h = has_top ? STATUSBAR_H : 0;
  int center_h = has_center ? (four_weeks ? FOUR_WEEK_CENTER_H : CENTER_H) : 0;
  int bottom_h = has_bottom ? (four_weeks ? FOUR_WEEK_BOTTOM_H : BOTTOM_H) : 0;

  // Space left for the time/weather band + the calendar after the fixed rows.
  int avail = height - stat_h - center_h - bottom_h;
  int time_h;
  if (four_weeks) {
    // The full Emery profile is 56px time/weather + 110px calendar. Space
    // released by optional rows is split in the original 42:58 proportion.
    int full_avail = FOUR_WEEK_TIME_H + FOUR_WEEK_CAL_H;
    int extra = avail > full_avail ? avail - full_avail : 0;
    time_h = clampi(FOUR_WEEK_TIME_H + (extra * 42) / 100,
                    FOUR_WEEK_TIME_H, 92);
    if (avail - time_h < FOUR_WEEK_CAL_H) {
      time_h = avail - FOUR_WEEK_CAL_H;
    }
  } else {
    // The clock takes ~42% of the flexible area. The calendar is the
    // protagonist and keeps the rest.
    time_h = clampi((avail * 42) / 100, 40, 92);
  }
  int cal_h = avail - time_h;

  int slot_top_h = center_h + time_h + bottom_h;

  L.statusbar = (LayoutRect){ 0, 0, width, stat_h };
  L.slot_top  = (LayoutRect){ 0, stat_h, width, slot_top_h };
  L.slot_bot  = (LayoutRect){ 0, stat_h + slot_top_h, width, cal_h };

  // statusbar internals (battery right-aligned; icons mirror the 144x168 look)
  L.battery     = (LayoutRect){ width - 48, 4, 44, 15 };
  L.bt_icon_x   = -2; L.bt_icon_y = 2;
  L.chrg_icon_x = width - 68; L.chrg_icon_y = 2;

  // Bands inside slot_top, stacked: CENTER (top), time/weather, BOTTOM (bottom).
  // All relative to slot_top's origin.
  L.clock_date = (LayoutRect){ 2, 0, width - 4, center_h };       // CENTER row band
  L.clock_time = (LayoutRect){ 0, center_h, width - 2, time_h };  // time/weather band
  L.subtext_top = center_h + time_h;                             // BOTTOM row top

  // Calendar grid inside slot_bot: 7 columns spanning the width, with one
  // weekday header followed by the effective number of week rows.
  L.cal_cols   = CAL_COLS;
  L.cal_gap    = CAL_GAP;
  L.cal_left   = CAL_LEFT;
  L.cal_cell_w = (width - 2 * CAL_LEFT) / CAL_COLS;
  L.cal_weeks  = four_weeks ? 4 : 3;
  L.cal_cell_h = cal_h / (L.cal_weeks + 1);

  return L;
}

// Back-compat default: all three rows and three calendar weeks.
TimelyLayout layout_compute(int width, int height) {
  return layout_compute_rows(width, height, 1, 1, 1, 3);
}

ClockFont clock_font_for(int width, int band_h) {
  if (width >= 180 && band_h >= 56) { return CLOCK_FONT_ROBOTO_49; }
  if (band_h >= 52) { return CLOCK_FONT_LECO_42; }
  if (band_h >= 44) { return CLOCK_FONT_LECO_38; }
  if (band_h >= 36) { return CLOCK_FONT_LECO_32; }
  return CLOCK_FONT_LECO_28;
}

int weather_glyph_size_for(int width, int band_h) {
  if (width < 180) { return 28; }
  if (band_h >= 64) { return 48; }
  return 40;
}

int chrg_icon_x_for(int width, int right_slot_is_bar) {
  if (right_slot_is_bar) { return width / 2 + 2; } // dock where the bar starts
  return width - 68;
}

int status_tray_x(int width, int idx) {
  // all values below are in pixel
  int icon_size = 20;
  int icon_gap = 2;
  int icon_box = icon_size + icon_gap;
  int border_right = 2 ;
  return width - (border_right + icon_box) - (idx * icon_box);
}

static TimelyLayout s_current;
void layout_store(TimelyLayout l) { s_current = l; }
TimelyLayout layout_get(void) { return s_current; }
