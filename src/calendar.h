#pragma once
// Calendar grid model (pure; no Pebble dependencies).

enum {
  CAL_WEEK_PATTERN_PREVIOUS_NEXT = 0,
  CAL_WEEK_PATTERN_LAST_TWO = 1,
  CAL_WEEK_PATTERN_NEXT_TWO = 2,
  CAL_WEEK_PATTERN_FOUR_WEEKS = 3,
};

typedef struct {
  int days[28];     // day-of-month numbers, row-major, up to 4 weeks x 7 columns
  int special_col;  // column [0-6] holding today
  int special_row;  // rendered week row [1-4] holding today (header is row 0)
} CalGrid;

// Build the grid. year=full (2026), mon=0-11, mday=1-31, wday=0(Sun)-6(Sat),
// dow_offset=start-of-week offset 0-6. Patterns 0-2 produce 3 weeks; pattern 3
// produces previous + current + the next 2 weeks.
CalGrid calendar_build(int year, int mon, int mday, int wday,
                       int dow_offset, int week_pattern);
