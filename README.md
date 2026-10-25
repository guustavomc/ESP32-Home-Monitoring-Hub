# ESP32 Home Monitoring Hub

The ESP32 Home Monitoring Hub is a versatile IoT project that leverages the ESP32 microcontroller to monitor environmental parameters such as temperature, humidity, pressure, and light intensity. It integrates multiple sensors and a display to provide real-time data visualization, making it ideal for home automation, environmental monitoring, or educational purposes.

## Table of Contents
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Wiring](#wiring)
- [Usage](#usage)
- [Libraries Used](#libraries-used)
- [Code examples](#code-examples)
- [Next Steps](#next-steps)
- [Contributing](#contributing)
- [License](#license)

## Features
- Real-time monitoring of temperature, humidity, atmospheric pressure, and light intensity.
- Visual display of sensor data on an ST7789 TFT screen.
- Publishes sensor readings as JSON over MQTT (Wi-Fi), with NTP timestamps.
- Keeps running when a sensor fails: missing readings are sent as `null`.
- Easy-to-use interface for home monitoring applications.
- Modular design supporting multiple sensors.
- Low-power operation suitable for continuous monitoring.

## Hardware Requirements
- **ESP32 Development Board** (e.g., ESP32-WROOM-32)
- **DHT Sensor** (e.g., DHT11 or DHT22 for temperature and humidity)
- **BMP280 Sensor** (for atmospheric pressure and temperature)
- **BH1750 Sensor** (for light intensity)
- **ST7789 TFT Display** (240x280 SPI TFT display)
- Jumper wires
- 2 Push Buttons
- 2 × 10kΩ resistors (pull-downs for the buttons)
- Breadboard or custom PCB
- USB cable for programming and power
- **MQTT broker** on your network (e.g., Mosquitto on a Raspberry Pi) to receive the data

## Software Requirements
- **Arduino IDE** or **PlatformIO** for programming the ESP32
- ESP32 board support package installed in the Arduino IDE
- Required libraries (listed below)

## Installation
1. **Set up the Arduino IDE**:
   - Install the Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software).
   - Add ESP32 board support by following the [ESP32 Arduino setup guide](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).

2. **Install the required libraries**:
   - Open the Arduino IDE Library Manager and install the following libraries:
     - `Adafruit_Sensor`
     - `DHT sensor library`
     - `Adafruit_BMP280_Library`
     - `BH1750`
     - `Adafruit_ST7735_Library`
     - `EspMQTTClient`
   - Alternatively, download them from the links in the [Libraries Used](#libraries-used) section and add them manually.

3. **Clone the repository**:
   ```bash
   git clone https://github.com/guustavomc/ESP32-Home-Monitoring-Hub.git
   ```

4. **Create `src/main/config.h`** (it is gitignored, since it holds your credentials):
   ```cpp
   #pragma once

   // Wi-Fi credentials
   #define WIFI_SSID     "your-wifi-name"
   #define WIFI_PASSWORD "your-wifi-password"

   // MQTT broker: your Raspberry Pi's local IP
   #define MQTT_BROKER   "192.168.1.100"
   #define MQTT_TOPIC    "home/sensors"

   // NTP: UTC offset in seconds (e.g. -10800 for BRT/GMT-3)
   #define NTP_UTC_OFFSET_SEC -10800
   ```

5. **Upload the code**:
   - Open the main sketch `src/main/main.ino` in the Arduino IDE.
   - Connect your ESP32 to your computer via USB.
   - Select the appropriate board and port in the Arduino IDE.
   - Upload the code to the ESP32.

## Wiring
Below is a general wiring guide for connecting the sensors and display to the ESP32. Adjust pin assignments based on your specific code configuration.

<img src="component-integration-tests\4-Sensor-Station-Interface-Control\breadboard-circuit.jpg" alt="Breadboard Circuit">

| Component       | ESP32 Pin | Notes                              |
|-----------------|-----------|------------------------------------|
| DHT11/DHT22     | GPIO 2    | Data pin                           |
| BMP280 (SDA)    | GPIO 21   | I2C SDA                            |
| BMP280 (SCL)    | GPIO 22   | I2C SCL                            |
| BH1750 (SDA)    | GPIO 21   | I2C SDA (shared I2C bus)           |
| BH1750 (SCL)    | GPIO 22   | I2C SCL (shared I2C bus)           |
| ST7789 (TFT_CS) | GPIO 14   | Chip Select                        |
| ST7789 (DC)     | GPIO 32   | Data/Command                       |
| ST7789 (RST)    | GPIO 15   | Reset                              |
| ST7789 (SCL)    | GPIO 18   | SPI Clock                          |
| ST7789 (SDA)    | GPIO 23   | SPI MOSI                           |
| On/Off Button   | GPIO 34   | Activate or deactivate display; 10kΩ pull-down to GND |
| Change Info Button | GPIO 33 | Change sensor data on display; 10kΩ pull-down to GND |


**Notes**:
- Ensure a common ground (GND) for all components.
- Power the sensors and display with 3.3V or 5V as per their specifications.
- Verify pin assignments in the code before wiring.
- Wire each button between 3.3V and its GPIO, with a 10kΩ resistor from that GPIO to GND. Without it the pin floats when the button is released and presses are missed (GPIO 34 has no internal pull resistors, so the external one is required).

## Usage
1. Power the ESP32 via USB or an external power source.
2. The ST7789 display will show real-time data from the connected sensors:
   - **Temperature** and **Humidity** from the DHT sensor.
   - **Atmospheric Pressure** (and optional temperature) from the BMP280.
   - **Light Intensity** (in lux) from the BH1750.
3. Button connected to GPIO 34 will switch On and Off the display.
4. Button connected to GPIO 33 will alter from data summary to specific sensor data.
5. The dot next to the display header shows the MQTT connection: green = connected, red = disconnected.
6. Every 5 seconds the readings are published to `MQTT_TOPIC`, once NTP has synced:
   ```json
   {"timestamp":"2026-10-19T14:30:00-03:00","temperature":23.4,"humidity":55.0,"light":312.5,"pressure":1013.25,"altitude":12.3}
   ```
   Temperature is the average of the DHT11 and BMP280 (or whichever one is working), pressure is in hPa, and a failed sensor reading is sent as `null`.

## Libraries Used
The project relies on the following open-source libraries:
- [Adafruit_Sensor](https://github.com/adafruit/Adafruit_Sensor) - Base library for Adafruit sensors
- [DHT-sensor-library](https://github.com/adafruit/DHT-sensor-library) - For DHT11/DHT22 sensors
- [Adafruit_BMP280_Library](https://github.com/adafruit/Adafruit_BMP280_Library) - For BMP280 pressure sensor
- [BH1750](https://github.com/claws/BH1750) - For BH1750 light sensor
- [Adafruit_ST7735_Library](https://github.com/adafruit/Adafruit-ST7735-Library) - ST77xx driver, used for the ST7789 TFT display
- [EspMQTTClient](https://github.com/plapointe6/EspMQTTClient) - For MQTT Client

## Code Examples
The project used as a base multiple example projects from the community, examples used for each component:
- [Display ST7789](https://github.com/adafruit/Adafruit-ST7735-Library/blob/master/examples/graphicstest_st7789/graphicstest_st7789.ino) - Display example
- [Sensor BH1750](https://randomnerdtutorials.com/esp32-bh1750-ambient-light-sensor/) - For BH1750 sensors
- [Sensor BMP280](https://circuitdigest.com/microcontroller-projects/interfacing-bmp280-sensor-with-arduino) - For BMP280 sensor
- [Sensor DHT11](https://blog.eletrogate.com/sensores-dht11-dht22/) - For DHT11 sensor
- [Sensor PIR Motion](https://randomnerdtutorials.com/arduino-with-pir-motion-sensor/) - For PIR Motion Sensor
- [MQTT Client](https://github.com/plapointe6/EspMQTTClient/blob/master/examples/SimpleMQTTClient/SimpleMQTTClient.ino) - For MQTT

## Next Steps
The following features are planned to enhance the ESP32 Home Monitoring Hub:
- ~~**Sharing Sensor Data to a Separate Server**~~: Done. Readings are published over MQTT using [EspMQTTClient](https://github.com/plapointe6/EspMQTTClient).
- **Raspberry Pi server**: Mosquitto broker, InfluxDB storage and a Grafana dashboard for the published readings.
- **PIR motion sensor**: wake the display and log room occupancy.
- **Alerts**: notifications when readings cross thresholds (e.g., high humidity).
- **OTA updates**: flash new firmware over Wi-Fi.

## Contributing
Contributions are welcome! To contribute:
1. Fork the repository.
2. Create a new branch (`git checkout -b feature/your-feature`).
3. Make your changes and commit (`git commit -m "Add your feature"`).
4. Push to the branch (`git push origin feature/your-feature`).
5. Open a Pull Request.

Please ensure your code follows the project's coding style and includes appropriate documentation.
