#include <Adafruit_GFX.h>    
#include <Adafruit_ST7789.h> 
#include <SPI.h>

#include <Wire.h>
#include <BH1750.h>

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

BH1750 lightMeter;

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

  // I2C devices
  Wire.begin();
  lightMeter.begin();
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

    // ===== BH1750 =====
    float lightData = lightMeter.readLightLevel();

    tftPrintDisplayHeader();

    if (isnan(temperatureData) || isnan(humidityData)) {
      Serial.println("Error reading DHT11 temperature and humidity data");
    } 
    else if (lightData < 0) {
      Serial.println("Error reading BH1750 light data");
    } 
    else {
      serialPrintSensorData(temperatureData, humidityData, lightData);
      tftPrintSensorData(temperatureData, humidityData, lightData);
    }
    delay(5000);
}

void tftPrintDisplayHeader(){
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(70, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.println("Sensor Data");
}

void serialPrintSensorData(float temperature, float humidity, float light){
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  Serial.print("Light: ");
  Serial.print(light);
  Serial.println("lx");
}

void tftPrintSensorData(float temperature, float humidity, float light) {
  tft.setTextWrap(false);
  tft.setCursor(0, 35);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  tft.print("Temp.: ");
  tft.print(temperature);  
  tft.println(" *C");
  
  tft.print("Humid.: ");
  tft.print(humidity);
  tft.println(" %");

  tft.print("Light: ");
  tft.print(light);
  tft.println(" lx");
}
