#include <Adafruit_GFX.h>    
#include <Adafruit_ST7789.h>
#include <SPI.h>

#include <Adafruit_Sensor.h>                       
#include <DHT.h>
#include <DHT_U.h>

// ================= TFT Pins ==================

#define TFT_CS         14
#define TFT_RST        15
#define TFT_DC         32

//SCL Pin	          - GPIO 18
//MOSI (SDA pin)	  - GPIO 23

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// ================= Sensors ===================

#define DHTTYPE    DHT11                           
#define DHTPIN 2                                   
DHT_Unified dht(DHTPIN, DHTTYPE);  

// ================= Global ====================

uint32_t delayMS;                                  

void setup(void) {

  Serial.begin(9600);
  Serial.print(F("Starting system..."));

  // TFT
  tft.init(240, 280); 
  tft.fillScreen(ST77XX_BLACK);


  // DHT11
  dht.begin();  
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);           
  dht.humidity().getSensor(&sensor);            

}

void loop() {
    delay(delayMS);

    // ===== DHT11 =====
    sensors_event_t tempEvent;
    sensors_event_t humidEvent;

    dht.temperature().getEvent(&tempEvent);   
    dht.humidity().getEvent(&humidEvent);     

    float temperatureData = tempEvent.temperature;
    float humidityData = humidEvent.relative_humidity;

    tftPrintHeader();

    if (isnan(temperatureData) || isnan(humidityData)) {
      Serial.println("Error reading DHT11 data");
    }
    else {
      serialPrintSensorData(temperatureData, humidityData);
      tftPrintTest(temperatureData, humidityData);
    }
    delay(5000);
}

void tftPrintHeader(){
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(100, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.println("Data");
}

void serialPrintSensorData(float temperature, float humidity){
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
}


void tftPrintTest(float temperature, float humidity) {
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
}
