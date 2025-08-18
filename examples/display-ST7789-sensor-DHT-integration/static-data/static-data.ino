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

  //tft.setSPISpeed(40000000);
  Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  dht.begin();  
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);           // imprime os detalhes do Sensor de Temperatura
  dht.humidity().getSensor(&sensor);            // imprime os detalhes do Sensor de Umidade

  Serial.println("done");
  delay(1000);
}

void loop() {
    sensors_event_t event;                        // inicializa o evento da Temperatura
    dht.temperature().getEvent(&event);           // faz a leitura da Temperatura
    dht.humidity().getEvent(&event);              // faz a leitura de umidade

    if (isnan(event.temperature) || isnan(event.relative_humidity))                 // se algum erro na leitura
    {
        Serial.println("Error reading sensor data");
    }
    else                                          // senão
    {
        Serial.print("Temperatura: ");              // imprime a Temperatura
        Serial.print(event.temperature);
        Serial.println(" *C");
        tftPrintTest();

    }
    delay(10000);
}


void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}


void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(100, 0);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.println("TESTS");
  tft.setCursor(0, 35);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Teste 1");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Teste 2");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Teste 3");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.println("Teste 4");
}
