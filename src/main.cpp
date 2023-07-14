#include <SPIFFS.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <gif.h>
#include <font.h>
#include <AutoConnect.h>
#include <WiFi.h>
#include <AutoConnect.h>
#include <ESP32Time.h>
#include "time.h"

#define FILESYSTEM SPIFFS

/* Time */
ESP32Time esp32rtc; 

/* Display */
#define PANEL_RES_X 64
#define PANEL_RES_Y 32
#define PANEL_CHAIN 1
MatrixPanel_I2S_DMA *dma_display = nullptr;
uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);
uint16_t myDarkRED = dma_display->color565(50, 0, 30);

/* Wifi Manager */
WebServer webServer;
AutoConnect Portal(webServer);
AutoConnectConfig PortalConfig;
WiFiClient client;
HTTPClient http;

void showStatus(String status)
{
  // int16_t  x1, y1;
  // uint16_t w, h;
  dma_display->setTextWrap(true);
  dma_display->setTextColor(myGREEN, myBLACK);
  dma_display->setFont(&Font4x5Fixed);
  dma_display->setTextSize(1);
  dma_display->setCursor(0, 4);
  // dma_display->getTextBounds(status, 0, 5, &x1, &y1, &w, &h);
  dma_display->fillRect(0, 0, 64, 32, myDarkRED);
  dma_display->print(status);
  delay(1500);
}

void stop()
{
  showStatus("Restaring...");
  delay(20000);
  exit(1);
}

bool atDetect(IPAddress &softapIP)
{
  showStatus("Configure Wifi");
  showStatus("Connect to\nMatrixPanel\nPassword\n12345678");
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

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(128);
  dma_display->fillScreen(myWHITE);
  delay(500);
  dma_display->clearScreen();
}

void setupTime()
{
  showStatus("Syncing time");
  const char *ntpServer = "pool.ntp.org";
  struct tm timeinfo;
  const long gmtOffset_sec = 3600;
  const int daylightOffset_sec = 3600;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  if (!getLocalTime(&timeinfo))
  {
    showStatus("Failed to obtain time");
    esp32rtc.setTime(1609459200);
  }
  else
  {
    esp32rtc.setTimeStruct(timeinfo);
  }
}

void setupNetworking()
{
  showStatus("Connecting...");

  PortalConfig.apid = "MatixPanel";
  PortalConfig.title = "Configure WiFi";
  PortalConfig.password = "12345678";
  PortalConfig.menuItems = AC_MENUITEM_CONFIGNEW;

  Portal.config(PortalConfig);
  Portal.onDetect(atDetect);

  if (Portal.begin())
  {
    showStatus("WIFI Connected");
  }
  else
  {
    showStatus("Portal Error.");
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

void setupStorage() {
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
  
  setupStorage();
  setupDisplay();
  setupNetworking();
  setupTime();
  GIFSetup();

  showStatus(F("Matrix OK"));
  dma_display->fillScreen(myBLACK);
}

void loop()
{
  showStatus(esp32rtc.getTime("%A,\n%B %d %Y \n%H:%M:%S"));
}
