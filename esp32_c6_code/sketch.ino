#include <Wire.h>
#include <WiFi.h>

#include <VOCGasIndexAlgorithm.h>
#include <NOxGasIndexAlgorithm.h>

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>


#define PIN_WS2812B 8  // The ESP32 pin GPIO16 connected to WS2812B
#define NUM_PIXELS 1   // The number of LEDs (pixels) on WS2812B LED strip

#define TXD_PIN (GPIO_NUM_5)
#define RXD_PIN (GPIO_NUM_4)


Adafruit_NeoPixel ws2812b(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);

const int16_t SCD40_ADDRESS = 0x62;
const int16_t SGP41_ADDRESS = 0x59;

uint8_t scd40_buffer_index = 0;
uint16_t co2_buffer[8];
uint16_t temperature_buffer[8];
uint16_t humidity_buffer[8];

VOCGasIndexAlgorithm voc_algorithm;
NOxGasIndexAlgorithm nox_algorithm;
uint16_t voc_index;
uint16_t nox_index;

uint8_t pmsa003_buffer_index = 0;
uint16_t pm1_0_buffer[16];
uint16_t pm2_5_buffer[16];
uint16_t pm10_buffer[16];

const uint16_t scd40_read_time = 5000;
const uint16_t sgp41_read_time = 1000;
const uint16_t data_send_time = 60000;

unsigned long scd40_previous_time = 0;
unsigned long sgp41_previous_time = 0;
unsigned long data_previous_time = 0;


// Replace with your network credentials (STATION)
const char* WIFI_SSID = "";
const char* WIFI_PASSWORD = "";
const char* CONTROLLER_NAME = "";
const char* API_KEY = "";

const char* SERVER_URL = "";


float get_array_avg(uint16_t buffer[], int length) {
  float average;

  for (uint8_t i = 0; i < length; i++) {
    average += buffer[i];
  }
  return average / length;
}


uint8_t CalcCrc(uint8_t data[2]) {
  uint8_t crc = 0xFF;
  for (int i = 0; i < 2; i++) {
    crc ^= data[i];
    for (uint8_t bit = 8; bit > 0; --bit) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x31u;
      } else {
        crc = (crc << 1);
      }
    }
  }
  return crc;
}


void read_i2c(uint8_t* data, uint8_t length, int16_t address) {
  uint8_t counter;
  counter = 0;

  Wire.requestFrom(address, length);
  while (Wire.available()) {
    data[counter++] = Wire.read();
  }
}


void change_led(uint16_t co2) {
  // change color of the LED according to co2 concentration
  // does not call for the LED update

  if (co2 >= 1600) {  // set red if concentration > 1600ppm
    ws2812b.setPixelColor(0, ws2812b.Color(10, 0, 0));
    return;
  }

  if (co2 >= 1000) {  // set red if concentration between 1600 and 1000ppm
    ws2812b.setPixelColor(0, ws2812b.Color(5, 5, 0));
    return;
  }

  if (co2 >= 300) {  // set green if concentration between 1000 and 300ppm
    ws2812b.setPixelColor(0, ws2812b.Color(0, 10, 0));
    return;
  }

  // under normal conditions co2 won't be less then ~400ppm, if so something is wrong. Set LED to purple
  ws2812b.setPixelColor(0, ws2812b.Color(5, 0, 5));
}


void setup() {
  // Serial.begin(115200);
  ws2812b.begin();
  Wire.begin();

  ws2812b.setPixelColor(0, ws2812b.Color(0, 100, 100));
  ws2812b.show();

  // while (!Serial);
  // Serial.print("setup started\n");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  setup_scd40();
  setup_sgp41();
  setup_pmsa003();

  // Serial.print("setup complete\n");
  ws2812b.clear();
  ws2812b.show();
}


void loop() {
  unsigned long currentTime = millis();

  if (Serial1.available() >= 32) {
    uint16_t PM1_0, PM2_5, PM10;

    if (!read_pmsa003(&PM1_0, &PM2_5, &PM10)) {
      // Serial.println("UART read error");
      setup_pmsa003();
      return;
    }

    pmsa003_buffer_index++;
    pm1_0_buffer[pmsa003_buffer_index & 0xf] = PM1_0;
    pm2_5_buffer[pmsa003_buffer_index & 0xf] = PM2_5;
    pm10_buffer[pmsa003_buffer_index & 0xf] = PM10;
  }

  if (currentTime - scd40_previous_time >= scd40_read_time) {
    scd40_previous_time = currentTime;
    uint16_t co2, temperature, humidity;
    get_scd40_data(&co2, &temperature, &humidity);

    scd40_buffer_index++;
    co2_buffer[scd40_buffer_index & 0x7] = co2;
    temperature_buffer[scd40_buffer_index & 0x7] = temperature;
    humidity_buffer[scd40_buffer_index & 0x7] = humidity;

    change_led(co2);
    ws2812b.show();
  }

  if (currentTime - sgp41_previous_time >= sgp41_read_time) {
    sgp41_previous_time = currentTime;

    uint16_t raw_voc, raw_nox;
    get_sgp41_data(temperature_buffer[scd40_buffer_index & 0x7], humidity_buffer[scd40_buffer_index & 0x7], &raw_voc, &raw_nox);

    voc_index = voc_algorithm.process(raw_voc);
    nox_index = nox_algorithm.process(raw_nox);
  }

  if (currentTime - data_previous_time >= data_send_time) {
    data_previous_time = currentTime;

    HTTPClient http;
    JsonDocument doc;

    doc["controller_name"] = CONTROLLER_NAME;
    doc["api_key"] = API_KEY;

    doc["carbon_dioxide"] = get_array_avg(co2_buffer, 8);
    doc["temperature"] = -45 + 175 * get_array_avg(temperature_buffer, 8) / 65536;
    doc["humidity"] = 100 * get_array_avg(humidity_buffer, 8) / 65536;

    doc["voc_index"] = voc_index;
    doc["nox_index"] = nox_index;

    doc["pm1.0"] = get_array_avg(pm1_0_buffer, 16);
    doc["pm2.5"] = get_array_avg(pm2_5_buffer, 16);
    doc["pm10"] = get_array_avg(pm10_buffer, 16);

    http.begin(SERVER_URL + String("api/create_sensor_reading"));

    String requestBody;
    serializeJson(doc, requestBody);
    http.POST(requestBody);

    http.end();
  }
}
