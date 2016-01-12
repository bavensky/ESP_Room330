#include <ESP8266WiFi.h>
#include <Wire.h>
#include "DHT.h"
//#include <OneWire.h>
//#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <ESP_Adafruit_SSD1306.h>

//  WIFI initiate
#define DEBUG
#define DEBUG_PRINTER Serial
#ifdef DEBUG
#define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define DEBUG_PRINT(...) {}
#define DEBUG_PRINTLN(...) {}
#endif
//

//  DHT initiate
#define DHTPIN 12
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE, 30);
float Temperature, Humidity;
//

//  DS18B20 initiate
//#define ONE_WIRE_BUS 0
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
//float ds;
//

//  Connect WIFI
//  Connect wifi for room330
//const char* ssid     = "tong";
//const char* password = "man09min";
//  Connect wifi for CMMakerClub
const char* ssid     = "NAT.WRTNODE";
const char* password = "devicenetwork";


float temp;
void connectWifi();
void reconnectWifiIfLinkDown();
void uploadThingsSpeak(float t, float H, float M, float W);

//  OLED  initiate
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// global variable
int moisture = 0;
unsigned long previousMillis = 0;
const long interval = 60000; // update about 1 minute

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(16, OUTPUT);
  dht.begin();      // DHT22 begin
//  sensors.begin();  // DS18B20 begin
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x78 >> 1); // OLED Begin
  
  connectWifi();  //  Connect WiFI
  delay(10); 
}

void loop() {
  yield(); // debug loop
  
  read_Sensor();
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    uploadThingsSpeak(Temperature, Humidity);
//    uploadThingsSpeak(Temperature, Humidity, moisture, ds);
    reconnectWifiIfLinkDown();
    previousMillis = currentMillis;
    digitalWrite(16, LOW);
  }
  
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.println("Weather");
  display.print("   Station");
  display.setTextSize(1);
  display.print("IP : ");
  display.println(WiFi.localIP());
  display.print("Temp = ");
  display.print(Temperature);
  display.println(" C");
  display.print("Humi = ");
  display.print(Humidity);
  display.println(" %RH");
//  display.print("Moisture Level = ");
//  display.println(moisture);
//  display.print("Water Temp = ");
//  display.println(" Not Now");
  display.display();
  digitalWrite(16, HIGH);
  delay(100);
  display.clearDisplay();
}

void read_Sensor() {
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
  int sum;
  for(int i=0; i<1000; i++)  {
    sum = map(analogRead(A0), 650, 1023, 10, 0);
    moisture += sum;
  }
  moisture = moisture/100;
//  sensors.requestTemperatures();
//  ds = sensors.getTempCByIndex(0);
}
//void uploadThingsSpeak(float t, float H, float M, float W) 
void uploadThingsSpeak(float t, float H) {
  static const char* host = "api.thingspeak.com";
  static const char* apiKey = "RL4UFEICOVEIYIDX"; // Room --> 1DYAF3DO3X8GR6JS  CMMC --> RL4UFEICOVEIYIDX 
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    DEBUG_PRINTLN("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/update/";
  //  url += streamId;
  //-----------------------------------------------
  url += "?key=";
  url += apiKey;
  url += "&field1=";
  url += t;
  //-----------------------------------------------
  url += "?key=";
  url += apiKey;
  url += "&field2=";
  url += H;
  //-----------------------------------------------
//  url += "?key=";
//  url += apiKey;
//  url += "&field3=";
//  url += M;
  //-----------------------------------------------
  //-----------------------------------------------
//  url += "?key=";
//  url += apiKey;
//  url += "&field4=";
//  url += W;
  //-----------------------------------------------
  DEBUG_PRINT("Requesting URL: ");
  DEBUG_PRINTLN(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
}

void reconnectWifiIfLinkDown() {
  if (WiFi.status() != WL_CONNECTED) {
    DEBUG_PRINTLN("WIFI DISCONNECTED");
    connectWifi();
  }
}

void connectWifi() {
  DEBUG_PRINTLN();
  DEBUG_PRINTLN();
  DEBUG_PRINT("Connecting to ");
  DEBUG_PRINTLN(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUG_PRINT(".");
  }

  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("WiFi connected");
  DEBUG_PRINTLN("IP address: ");
  DEBUG_PRINTLN(WiFi.localIP());
}
