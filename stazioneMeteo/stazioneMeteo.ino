#include <SPI.h>
#include "epd1in54_V2.h"
#include "imagedata.h"
#include "epdpaint.h"
#include <stdio.h>
 
#include "gui.c"
 

 
GxIO_Class io(SPI, 10, 9, 8);   // CREO SERIALE PER E-INK -- CS DC RST
GxEPD_Class display(io, 8, 7);  // CREO DISPLAY PER E-INK -- Seriale RST BUSY
 
 
// WIFI E TELEGRAM
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
 
const char* ssid = "EVENTI";
const char* password = "Eventi2021";
 
#define BOTToken "7153347048:AAFdFQwXvd4UDSOetacqR9NK4p1Cf_Pla3I"
#define CHAT_ID "501061639"
 
WiFiClientSecure client;
UniversalTelegramBot bot(BOTToken, client);
 
 
// SENSORI
#include "DHT.h"
#include "MQ135.h"
 
MQ135 mq135_sensor(A1);
DHT dht(2, DHT11);
 
float t, h, PPM;
 
 
 
void setup() {
  Serial.begin(115200);
  display.init();
  display.setRotation(2);
 
  dht.begin();
 
  WiFi.mode(WIFI_STA);
 
}
 
 
void loop() {
  lettura_sensori();
  stampa_valori();
  messaggio();
  delay(1000);
}
 

 
void lettura_sensori() {
  t = dht.readTemperature();
  h = dht.readHumidity();
  PPM = mq135_sensor.getCorrectedPPM(t, h);
}
 
 
void stampa_valori() {
  display.setRotation(3);
  display.fillRect(65,0,135,200, GxEPD_WHITE);
 
  display.setFont(&FreeSansBold24pt7b);
  display.setTextColor(GxEPD_BLACK);
 
  display.setCursor(65,50);
  display.print(String(t,1));
 
  display.setCursor(65,115);
  display.print(String(h,0) + "%");
 
  display.setCursor(65,180);
  if (PPM > 100000) display.setFont(&FreeSansBold18pt7b);
  display.print(String(PPM,0));
 
  display.update();
}
 
 
void messaggio() {
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connesso");
 
  bot.sendMessage(CHAT_ID, "\xf0\x9f\x8c\xa1 " + String(t,1) + "°C \n \xf0\x9f\x92\xa7 " + String(h,0) + "% \n \xf0\x9f\x92\xa8 " + String(PPM,0) + " PPM");  //invia il messaggio
  
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}
 
