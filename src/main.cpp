#include "config.h"
#include "utils.h"

#include <SPIFFS.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <AutoConnect.h>
#include <WiFi.h>
#include <ESP32Time.h>
#include <ElegantOTA.h>
#include <WebServer.h>
#include <HTTPClient.h>

#include "scroll_text.h"
#include "show_text.h"
#include "show_gif.h"

#include "./aurora/Playlist.h"

uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);

String apName = "Matrix";
String apPassword = "12345678";

int buttonPin = 11;
int loopCounter = 0;

MatrixPanel_I2S_DMA *dma_display = nullptr;
GFXcanvas16 *canvas = nullptr;
Playlist *playlist = nullptr;

ESP32Time esp32rtc;

WebServer webServer;
AutoConnect Portal(webServer);
AutoConnectConfig PortalConfig;
WiFiClient client;
HTTPClient http;

void showStatus(const String &status)
{
  dma_display->clearScreen();
  dma_display->setTextWrap(true);
  dma_display->setTextColor(myWHITE, myBLACK);
  dma_display->setTextSize(1);
  dma_display->setCursor(0, 0);
  dma_display->print(status);
  dma_display->setFont();
  dma_display->setTextWrap(false);
#ifdef S2_MINI_DEBUG
  Serial.println(status);
#endif
  delay(1000);
}

void restart()
{
  showStatus(F("Restaring"));
  delay(5000);
  ESP.restart();
}

bool atDetect(IPAddress &softapIP)
{
  showStatus(F("Configure Wifi"));
  showStatus(apName);
  showStatus(apPassword);
  return true;
}

void setupDisplay()
{
  HUB75_I2S_CFG::i2s_pins _pins_x1 = {
      WF1_R1_PIN, WF1_G1_PIN, WF1_B1_PIN, WF1_R2_PIN, WF1_G2_PIN,
      WF1_B2_PIN, WF1_A_PIN, WF1_B_PIN, WF1_C_PIN, WF1_D_PIN,
      WF1_E_PIN, WF1_LAT_PIN, WF1_OE_PIN, WF1_CLK_PIN};

  HUB75_I2S_CFG mxconfig(
      PANEL_RES_X,
      PANEL_RES_Y,
      PANEL_CHAIN,
      _pins_x1);

  /* Fix Flickering */
  mxconfig.latch_blanking = 3;
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_15M; // HZ_20M 15/120
  mxconfig.clkphase = false;
  mxconfig.min_refresh_rate = 120;

#ifdef ENABLE_DOUBLE_BUFFER
  mxconfig.double_buff = true;
#endif
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  canvas = new GFXcanvas16(PANEL_RES_X, PANEL_RES_Y);
  dma_display->begin();
  dma_display->setBrightness8(128);
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
  showStatus(F("Syncing"));
  const char *ntpServer = "time.cloudflare.com";
  struct tm timeinfo;
  const long gmtOffset_sec = 3600;
  const int daylightOffset_sec = 3600;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  if (!getLocalTime(&timeinfo))
  {
    showStatus(F("Failed to get time"));
    restart();
  }
  else
  {
    esp32rtc.setTimeStruct(timeinfo);
  }
}

void checkUpdateMode()
{
#ifdef S2_MINI_DEBUG
#pragma message "!!! UPDATE MODE DISABLED"
  Serial.println("...");
  return;
#endif
  if (digitalRead(buttonPin) == LOW)
  {
    webServer.on("/", []()
                 { webServer.send(200, "text/plain", apName); });
    ElegantOTA.begin(&webServer);
    webServer.begin();
    showStatus(WiFi.localIP().toString());
    while (true)
    {
      webServer.handleClient();
    }
  }
}

