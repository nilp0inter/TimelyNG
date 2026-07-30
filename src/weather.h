#pragma once
#include <stdint.h>
#include <pebble.h>
// Weather state, owned by weather.c. Access only through weather_state().
typedef struct weather_data {
  int16_t current;    // current temperature
  char condition[2];  // weather condition (mapped to a single climacons glyph)
  int16_t requests;   // requests sent since last success
  int16_t failures;   // failed responses since last success
  char city[24];      // location name from the weather provider (display only)
} __attribute__((__packed__)) weather_data;

weather_data *weather_state(void);

// Weather display component — owns its layer.
void weather_create(Layer *parent, GRect frame);
void weather_destroy(void);
void weather_set_frame(GRect frame);
void weather_set_hidden(bool hidden);
void weather_set_glyph_size(int size);  // climacons px (28/40/48); drives block metrics
void weather_set_temperature_font(GFont font);
void weather_mark_dirty(void);
