#include "utest.h"
#include "calendar.h"

// Thu June 4 2026, Sunday start, 1 week before + 1 after.
// Matches the emulator screenshot: 24..30 / 31,1..6 / 7..13, today=4.
UTEST(calendar, june_2026_sunday_start) {
  CalGrid g = calendar_build(2026, 5, 4, 4, 0, 0);
  ASSERT_EQ(4, g.special_col);
  ASSERT_EQ(2, g.special_row);
  ASSERT_EQ(24, g.days[0]);
  ASSERT_EQ(30, g.days[6]);
  ASSERT_EQ(31, g.days[7]);
  ASSERT_EQ(4,  g.days[11]);
  ASSERT_EQ(6,  g.days[13]);
  ASSERT_EQ(7,  g.days[14]);
  ASSERT_EQ(13, g.days[20]);
}

UTEST(calendar, monday_start_offset) {
  CalGrid g = calendar_build(2026, 5, 4, 4, 1, 0);
  ASSERT_EQ(3, g.special_col);
  ASSERT_EQ(1, g.days[7]);   // Monday Jun 1 starts the week row
  ASSERT_EQ(4, g.days[10]);  // today
}

UTEST(calendar, sunday_with_monday_start_wraps_to_last_col) {
  CalGrid g = calendar_build(2026, 5, 7, 0, 1, 0);
  ASSERT_EQ(6, g.special_col); // Sunday is the last column under Monday start
}

UTEST(calendar, four_weeks_extend_existing_previous_next_pattern) {
  CalGrid three = calendar_build(
      2026, 5, 4, 4, 0, CAL_WEEK_PATTERN_PREVIOUS_NEXT);
  CalGrid four = calendar_build(
      2026, 5, 4, 4, 0, CAL_WEEK_PATTERN_FOUR_WEEKS);
  ASSERT_EQ(4, four.special_col);
  ASSERT_EQ(2, four.special_row);
  for (int i = 0; i < 21; i++) {
    ASSERT_EQ(three.days[i], four.days[i]);
  }
  ASSERT_EQ(14, four.days[21]);
  ASSERT_EQ(20, four.days[27]);
}

UTEST(calendar, four_weeks_cross_year_boundary) {
  CalGrid g = calendar_build(
      2026, 11, 31, 4, 0, CAL_WEEK_PATTERN_FOUR_WEEKS);
  ASSERT_EQ(4, g.special_col);
  ASSERT_EQ(2, g.special_row);
  ASSERT_EQ(20, g.days[0]);
  ASSERT_EQ(31, g.days[11]);
  ASSERT_EQ(2, g.days[13]);
  ASSERT_EQ(3, g.days[14]);
  ASSERT_EQ(16, g.days[27]);
}

UTEST(calendar, four_weeks_honor_monday_start) {
  CalGrid g = calendar_build(
      2026, 5, 4, 4, 1, CAL_WEEK_PATTERN_FOUR_WEEKS);
  ASSERT_EQ(3, g.special_col);
  ASSERT_EQ(2, g.special_row);
  ASSERT_EQ(25, g.days[0]);
  ASSERT_EQ(31, g.days[6]);
  ASSERT_EQ(15, g.days[21]);
  ASSERT_EQ(21, g.days[27]);
}
