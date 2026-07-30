#pragma once
// Pebble-free layout geometry so it can be unit-tested on the host.
typedef struct { int x, y, w, h; } LayoutRect;

typedef struct {
  LayoutRect statusbar;   // top band (fixed height: BT/charge/battery)
  LayoutRect slot_top;    // upper half: time/date/weather
  LayoutRect slot_bot;    // lower half: calendar
  LayoutRect battery;     // right-aligned battery box (within statusbar)
  int bt_icon_x, bt_icon_y;
  int chrg_icon_x, chrg_icon_y;
  LayoutRect clock_time;  // relative to slot_top origin
  LayoutRect clock_date;  // relative to slot_top origin
  int subtext_top;
  int cal_cols;
  int cal_cell_w;
  int cal_cell_h;
  int cal_weeks;
  int cal_left;
  int cal_gap;
} TimelyLayout;

// Compute the layout for a screen of width x height pixels. The _rows variant
// takes which optional rows are enabled (TOP status bar / CENTER above-time /
// BOTTOM above-calendar) and the requested number of calendar weeks. Four weeks
// are only honored on wide Emery geometry; other screens retain three weeks.
TimelyLayout layout_compute_rows(int width, int height, int has_top,
                                 int has_center, int has_bottom,
                                 int calendar_weeks);
TimelyLayout layout_compute(int width, int height); // all rows, 3 weeks

// Clock font choice as a pure, testable decision (the view maps it to a Pebble
// FONT_KEY). Scales with the time-band height; the big proportional Roboto is
// only for wide screens where it won't collide with the weather column.
typedef enum {
  CLOCK_FONT_ROBOTO_49 = 0, // wide screens, tall band
  CLOCK_FONT_LECO_42,
  CLOCK_FONT_LECO_38,
  CLOCK_FONT_LECO_32,
  CLOCK_FONT_LECO_28,       // smallest (very short band)
} ClockFont;
ClockFont clock_font_for(int width, int band_h);

// Weather glyph size (Climacons px) as a pure, testable decision. Narrow
// screens stay at the compact 28; wide screens take 48 when the band fits
// glyph + temperature (>= 64px), else 40.
int weather_glyph_size_for(int width, int band_h);

// Status icon (charging/DND/hourvibe) x position. The legacy spot sits left of
// the 44px battery box; when the right status slot draws a battery bar (box
// from width/2+2) the icon moves clear of it. Narrow screens only — wide
// screens use the status tray below the clock instead.
int chrg_icon_x_for(int width, int right_slot_is_bar);

// Status-tray slot x (wide screens): 20px icons pack from the right edge
// (width-4) at a 22px pitch; idx 0 is the rightmost.
int status_tray_x(int width, int idx);

// The most recently computed layout (set by the view at window_load); read by
// components that render proportionally (calendar, etc.).
void layout_store(TimelyLayout l);
TimelyLayout layout_get(void);
