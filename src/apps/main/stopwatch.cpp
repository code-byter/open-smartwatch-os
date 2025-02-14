
#include "./apps/main/stopwatch.h"

#include "config.h"  // Include the config here again to access the language definitions of it
#include "gfx_util.h"
#include "osw_app.h"
#include "osw_hal.h"
#include "osw_ui_util.h"

// continue after sleep does not work yet
// because millis restarts from 0
// TODO: use the I2C RTC instead
RTC_DATA_ATTR long start = 0;
RTC_DATA_ATTR long diff = 0;
RTC_DATA_ATTR bool running = false;
RTC_DATA_ATTR bool reset = true;
RTC_DATA_ATTR long sumPaused = 0;

void OswAppStopWatch::setup(OswHal* hal) {}

void OswAppStopWatch::loop(OswHal* hal) {
  if (hal->btn3Down()) {
    if (reset) {  // Start
      start = hal->getLocalTime();
    } else {  // Continue
      sumPaused += diff;
      start = hal->getLocalTime();
    }
    running = true;
    reset = false;
    hal->clearBtn3();
  }

  if (hal->btn2Down()) {
    if (running) {  // Stop
      running = false;
    } else {  // Reset
      diff = 0;
      sumPaused = 0;
      reset = true;
    }
    hal->clearBtn2();
  }

  hal->gfx()->fill(0);
  hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));

  if (reset) {
    hal->gfx()->setTextCursorBtn3();
    hal->gfx()->print(LANG_STW_START);
  } else if (!running) {
    hal->gfx()->setTextCursorBtn3();
    hal->gfx()->print(LANG_STW_CONTINUE);
  }

  if (running) {
    hal->gfx()->setTextCursorBtn2();
    hal->gfx()->print(LANG_STW_STOP);
  } else if (!reset) {
    hal->gfx()->setTextCursorBtn2();
    hal->gfx()->print(LANG_STW_RESET);
  }

  // hal->gfx()->setTextCursorBtn1();
  // hal->gfx()->print("TEST");

  if (running) {
    diff = hal->getLocalTime() - start;
  }

  long total = diff + sumPaused;
  long deltaSeconds = (total) % 60;
  long deltaMinutes = (total / 60) % 60;
  long deltaHours = (total / 60 / 60) % 24;
  long deltaDays = total / 60 / 60 / 24;

  hal->getCanvas()->setTextSize(4);

  if (deltaDays) {
    hal->gfx()->setTextSize(4);
    hal->gfx()->setTextBottomAligned();
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextCursor(120, 120);
    hal->gfx()->print((String(deltaDays) + "d").c_str());
  }

  hal->gfx()->setTextSize(4);
  if (deltaDays) {
    hal->gfx()->setTextTopAligned();
  } else {
    hal->gfx()->setTextMiddleAligned();
  }
  hal->gfx()->setTextLeftAligned();
  // manually center the counter:
  hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(4), 120);
  hal->gfx()->printDecimal(deltaHours, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(deltaMinutes, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(deltaSeconds, 2);

  // hal->gfx()->print(".");
  // pushing the button is inaccurate
  // also we have more space on the screen this way
  // hal->gfx()->print(deltaMillis / 100);

  hal->requestFlush();
}

void OswAppStopWatch::stop(OswHal* hal) {}
