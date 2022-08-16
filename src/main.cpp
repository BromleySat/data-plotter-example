#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#define DEVICE_ID "Data Plotter Example"
#define MAX_JSON_LENGTH 250
#define LOOP_DELAY 200

const char *SSID = "<your-Wifi-SSID>";
const char *PWD = "<your-Wifi-passoword>";

String serverPath = "https://bromleysat.space/data/index.html";
WebServer webServer(80);
StaticJsonDocument<MAX_JSON_LENGTH> jsonDocument;
char buffer[MAX_JSON_LENGTH];

int voltage = 0;
float temperature = 0;
unsigned int waterLevel = 0;
unsigned int relayOn = 0;

void getDataPlotterSite()
{
  HTTPClient http;
  String payload = "";

  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  webServer.send(200, "text/html", payload);
}

void getData()
{
  jsonDocument.clear();
  jsonDocument["Voltage"] = voltage;
  jsonDocument["Temperature"] = temperature;
  jsonDocument["WaterLevel"] = waterLevel;
  jsonDocument["RelayIsOn"] = relayOn;
  serializeJson(jsonDocument, buffer);
  webServer.send(200, "application/json", buffer);
}

void getConfig()
{
  jsonDocument.clear();
  jsonDocument["deviceId"] = DEVICE_ID;
  serializeJson(jsonDocument, buffer);
  webServer.send(200, "application/json", buffer);
}

void readSensorData()
{
  voltage = random(-248, 248);
  temperature = random(-120, 400) / 100.0;
  waterLevel = random(0, 100);
  relayOn = random(0, 1);
}

void connectToWiFi()
{
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PWD);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

void configureRoutes()
{
  webServer.on("/", getDataPlotterSite);
  webServer.on("/api/data", getData);
  webServer.on("/api/config", getConfig);
  webServer.enableCORS();
  webServer.begin();
}

void setup()
{
  Serial.begin(9600);
  connectToWiFi();
  configureRoutes();
}

void loop()
{
  readSensorData();
  webServer.handleClient();
  delay(LOOP_DELAY);
}
