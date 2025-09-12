#include <Adafruit_GFX.h>    
#include <Adafruit_ST7789.h> 
#include <SPI.h>

#include <Wire.h>
#include <BH1750.h>

#include <Adafruit_Sensor.h>                       
#include <DHT.h>
#include <DHT_U.h>

#define TFT_CS         14
#define TFT_RST        15
#define TFT_DC         32
//MOSI == SDA pin 23
//SCL pin 18

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

BH1750 lightMeter;

#define DHTTYPE    DHT11                           
#define DHTPIN 2                                   
DHT_Unified dht(DHTPIN, DHTTYPE);                  
uint32_t delayMS;                                  

void setup(void) {

  Serial.begin(9600);
  Serial.print(F("Hello! ST77xx TFT Test"));
  Wire.begin();
  tft.init(240, 280);
  dht.begin();  
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);           

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;
}

void loop() {
    delay(delayMS);

    sensors_event_t tempEvent;
    sensors_event_t humidEvent;

    dht.temperature().getEvent(&tempEvent);
    dht.humidity().getEvent(&humidEvent);

    float temperatureData = tempEvent.temperature;
    float humidityData = humidEvent.relative_humidity;

    tft.setTextWrap(false);
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(100, 0);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.println("Data");

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

      tftPrintTest(temperatureData, humidityData, lightMeter.readLightLevel());
    }
    delay(5000);
}


void tftPrintTest(float temperature, float humidity, float lux) {
  tft.setTextWrap(false);
  tft.setCursor(0, 35);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print("Temperature: ");
  tft.println(temperature);  
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print("Humidity: ");
  tft.println(humidity);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print("Light: ");
  tft.println(lux);
}
