#include <Adafruit_GFX.h>    
#include <Adafruit_ST7789.h> 
#include <SPI.h>

#include <Wire.h>
#include <BH1750.h>

#include <Adafruit_Sensor.h>                       
#include <DHT.h>
#include <DHT_U.h>

#include <Adafruit_BMP280.h>

#include "config.h"
#include <WiFi.h>
#include <EspMQTTClient.h>
#include <time.h>

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

EspMQTTClient mqttClient(
  WIFI_SSID,
  WIFI_PASSWORD,
  MQTT_BROKER,
  "ESP32SensorHub"
);

// ================= Global ====================

struct MainData {
  float temperature;
  float humidity;
  float light;
  float pressure;
  float altitude;
};

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

uint8_t displayMode = 0;          // 0 = Main, 1 = DHT11, 2 = BH1750, 3 = BMP280
const uint8_t MAX_MODES = 4;

// Buttons
#define activateDisplayButton 34
#define changeDisplayInformation 33

// Define millis delay globally
unsigned long sensorReadDelay = 5000;
unsigned long previousMillis = 0;
bool newSensorData = false;  // Tracks if new sensor data is available
bool currentDisplayState = false;  // Tracks if display is currently active


MainData lastMainData = {NAN, NAN, NAN, NAN, NAN};
DHT11Data lastDHT11Data = {NAN, NAN};
BH1750Data lastBH1750Data = {NAN};
BMP280Data lastBMP280Data = {NAN, NAN, NAN};

void onConnectionEstablished() {
  Serial.println("MQTT broker connected");
}

String getTimestamp() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "1970-01-01T00:00:00Z";
  char buf[25];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(buf);
}

void publishSensorData() {
  if (!mqttClient.isConnected()) return;

  String payload = "{";
  payload += "\"timestamp\":\"" + getTimestamp() + "\",";
  payload += "\"temperature\":" + String(lastMainData.temperature, 1) + ",";
  payload += "\"humidity\":" + String(lastMainData.humidity, 1) + ",";
  payload += "\"light\":" + String(lastMainData.light, 1) + ",";
  payload += "\"pressure\":" + String(lastMainData.pressure / 100.0F, 2) + ",";
  payload += "\"altitude\":" + String(lastMainData.altitude, 1);
  payload += "}";

  mqttClient.publish(MQTT_TOPIC, payload);
  Serial.println("MQTT published: " + payload);
}

void setup(void) {

  Serial.begin(9600);
  Serial.print(F("Starting system..."));

  //Button 
  pinMode(activateDisplayButton, INPUT);// GPIO34 = input-only
  pinMode(changeDisplayInformation, INPUT);// GPIO33 = input-only


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

  configTime(NTP_UTC_OFFSET_SEC, 0, "pool.ntp.org");
  Serial.println("Waiting for NTP sync...");

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
  updateIndividualSensorData();
  newSensorData = true;  // Flag initial data

}

void loop() {
  mqttClient.loop();

  unsigned long currentMillis = millis();

  // ---- read sensors on a fixed interval ----
  if(currentMillis-previousMillis >= max(sensorReadDelay, delayDHT)){
    previousMillis = currentMillis;
    updateIndividualSensorData();
    publishSensorData(); 
    newSensorData = true;
  }

  // ---- BUTTON 1: activate / deactivate display ----
  if (checkButtonPress()) {
    currentDisplayState = !currentDisplayState;
    if (currentDisplayState) {
      displayMode = 0;
      tftPrintCurrentPage();
      serialPrintSensorData();
    } 
    else {
      tftBlankDisplay();
    }
  }

  // ---- BUTTON 2: change information page ----
  if (currentDisplayState && checkChangeButtonPress()) {
    displayMode = (displayMode + 1) % MAX_MODES;
    tftPrintCurrentPage();
  }

  // ---- refresh display only when new sensor data arrives ----
  if (currentDisplayState && newSensorData) {
    tftPrintCurrentPage();
    serialPrintSensorData();
    newSensorData = false;
  }
}

void updateIndividualSensorData(){
  lastDHT11Data = readDHT11Data();
  lastBH1750Data = readBH1750Data();
  lastBMP280Data = readBMP280Data();
  updateMainData();
}

void updateMainData(){
  lastMainData.temperature = (lastDHT11Data.temperature+lastBMP280Data.temperature) / 2;
  lastMainData.humidity = lastDHT11Data.humidity;
  lastMainData.light = lastBH1750Data.light;
  lastMainData.pressure = lastBMP280Data.pressure;
  lastMainData.altitude = lastBMP280Data.altitude;
}

