#include <Adafruit_GFX.h>    
#include <Adafruit_ST7789.h> 
#include <SPI.h>

#include <Wire.h>
#include <BH1750.h>

#include <Adafruit_Sensor.h>                       
#include <DHT.h>
#include <DHT_U.h>

#include <Adafruit_BMP280.h>

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

#define BMP280_ADDRESS 0x76
Adafruit_BMP280 bmp;
//SCK (SCL Pin)	  - GPIO 22
//SDI (SDA pin)	  - GPIO 21

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

  // BMP280
  if (!bmp.begin(BMP280_ADDRESS)) {
    Serial.println(F("Could not find a valid BMP280 sensor!"));
    while (1) delay(10);
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
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

  // ===== BMP280 =====
  float bmpTemp = bmp.readTemperature();
  float bmpPressure = bmp.readPressure(); 
  float bmpAltitude = bmp.readAltitude(1013.25);

  tftPrintDisplayHeader();

  if (isnan(temperatureData) || isnan(humidityData)) {
    Serial.println("Error reading DHT11 temperature and humidity data");
  } 
  else if (lightData < 0) {
    Serial.println("Error reading BH1750 light data");
  } 
  else {
    serialPrintSensorData(temperatureData, humidityData, lightData, bmpTemp, bmpPressure, bmpAltitude);
    tftPrintSensorData(temperatureData, humidityData, lightData, bmpTemp, bmpPressure, bmpAltitude);
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

void serialPrintSensorData(float temperature, float humidity, float light, float bmpTemp, float pressure, float altitude){
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  Serial.print("Light: ");
  Serial.print(light);
  Serial.println("lx");

  Serial.print("BMP280 Temp: ");
  Serial.print(bmpTemp);
  Serial.println(" *C");

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" Pa");

  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println(" m");
}

void tftPrintSensorData(float temperature, float humidity, float light, float bmpTemp, float pressure, float altitude) {
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

  tft.print("BMP280: ");
  tft.print(bmpTemp);
  tft.println(" *C");

  tft.print("Press.: ");
  tft.print(pressure/100.0F);
  tft.println(" hPa");

  tft.print("Alt.: ");
  tft.print(altitude);
  tft.println(" m");
}
