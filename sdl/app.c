#include "sim.h"
#include <math.h>
#include <stdint.h>

#define X_SIZE SIM_X_SIZE / 4
#define Y_SIZE SIM_Y_SIZE / 4

#define CLICK_TEMP 90
#define CLICK_RAD 30

#define COLORS_CNT 8

static const uint32_t colors[10] = {0x000004, 0x160B39, 0x420A68, 0x6A176E,
                                    0x932667, 0xBA3655, 0xDD513A, 0xF37812,
                                    0xFCA50A, 0xF6E8A0};

float rgb2linear(uint32_t channel) {
  float s = channel / 255.0f;
  return s <= 0.04045 ? s / 12.92 : pow((s + 0.055) / 1.055, 2.4);
}

uint32_t linear2rgb(float linear) {
  float s = linear <= 0.0031308 ? linear * 12.92
                                : 1.055 * pow(linear, 1.0 / 2.4) - 0.055;
  return (uint8_t)(s * 255);
}

uint32_t interpolate(uint32_t color1, uint32_t color2, float t) {
  // 0x[__ rr gg bb]
  float r1 = rgb2linear(color1 >> 16 & 0xff);
  float g1 = rgb2linear(color1 >> 8 & 0xff);
  float b1 = rgb2linear(color1 & 0xff);

  float r2 = rgb2linear(color2 >> 16 & 0xff);
  float g2 = rgb2linear(color2 >> 8 & 0xff);
  float b2 = rgb2linear(color2 & 0xff);

  uint32_t r = linear2rgb((r2 - r1) * t + r1);
  uint32_t g = linear2rgb((g2 - g1) * t + g1);
  uint32_t b = linear2rgb((b2 - b1) * t + b1);

  return (r << 16) | (g << 8) | b;
}

uint32_t colorByTemp(float temp) {
  if (temp >= 100.0)
    return colors[COLORS_CNT - 1];

  // 96.8 -> 9.68 => interpolate(9th, 10th, 0.68)
  float t = fmax(0.0, fmin(temp, 100.0)) / 10.0;
  uint32_t k = t;
  uint32_t color1 = colors[k];
  uint32_t color2 = colors[k + 1];
  return interpolate(color1, color2, t - (float)k);
}

float calcTemp(uint32_t x, uint32_t y, float *field) {
  float currT = field[y * X_SIZE + x];

  float top = y > 0 ? field[(y - 1) * X_SIZE + x] : currT;
  float bottom = y < Y_SIZE - 1 ? field[(y + 1) * X_SIZE + x] : currT;
  float left = x > 0 ? field[y * X_SIZE + x - 1] : currT;
  float right = x < X_SIZE - 1 ? field[y * X_SIZE + x + 1] : currT;

  return currT + 0.25 * (top + bottom + left + right - 4.0 * currT);
}

void recalculateField(float *current, float *next) {
  for (uint32_t y = 0; y < Y_SIZE; y++)
    for (uint32_t x = 0; x < X_SIZE; x++) {
      float temp = calcTemp(x, y, current);
      next[y * X_SIZE + x] = temp;
    }
}

void drawField(float *field) {
  for (uint32_t y = 0; y < Y_SIZE; y++)
    for (uint32_t x = 0; x < X_SIZE; x++)
      simPutPixel(x, y, colorByTemp(field[y * X_SIZE + x]));
}

void addObjectCircle(uint32_t xy, float *field) {
  uint32_t cx = xy >> 16;
  uint32_t cy = xy & 0xffff;
  int x, y;

  for (x = 0, y = CLICK_RAD; x < y; x++)
    for (; y >= 0; y--) {
      field[(cy + y) * X_SIZE + (cx + x)] += CLICK_TEMP;
      field[(cy - y) * X_SIZE + (cx + x)] += CLICK_TEMP;
      field[(cy + y) * X_SIZE + (cx - x)] += CLICK_TEMP;
      field[(cy - y) * X_SIZE + (cx - x)] += CLICK_TEMP;

      field[(cy + x) * X_SIZE + (cx + y)] += CLICK_TEMP;
      field[(cy - x) * X_SIZE + (cx + y)] += CLICK_TEMP;
      field[(cy + x) * X_SIZE + (cx - y)] += CLICK_TEMP;
      field[(cy - x) * X_SIZE + (cx - y)] += CLICK_TEMP;

      if (x * x + (y - 1) * (y - 1) < CLICK_RAD * CLICK_RAD)
        break;
    }
}

void addObjectRectangle(uint32_t xy, float *field) {
  uint32_t x = xy >> 16;
  uint32_t y = xy & 0xffff;

  uint32_t top = y - CLICK_RAD >= 0 ? y - CLICK_RAD : 0;
  uint32_t bottom = y + CLICK_RAD < Y_SIZE ? y + CLICK_RAD : Y_SIZE - 1;
  uint32_t left = x - CLICK_RAD >= 0 ? x - CLICK_RAD : 0;
  uint32_t right = x + CLICK_RAD < X_SIZE ? x + CLICK_RAD : X_SIZE - 1;

  for (uint32_t yy = top; yy <= bottom; yy++)
    for (uint32_t xx = left; xx <= right; xx++)
      field[yy * X_SIZE + xx] += CLICK_TEMP;
}

void app() {
  float field1[X_SIZE * Y_SIZE] = {};
  float field2[X_SIZE * Y_SIZE] = {};
  float *prev = field1;
  float *next = field2;

  while (1) {
    while (simHasClick())
      addObjectRectangle(simGetClick(), prev);

    recalculateField(prev, next);
    drawField(next);
    simFlush();

    float *tmp = prev;
    prev = next;
    next = tmp;
  }
}