bool checkButtonPress() {
  static bool lastStableState = LOW;       // Last stable state
  static bool lastReading = LOW;           // Last raw reading
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 150; // Increase debounce delay

  bool currentReading = digitalRead(activateDisplayButton);

  // If the state changed since the last reading, reset the timer
  if (currentReading != lastReading) {
    lastDebounceTime = millis();
  }

  // Enough time has passed and the reading differs from the last stable state
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (currentReading != lastStableState) {
      lastStableState = currentReading;

      // Detect rising edge (LOW -> HIGH)
      if (lastStableState == HIGH) {
        return true;
      }
    }
  }

  lastReading = currentReading;
  return false;
}

bool checkChangeButtonPress() {
  static bool lastStableState = LOW;
  static bool lastReading     = LOW;
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 150;

  bool currentReading = digitalRead(changeDisplayInformation);

  if (currentReading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (currentReading != lastStableState) {
      lastStableState = currentReading;
      if (lastStableState == HIGH) {          // rising edge
        return true;
      }
    }
  }
  lastReading = currentReading;
  return false;
}

void tftPrintCurrentPage() {
  tftPrintDisplayHeader();          // common header for every mode
  switch (displayMode) {
    case 0:  
      tftPrintMainSensorData(); 
      break;
    case 1:  
      tftPrintDHT11Data();      
      break;
    case 2:  
      tftPrintBH1750Data();     
      break;
    case 3:  
      tftPrintBMP280Data();     
      break;
  }
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
    return data; 
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
    return data;

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

  // Green = MQTT connected, Red = disconnected
  uint16_t dotColor = mqttClient.isConnected() ? ST77XX_GREEN : ST77XX_RED;
  tft.fillCircle(60, 8, 5, dotColor);
}

void tftBlankDisplay(){
  tft.fillScreen(ST77XX_BLACK);
}

void serialPrintSensorData(){
  Serial.print("Temperature: ");
  Serial.print(lastMainData.temperature);
  Serial.println(" *C");

  Serial.print("Humidity: ");
  Serial.print(lastMainData.humidity);
  Serial.println("%");

  Serial.print("Light: ");
  Serial.print(lastMainData.light);
  Serial.println("lx");

  Serial.print("Pressure: ");
  Serial.print(lastMainData.pressure);
  Serial.println(" Pa");

  Serial.print("Altitude: ");
  Serial.print(lastMainData.altitude);
  Serial.println(" m");
}

void tftPrintMainSensorData() {
  tft.setTextWrap(false);
  tft.setCursor(0, 35);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  tft.print("Temp.: ");
  tft.print(lastMainData.temperature);  
  tft.println(" *C");
  
  tft.print("Humid.: ");
  tft.print(lastMainData.humidity);
  tft.println(" %");

  tft.print("Light: ");
  tft.print(lastMainData.light);
  tft.println(" lx");

  tft.print("Press.: ");
  tft.print(lastMainData.pressure/100.0F);
  tft.println(" hPa");

  tft.print("Alt.: ");
  tft.print(lastMainData.altitude);
  tft.println(" m");
}

void tftPrintDHT11Data(){
  tft.setTextWrap(false);
  tft.setCursor(0, 35);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  tft.println("DHT11 Sensor");

  tft.print("Temp.: ");
  tft.print(lastDHT11Data.temperature);  
  tft.println(" *C");
  
  tft.print("Humid.: ");
  tft.print(lastDHT11Data.humidity);
  tft.println(" %");
}

void tftPrintBH1750Data(){
  tft.setTextWrap(false);
  tft.setCursor(0, 35);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  tft.println("BH1750 Sensor");

  tft.print("Light.: ");
  tft.print(lastBH1750Data.light);  
  tft.println(" lx");
}

void tftPrintBMP280Data(){
  tft.setTextWrap(false);
  tft.setCursor(0, 35);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  tft.println("BMP280 Sensor");

  tft.print("Temp.: ");
  tft.print(lastBMP280Data.temperature);  
  tft.println(" *C");

  tft.print("Press.: ");
  tft.print(lastBMP280Data.pressure/100.0F);
  tft.println(" hPa");

  tft.print("Alt.: ");
  tft.print(lastBMP280Data.altitude);
  tft.println(" m");
}