
#include "./apps/_experiments/crypto_tracker.h"

#include <AnimatedGIF.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

// #include "assets/mwdu.h"
// #define GIF_NAME mwdu_gif

#include "assets/doge.h"
#define GIF_NAME doge_gif

#define HTTP_GET_BUF_LEN 20
char httpGetBufferDoge[HTTP_GET_BUF_LEN];

AnimatedGIF gif_doge;

// handle which is used by GIFDraw callback below
Graphics2D* doge_gfx;
int OffsetX = 12;
int OffsetY = 30;

String price = "$0.0054";

// callback to draw the gif to the gfx above
void GIFDrawDoge(GIFDRAW* pDraw) {
  uint8_t* s;
  uint16_t *d, *usPalette, usTemp[320];
  int x, y;

  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y;  // current line

  s = pDraw->pPixels;
  if (pDraw->ucDisposalMethod == 2)  // restore to background color
  {
    for (x = 0; x < pDraw->iWidth; x++) {
      if (s[x] == pDraw->ucTransparent) s[x] = pDraw->ucBackground;
    }
    pDraw->ucHasTransparency = 0;
  }
  // Apply the new pixels to the main image
  if (pDraw->ucHasTransparency)  // if transparency used
  {
    uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
    int x, iCount;
    pEnd = s + pDraw->iWidth;
    x = 0;
    iCount = 0;  // count non-transparent pixels
    while (x < pDraw->iWidth) {
      c = ucTransparent - 1;
      d = usTemp;
      while (c != ucTransparent && s < pEnd) {
        c = *s++;
        if (c == ucTransparent)  // done, stop
        {
          s--;  // back up to treat it like transparent
        } else  // opaque
        {
          *d++ = usPalette[c];
          iCount++;
        }
      }            // while looking for opaque pixels
      if (iCount)  // any opaque pixels?
      {
        for (int xOffset = 0; xOffset < iCount; xOffset++) {
          doge_gfx->drawPixel(x + xOffset + OffsetX, y + OffsetY, usTemp[xOffset]);
          // dma_display.drawPixelRGB565(x + xOffset, y, usTemp[xOffset]);
        }
        x += iCount;
        iCount = 0;
      }
      // no, look for a run of transparent pixels
      c = ucTransparent;
      while (c == ucTransparent && s < pEnd) {
        c = *s++;
        if (c == ucTransparent)
          iCount++;
        else
          s--;
      }
      if (iCount) {
        x += iCount;  // skip these
        iCount = 0;
      }
    }
  } else {
    s = pDraw->pPixels;
    // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
    for (x = 0; x < pDraw->iWidth; x++) {
      doge_gfx->drawPixel(x + OffsetX, y + OffsetY, usPalette[*s++]);
      // dma_display.drawPixelRGB565(x, y, usPalette[*s++]);
    }
  }
}

bool dogeGIFOpen = false;

void OswAppCryptoTracker::setup(OswHal* hal) {
  doge_gfx = hal->gfx();
  Serial.println("stating gif player");
  gif_doge.begin(LITTLE_ENDIAN_PIXELS);
  if (gif_doge.open((uint8_t*)GIF_NAME, sizeof(GIF_NAME), GIFDrawDoge)) {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif_doge.getCanvasWidth(), gif_doge.getCanvasHeight());
    dogeGIFOpen = true;
  }
  hal->gfx()->fill(rgb565(0, 0, 0));
}

void OswAppCryptoTracker::loop(OswHal* hal) {
  if (hal->btn3Down()) {
        if (hal->getWiFi()->isConnected()) {
          hal->getWiFi()->disableWiFi();
          hal->requestFlush();
        } else {
          hal->getWiFi()->joinWifi();
        }
      }

      if (hal->getWiFi()->isConnected()) {
            Serial.println("Getting Data");
            hal->getWiFi()->get("http://192.168.178.81:1234/dogecoin", httpGetBufferDoge, HTTP_GET_BUF_LEN);
            price = String(httpGetBufferDoge);
      }


  hal->gfx()->setTextCursor(25, 165);
  hal->gfx()->setTextSize(4);
  hal->gfx()->setTextColor(rgb565(235, 155, 52), 0);
  hal->gfx()->print(price);
  
  if (dogeGIFOpen) {
    if (!gif_doge.playFrame(true, NULL)) {
      gif_doge.reset();

      
      
    }
  } else {
    hal->gfx()->setTextCursor(20, 200);
    hal->gfx()->print("GIF error!");
  }

  // this app has something to display, request a flush to the screen
  hal->requestFlush();
}

void OswAppCryptoTracker::stop(OswHal* hal) {
  if (dogeGIFOpen) {
    gif_doge.close();
  }
}
