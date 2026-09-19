#include "sim.h"
#include <stdint.h>

#define X_SIZE SIM_X_SIZE
#define Y_SIZE SIM_Y_SIZE

#define F 0.25

int32_t colorByTemp(int32_t temp) {
  if (temp <= 10)
    return 0x0A1128;
  else if (temp <= 20)
    return 0x1C3166;
  else if (temp <= 30)
    return 0x005F9E;
  else if (temp <= 40)
    return 0x0087B3;
  else if (temp <= 50)
    return 0x00A896;
  else if (temp <= 60)
    return 0x02C39A;
  else if (temp <= 70)
    return 0xA2E8DD;
  else if (temp <= 80)
    return 0xF4F1DE;
  else if (temp <= 90)
    return 0xF2CC8F;
  else if (temp <= 100)
    return 0xEAB64D;
  else if (temp <= 110)
    return 0xF38148;
  else if (temp <= 120)
    return 0xE0533C;
  else if (temp <= 130)
    return 0xC1121F;
  else if (temp <= 140)
    return 0x780000;
  else
    return 0xFDF0ED;
}

int32_t calcTemp(int32_t x, int32_t y, int32_t *field) {
  int32_t currT = field[y * X_SIZE + x];
  int32_t top, left, right, bottom;

  if (y > 0)
    top = field[(y - 1) * X_SIZE + x];
  else
    top = currT;

  if (y < Y_SIZE - 1)
    bottom = field[(y + 1) * X_SIZE + x];
  else
    bottom = currT;

  if (x > 0)
    left = field[y * X_SIZE + x - 1];
  else
    left = currT;

  if (x < X_SIZE - 1)
    right = field[y * X_SIZE + x + 1];
  else
    right = currT;

  return currT + F * (top + bottom + left + right - 4 * currT);
}

void recalculateField(int32_t *current, int32_t *next) {
  for (int y = 0; y < Y_SIZE; y++)
    for (int x = 0; x < X_SIZE; x++)
      next[y * X_SIZE + x] = calcTemp(x, y, current);
}

void drawField(int32_t *field) {
  for (int y = 0; y < Y_SIZE; y++)
    for (int x = 0; x < X_SIZE; x++)
      simPutPixel(x, y, colorByTemp(field[y * X_SIZE + x]));
}

void app() {
  int32_t field[X_SIZE * Y_SIZE] = {};
  int32_t *prev = field;
  int32_t *next;

  while (1) {
    drawField(prev);
    simFlush();
    recalculateField(prev, next);
    prev = next;
  }
}
