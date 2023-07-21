#include "config.h"
#include <SPIFFS.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <AutoConnect.h>
#include <WiFi.h>
#include <ESP32Time.h>
#include <scroll.h>
#include <gif.h>
#include "utils.h"

/* Time */
ESP32Time esp32rtc;

/* Display */
MatrixPanel_I2S_DMA *dma_display = nullptr;
uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);

/* Wifi Manager */
WebServer webServer;
AutoConnect Portal(webServer);
AutoConnectConfig PortalConfig;
WiFiClient client;
HTTPClient http;

void showStatus(const String& status)
{
  dma_display->clearScreen();
  dma_display->setTextWrap(true);
  dma_display->setTextColor(myWHITE, myBLACK);
  dma_display->setTextSize(1);
  dma_display->setCursor(0, 0);
  dma_display->print(status);
  dma_display->setFont();
  dma_display->setTextWrap(false);
  delay(1500);
}

void stop()
{
  showStatus("Restaring");
  delay(5000);
  exit(1);
}

bool atDetect(IPAddress &softapIP)
{
  showStatus("Configure Wifi");
  showStatus("AP:MatrixPanel\nPASS:\n12345678");
  return true;
}

void setupDisplay()
{
  HUB75_I2S_CFG mxconfig(
      PANEL_RES_X,
      PANEL_RES_Y,
      PANEL_CHAIN);

  /* Fix Flickering */
  mxconfig.latch_blanking = 3;
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_15M;
  mxconfig.clkphase = false;
#ifdef ENABLE_DOUBLE_BUFFER
  mxconfig.double_buff = true;
#endif
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(90);
  dma_display->fillScreen(myRED);
  delay(200);
  dma_display->fillScreen(myGREEN);
  delay(200);
  dma_display->fillScreen(myBLUE);
  delay(200);
  dma_display->clearScreen();
}

void setupTime()
{
  showStatus("Syncing");
  const char *ntpServer = "time.cloudflare.com";
  struct tm timeinfo;
  const long gmtOffset_sec = 3600;
  const int daylightOffset_sec = 3600;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  if (!getLocalTime(&timeinfo))
  {
    showStatus("Failed to get time");
    stop();
  }
  else
  {
    esp32rtc.setTimeStruct(timeinfo);
  }
}

void setupNetworking()
{
  showStatus("Connecting");

  PortalConfig.apid = "MatixPanel";
  PortalConfig.title = "Configure WiFi";
  PortalConfig.password = "12345678";
  PortalConfig.menuItems = AC_MENUITEM_CONFIGNEW;

  Portal.config(PortalConfig);
  Portal.onDetect(atDetect);

  if (Portal.begin())
  {
    showStatus("Connected");
  }
  else
  {
    showStatus("Portal Error");
    stop();
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    showStatus("Connecting...");
  }
  showStatus(WiFi.localIP().toString());
  delay(1000);
}

void setupStorage()
{
  if (!SPIFFS.begin())
  {
    showStatus(F("SPIFFS Mount Failed"));
    stop();
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Serial Started"));

  setupDisplay();
  setupStorage();
  setupNetworking();
  setupTime();
  setupGIF();

  showStatus(F("Matrix OK"));
}

void loop()
{
  showGIF((char*)"/ww.gif", 1);
  char englishTime[100];
  getTimeEnglish(englishTime, esp32rtc.getHour(), esp32rtc.getMinute());
  String time("The time is ");
  time += englishTime;
  scrollText(time, myRED);
  // ShowGIF((char*)"/kill.gif", 3);
}
