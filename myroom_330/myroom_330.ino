#include <ESP8266WiFi.h>
#include <Wire.h>
#include "DHT.h"  
#define DEBUG
#define DEBUG_PRINTER Serial
#ifdef DEBUG
#define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define DEBUG_PRINT(...) {}
#define DEBUG_PRINTLN(...) {}
#endif

#define DHTPIN 2      
#define DHTTYPE DHT22 
float Temperature, Humidity; 
//อันนี้เน็ตครับ

const char* ssid     = "NAT.WRTNODE";
const char* password = "devicenetwork";//แก้ด้วยครับ
float temp;

void connectWifi();
void reconnectWifiIfLinkDown();
void uploadThingsSpeak(float t, float H, float M);

DHT dht(DHTPIN, DHTTYPE, 30);

int moisture = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin();  
    delay(10);
    
    dht.begin();
    
    connectWifi();
    delay(100);                
}
 
void loop() {
    read_Sensor();
    uploadThingsSpeak(Temperature, Humidity, moisture);
    reconnectWifiIfLinkDown(); 
}

void read_Sensor(){                   
   Temperature = dht.readTemperature();
   Humidity = dht.readHumidity();
   moisture = map(analogRead(A0), 650, 1023, 10, 0);
  }
  
void uploadThingsSpeak(float t, float H, float M) {
    static const char* host = "api.thingspeak.com";
    static const char* apiKey = "5LLH3VK79YMT0YLU";
 
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
    url += "?key=";
    url += apiKey;
    url += "&field3=";
    url += M;
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
