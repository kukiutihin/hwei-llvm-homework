#include <math.h>
#include <stdint.h>

#include "sim.h"

#define X_SIZE SIM_X_SIZE / 2
#define Y_SIZE SIM_Y_SIZE / 2

#define CLICK_TEMP 90
#define CLICK_RAD 30

#define COLORS_CNT 8

static const uint64_t colors[10] = {0x000004, 0x160B39, 0x420A68, 0x6A176E,
                                    0x932667, 0xBA3655, 0xDD513A, 0xF37812,
                                    0xFCA50A, 0xF6E8A0};

double rgb2linear(uint64_t channel) {
  double s = channel / 255.0;
  return s <= 0.04045 ? s / 12.92 : pow((s + 0.055) / 1.055, 2.4);
}

uint64_t linear2rgb(double linear) {
  double s = linear <= 0.0031308 ? linear * 12.92
                                 : 1.055 * pow(linear, 1.0 / 2.4) - 0.055;
  return (uint8_t)(s * 255);
}

uint64_t interpolate(uint64_t color1, uint64_t color2, double t) {
  // 0x[__ rr gg bb]
  double r1 = rgb2linear(color1 >> 16 & 0xff);
  double g1 = rgb2linear(color1 >> 8 & 0xff);
  double b1 = rgb2linear(color1 & 0xff);

  double r2 = rgb2linear(color2 >> 16 & 0xff);
  double g2 = rgb2linear(color2 >> 8 & 0xff);
  double b2 = rgb2linear(color2 & 0xff);

  uint64_t r = linear2rgb((r2 - r1) * t + r1);
  uint64_t g = linear2rgb((g2 - g1) * t + g1);
  uint64_t b = linear2rgb((b2 - b1) * t + b1);

  return (r << 16) | (g << 8) | b;
}

uint64_t colorByTemp(double temp) {
  if (temp >= 100.0) return colors[COLORS_CNT - 1];

  // 96.8 -> 9.68 => interpolate(9th, 10th, 0.68)
  double t = fmax(0.0, fmin(temp, 100.0)) / 10.0;
  uint64_t k = t;
  uint64_t color1 = colors[k];
  uint64_t color2 = colors[k + 1];
  return interpolate(color1, color2, t - (double)k);
}

double calcTemp(uint64_t x, uint64_t y, double* field) {
  double currT = field[y * X_SIZE + x];

  double top = y > 0 ? field[(y - 1) * X_SIZE + x] : currT;
  double bottom = y < Y_SIZE - 1 ? field[(y + 1) * X_SIZE + x] : currT;
  double left = x > 0 ? field[y * X_SIZE + x - 1] : currT;
  double right = x < X_SIZE - 1 ? field[y * X_SIZE + x + 1] : currT;

  return currT + 0.25f * (top + bottom + left + right - 4.0 * currT);
}

void recalculateField(double* current, double* next) {
  for (uint64_t y = 0; y < Y_SIZE; y++)
    for (uint64_t x = 0; x < X_SIZE; x++) {
      double temp = calcTemp(x, y, current);
      next[y * X_SIZE + x] = temp;
    }
}

void drawField(double* field) {
  for (uint64_t y = 0; y < Y_SIZE; y++)
    for (uint64_t x = 0; x < X_SIZE; x++)
      simPutPixel(x, y, colorByTemp(field[y * X_SIZE + x]));
}

void addObjectCircle(uint64_t xy, double* field) {
  uint64_t cx = xy >> 16;
  uint64_t cy = xy & 0xffff;
  uint64_t x, y;

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

      if (x * x + (y - 1) * (y - 1) < CLICK_RAD * CLICK_RAD) break;
    }
}

void addObjectRectangle(uint64_t xy, double* field) {
  uint64_t x = xy >> 16;
  uint64_t y = xy & 0xffff;

  uint64_t top = y - CLICK_RAD >= 0 ? y - CLICK_RAD : 0;
  uint64_t bottom = y + CLICK_RAD < Y_SIZE ? y + CLICK_RAD : Y_SIZE - 1;
  uint64_t left = x - CLICK_RAD >= 0 ? x - CLICK_RAD : 0;
  uint64_t right = x + CLICK_RAD < X_SIZE ? x + CLICK_RAD : X_SIZE - 1;

  for (uint64_t yy = top; yy <= bottom; yy++)
    for (uint64_t xx = left; xx <= right; xx++)
      field[yy * X_SIZE + xx] += CLICK_TEMP;
}

void app() {
  double field1[X_SIZE * Y_SIZE] = {};
  double field2[X_SIZE * Y_SIZE] = {};
  double* prev = field1;
  double* next = field2;

  while (1) {
    while (simHasClick()) addObjectCircle(simGetClick(), prev);

    recalculateField(prev, next);
    drawField(next);
    simFlush();

    double* tmp = prev;
    prev = next;
    next = tmp;
  }
}

int main() {
  simInit();
  app();
  simExit();
  return 0;
}
