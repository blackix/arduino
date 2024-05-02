#include <SPI.h>
#include "epd1in54_V2.h"
#include "imagedata.h"
#include "epdpaint.h"
#include <DHT.h>
#include <MQ135.h>

MQ135 mq135_sensor(A1);

float PPM;

#define DHTPIN 2      // Il pin di Arduino a cui è collegato il sensore DHT11
#define DHTTYPE DHT11 // Specifica il tipo di sensore DHT

DHT dht(DHTPIN, DHTTYPE);

Epd epd;
unsigned char image[1024];
Paint paint(image, 0, 0);  // L'inizializzazione qui non specifica ancora le dimensioni

#define COLORED     0
#define UNCOLORED   1

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  Serial.println("e-Paper init and clear");
  epd.LDirInit();
  epd.Clear();

  // Imposta la larghezza e l'altezza dell'area di disegno
  paint.SetWidth(200);  // Larghezza massima dell'immagine
  paint.SetHeight(20);  // Aumento dell'altezza per evitare la sovrapposizione delle righe di testo
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  PPM = mq135_sensor.getCorrectedPPM(temperature, humidity);


  // Verifica se le letture sono valide
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Pulisce l'intero buffer per assicurare che non ci siano dati residui
  paint.Clear(UNCOLORED);

  // // Prepara la stringa della temperatura e disegnala
   String tempStr = "Temp: " + String(temperature, 1) + " C";
   paint.DrawStringAt(0, 0, tempStr.c_str(), &Font24, COLORED);
   epd.SetFrameMemory(paint.GetImage(), 0, 10, paint.GetWidth(), paint.GetHeight());

  // // Pulisce nuovamente il buffer prima di disegnare la stringa dell'umidità
  // paint.Clear(UNCOLORED);
paint.Clear(UNCOLORED);

  // Prepara la stringa dell'umidità e disegnala
  String humStr = "Hum: " + String(humidity, 1) + " %";
  paint.DrawStringAt(0, 0, humStr.c_str(), &Font24, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 60, paint.GetWidth(), paint.GetHeight());

paint.Clear(UNCOLORED);

  // Prepara la stringa dell'umidità e disegnala
  String PartPerMillion = "PPM: " + String(PPM, 1);
  paint.DrawStringAt(0, 0, PartPerMillion.c_str(), &Font16, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 100, paint.GetWidth(), paint.GetHeight());
  // Aggiorna il display
  epd.DisplayFrame();

  // Attesa tra le letture
  delay(10000);
}