//Based on the SHT4x example from https://github.com/adafruit/Adafruit_SHT4X

/*
  Reads temperature and humidity from an SHT41 (I2C, address 0x44) every second.
  Wiring: VIN -> 3.3V, GND -> GND, SDA -> GPIO 21, SCL -> GPIO 22
*/

#include <Wire.h>
#include <Adafruit_SHT4x.h>

Adafruit_SHT4x sht4;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!sht4.begin(&Wire)) {
    Serial.println(F("Could not find SHT41 sensor, check wiring"));
    while (1) delay(10);
  }
  Serial.print(F("SHT41 found, serial number 0x"));
  Serial.println(sht4.readSerial(), HEX);

  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  sht4.setHeater(SHT4X_NO_HEATER);
}

void loop() {
  sensors_event_t humidity, temp;
  if (sht4.getEvent(&humidity, &temp)) {
    Serial.print("Temperature: ");
    Serial.print(temp.temperature);
    Serial.println(" *C");
    Serial.print("Humidity: ");
    Serial.print(humidity.relative_humidity);
    Serial.println(" %");
  } else {
    Serial.println("Error reading SHT41");
  }
  delay(1000);
}