void setupNetworking()
{
  showStatus(F("Connecting"));

  uint8_t a = rand() % 256;
  uint8_t b = rand() % 256;
  uint8_t c = rand() % 256;
  uint8_t newMACAddress[] = {0x32, 0xAE, 0xA4, a, b, c};
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);

  PortalConfig.apid = apName;
  PortalConfig.password = apPassword;
  PortalConfig.title = "Configure WiFi";
  PortalConfig.menuItems = AC_MENUITEM_CONFIGNEW | AC_MENUITEM_DELETESSID | AC_MENUITEM_DEVINFO | AC_MENUITEM_UPDATE | AC_MENUITEM_OPENSSIDS;
  PortalConfig.autoSave = AC_SAVECREDENTIAL_ALWAYS;
  PortalConfig.autoReconnect = true;

  Portal.config(PortalConfig);
  Portal.onDetect(atDetect);

  if (Portal.begin())
  {
    showStatus(F("Connected"));
  }
  else
  {
    showStatus(F("Portal Error"));
    restart();
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    showStatus(F("..."));
  }
  showStatus(WiFi.macAddress());
  showStatus(WiFi.localIP().toString());
  showStatus("Done");
}

void setupStorage()
{
  if (!SPIFFS.begin())
  {
    showStatus(F("SPIFFS Mount Failed"));
    restart();
  }
}

void setup()
{
  Serial.begin(115200);
  delay(5000);
  Serial.println(F("Serial Started"));
  pinMode(buttonPin, INPUT);
  randomSeed(esp_random());
  setupDisplay();
  setupNetworking();

  checkUpdateMode();

  setupStorage();
  setupTime();
  setupGIF();

  playlist = new Playlist();
  playlist->setup();

  showStatus(F("Matrix OK"));
}

void show_jokes()
{
  dma_display->clearScreen();
  HTTPClient http;
  char url[] = "http://ssh.sonicpix.ro/matrix/read.php?screen=joke";
  http.begin(url);
  http.setTimeout(5000);

  int httpResponseCode = http.GET();

  if (httpResponseCode == 200)
  {
    String payload = http.getString();
    show_text(payload);
    show_random_numbered_gif((char *)"j", 5);
  }
  else
  {
    show_text(http.errorToString(httpResponseCode));
  }
  http.end();
}

void show_music()
{
  dma_display->clearScreen();
  HTTPClient http;
  char url[] = "http://ssh.sonicpix.ro/matrix/read.php?screen=music";
  http.begin(url);
  http.setTimeout(5000);

  int httpResponseCode = http.GET();

  if (httpResponseCode == 200)
  {
    String payload = http.getString();
    show_text(payload);
    show_random_numbered_gif((char *)"m", 4);
  }
  else
  {
    show_text(http.errorToString(httpResponseCode));
  }
  http.end();
}

void show_custom_text_scroll()
{
  dma_display->clearScreen();
  HTTPClient http;
  char url[] = "http://ssh.sonicpix.ro/matrix/read.php?screen=custom";
  http.begin(url);
  http.setTimeout(5000);

  int httpResponseCode = http.GET();

  if (httpResponseCode == 200)
  {
    String payload = http.getString();
    scroll_text(payload, getRandomColor());
    show_random_numbered_gif((char *)"w", 4);
  }
  else
  {
    show_text(http.errorToString(httpResponseCode));
  }
  http.end();
}

void show_time()
{
  char englishTime[100];
  char final[150];
  getTimeEnglish(englishTime, esp32rtc.getHour(), esp32rtc.getMinute());
  sprintf(final, " ... The time is %s ... ", englishTime);
  show_text(final);
  show_random_numbered_gif((char *)"t", 4);
}

void checkNetworking()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    show_text("NOT CONNECTED");
    restart();
  }
  checkUpdateMode();
}

void loop()
{
  switch (loopCounter)
  {
  case 0:
    show_custom_text_scroll();
    break;
  case 1:
    show_time();
    break;
  case 2:
    show_music();
    break;
  case 3:
    show_jokes();
    break;
  }
  checkNetworking();
  loopCounter++;
  if (loopCounter > 3)
  {
    loopCounter = 0;
  }
}
