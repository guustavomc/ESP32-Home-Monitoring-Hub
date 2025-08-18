#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#include <Adafruit_Sensor.h>                       // Biblioteca DHT Sensor Adafruit 
#include <DHT.h>
#include <DHT_U.h>

#define TFT_CS         14
#define TFT_RST        15
#define TFT_DC         32

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

float p = 3.1415926;

// selecione um sensor, retirando o comentário - duas barras
#define DHTTYPE    DHT11                           // Sensor DHT11
//#define DHTTYPE      DHT22                       // Sensor DHT22 ou AM2302
#define DHTPIN 2                                   // Pino do Arduino conectado no Sensor(Data) 
DHT_Unified dht(DHTPIN, DHTTYPE);                  // configurando o Sensor DHT - pino e tipo
uint32_t delayMS;                                  // variável para atraso no tempo

void setup(void) {

  Serial.begin(9600);
  Serial.print(F("Hello! ST77xx TFT Test"));

  tft.init(240, 280);           // Init ST7789 280x240
  dht.begin();  
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);           // imprime os detalhes do Sensor de Temperatura
  dht.humidity().getSensor(&sensor);            // imprime os detalhes do Sensor de Umidade
  //tft.setSPISpeed(40000000);

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;
}

void loop() {
    delay(delayMS);

    sensors_event_t tempEvent;
    sensors_event_t humidEvent;

    dht.temperature().getEvent(&tempEvent);   // read temperature
    dht.humidity().getEvent(&humidEvent);     // read humidity

    float temperatureData = tempEvent.temperature;
    float humidityData = humidEvent.relative_humidity;
    if (isnan(temperatureData)) {
      Serial.println("Error reading temperature data");
    } 
    else if (isnan(humidityData)) {
      Serial.println("Error reading humidity data");
    } 
    else {
      Serial.print("Temperature: ");
      Serial.print(temperatureData);
      Serial.println(" *C");

      Serial.print("Humidity: ");
      Serial.print(humidityData);
      Serial.println("%");

      tftPrintTest(temperatureData, humidityData);
    }
    delay(5000);
}


void tftPrintTest(float temperature, float humidity) {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(100, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.println("DATA");
  tft.setCursor(0, 35);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print(temperature);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print(humidity);
}
