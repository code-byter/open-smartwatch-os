#ifndef OSW_APP_GIF_PLAYER_H
#define OSW_APP_GIF_PLAYER_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppCryptoTracker : public OswApp {
 public:
  OswAppCryptoTracker(void){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppCryptoTracker(){};

 private:
};

#endif
