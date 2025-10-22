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
uint32_t delayDHT;

#define BMP280_ADDRESS 0x76
Adafruit_BMP280 bmp;
//SCK (SCL Pin)	  - GPIO 22
//SDI (SDA pin)	  - GPIO 21

// ================= Buttons ===================
#define activateDisplayButton 34
bool lastButtonState = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;  // 50ms debounce period

// ================= Global ====================

// Define struct globally for DHT11 data
struct DHT11Data {
  float temperature;
  float humidity;
};

// Define struct globally for BH1750 data
struct BH1750Data{
  float light;
};

// Define struct globally for BMP280 data
struct BMP280Data{
  float temperature;
  float pressure; 
  float altitude;
};

// Define millis delay globally
unsigned long sensorReadDelay = 5000;
unsigned long previousMillis = 0;

DHT11Data lastDHT11Data = {NAN, NAN};
BH1750Data lastBH1750Data = {NAN};
BMP280Data lastBMP280Data = {NAN, NAN, NAN};


void setup(void) {

  Serial.begin(9600);
  Serial.print(F("Starting system..."));

  //Button 
  pinMode(activateDisplayButton, INPUT);

  // TFT
  tft.init(240, 280);
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(2);

  // DHT11
  dht.begin();  
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);      
  delayDHT = sensor.min_delay / 1000;  // Set minimum DHT11 sampling period (in ms)

  // I2C devices
  Wire.begin();

  // BH1750
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
  
  //Initial Sensor read
  updateSensorData();
}

void loop() {

  //Initiate millis delay
  unsigned long currentMillis = millis();
  if(currentMillis-previousMillis >= sensorReadDelay){
    previousMillis = currentMillis;
    updateSensorData();
  }

  if (readActivateDisplayButton())
  {
    tftPrintDisplayHeader();
    serialPrintSensorData(lastDHT11Data.temperature, lastDHT11Data.humidity, lastBH1750Data.light, lastBMP280Data.temperature, lastBMP280Data.pressure, lastBMP280Data.altitude);
    tftPrintSensorData(lastDHT11Data.temperature, lastDHT11Data.humidity, lastBH1750Data.light, lastBMP280Data.temperature, lastBMP280Data.pressure, lastBMP280Data.altitude);
  }
  else{
    tftBlankDisplay();
  }
}

void updateSensorData(){
  lastDHT11Data = readDHT11Data();
  lastBH1750Data = readBH1750Data();
  lastBMP280Data = readBMP280Data();
}

bool readActivateDisplayButton(){
  bool reading = digitalRead(activateDisplayButton)==HIGH;

  if (lastButtonState != reading){
    lastDebounceTime = millis();
  }
  if(millis()-lastDebounceTime>debounceDelay){
    lastButtonState = reading;
  }
  return lastButtonState;
  
}

DHT11Data readDHT11Data(){
  sensors_event_t tempEvent;
  sensors_event_t humidEvent;
  DHT11Data data;

  dht.temperature().getEvent(&tempEvent);
  dht.humidity().getEvent(&humidEvent);

  float temperatureData = tempEvent.temperature;
  float humidityData = humidEvent.relative_humidity;
  if (isnan(temperatureData) || isnan(humidityData)) {
    Serial.println("Error reading DHT11 temperature and humidity data");
    data.temperature = NAN;
    data.humidity = NAN;
  } 

  data.temperature = temperatureData;
  data.humidity = humidityData;

  return data;
}

BH1750Data readBH1750Data(){
  float lightData = lightMeter.readLightLevel();
  BH1750Data data;
  if (lightData < 0) {
    Serial.println("Error reading BH1750 light data");
    data.light = NAN;
  } 
  data.light = lightData;

  return data;
}

BMP280Data readBMP280Data(){
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure(); 
  float altitude = bmp.readAltitude(1013.25);
  BMP280Data data;

  data.temperature = temperature;
  data.pressure = pressure;
  data.altitude = altitude;

  return data;
}

void tftPrintDisplayHeader(){
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(70, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.println("Sensor Data");
}

void tftBlankDisplay(){
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(0);
  tft.println(" ");
}

void serialPrintSensorData(float temperature, float humidity, float light, float bmpTemp, float pressure, float altitude){
  Serial.print(F("Temperature: "));
  Serial.print(temperature);
  Serial.println(F(" *C"));

  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.println(F("%"));

  Serial.print(F("Light: "));
  Serial.print(light);
  Serial.println(F("lx"));

  Serial.print(F("BMP280 Temp: "));
  Serial.print(bmpTemp);
  Serial.println(F(" *C"));

  Serial.print(F("Pressure: "));
  Serial.print(pressure);
  Serial.println(F(" Pa"));

  Serial.print(F("Altitude: "));
  Serial.print(altitude);
  Serial.println(F(" m"));
}

void tftPrintSensorData(float temperature, float humidity, float light, float bmpTemp, float pressure, float altitude) {
  tft.setTextWrap(false);
  tft.setCursor(0, 35);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  tft.print(F("Temp.: "));
  tft.print(temperature);  
  tft.println(F(" *C"));
  
  tft.print(F("Humid.: "));
  tft.print(humidity);
  tft.println(F(" %"));

  tft.print(F("Light: "));
  tft.print(light);
  tft.println(F(" lx"));

  tft.print(F("BMP280: "));
  tft.print(bmpTemp);
  tft.println(F(" *C"));

  tft.print(F("Press.: "));
  tft.print(pressure / 100.0F);
  tft.println(F(" hPa"));

  tft.print(F("Alt.: "));
  tft.print(altitude);
  tft.println(F(" m"));
}
