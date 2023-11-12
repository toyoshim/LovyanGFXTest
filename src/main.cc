#include <stdio.h>

#include "freertos/freertos.h"
#include "freertos/task.h"

#include "esp_task_wdt.h"

#define LGFX_AUTODETECT
#include "LovyanGFX.hpp"

static void task(void* arguments) {
  LGFX lcd;
  lcd.init();
  lcd.fillScreen(lcd.color888(0, 0, 0));

  for (;;) {
    vTaskDelay(1);
    int32_t x, y;
    // if (lcd.getTouchRaw(&x, &y)) {
    if (lcd.getTouch(&x, &y)) {
      lcd.drawPixel(x, y, lcd.color888(255, 255, 255));

      // Current getTouch() never returns a point that is recognized as in the
      // virtual screen button area. With my patch, getTouch() will be fixed to
      // return the same value with the one getTouchRaw() does unless users set
      // up their own affine matrix. This is because the default adjustment
      // logic mistakenly runs after the Core2 specific code's 1:1
      // initialization, and set it up as touch area to panel area mapping
      // matrix.
      printf("(%ld, %ld)%s\n", x, y,
             (lcd.panel()->config().panel_height <= y)
                 ? " - virtual screen button area"
                 : "");
    }
  }
}

extern "C" void app_main() {
  xTaskCreatePinnedToCore(task, "LovyanGFXTest", 8192, 0, 1, 0, APP_CPU_NUM);
  for (;;) {
    vTaskDelay(1);
  }
}