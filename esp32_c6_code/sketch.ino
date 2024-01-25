#include <Wire.h>
#include <WiFi.h>

#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <Adafruit_NeoPixel.h>

#define PIN_WS2812B 8  // The ESP32 pin GPIO16 connected to WS2812B
#define NUM_PIXELS 1   // The number of LEDs (pixels) on WS2812B LED strip

Adafruit_NeoPixel ws2812b(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);

const int16_t SCD40_ADDRESS = 0x62;
const int16_t SGP41_ADDRESS = 0x59;

// Replace with your network credentials (STATION)
const char* WIFI_SSID = "";
const char* WIFI_PASSWORD = "";
const char* CONTROLLER_NAME = "";
const char* API_KEY = "";

const char* SERVER_URL = "";


uint8_t CalcCrc(uint8_t data[2]) {
  uint8_t crc = 0xFF;
  for(int i = 0; i < 2; i++) {
    crc ^= data[i];
    for(uint8_t bit = 8; bit > 0; --bit) {
      if(crc & 0x80) {
        crc = (crc << 1) ^ 0x31u;
      } else {
        crc = (crc << 1);
        }
      }
    }
  return crc;
}


void read_i2c(uint8_t *data, uint8_t length, int16_t address) {
  uint8_t counter;
  counter = 0;

  Wire.requestFrom(address, length);
  while (Wire.available()) {
    data[counter++] = Wire.read();
  }
}


void setup_scd40() {
  // setup scd40 according to datasheet
  // you have to wait 5 seconds for the first measurement

  Wire.beginTransmission(SCD40_ADDRESS);
  Wire.write(0x21);
  Wire.write(0xb1);
  Wire.endTransmission();
}


void setup_sgp41() {
  // setup sgp41 according to datasheet
  // preforms conditioning for 10 seconds

  int sgp41_conditioning = 10;

  while(sgp41_conditioning > 0) {
    Wire.beginTransmission(SGP41_ADDRESS);
    Wire.write(0x26);
    Wire.write(0x12);
    Wire.write(0x80);
    Wire.write(0x00);
    Wire.write(0xA2);
    Wire.write(0x66);
    Wire.write(0x66);
    Wire.write(0x93);
    Wire.endTransmission();

    sgp41_conditioning--;
    delay(1000);
  }

  Wire.beginTransmission(SGP41_ADDRESS);
  Wire.write(0x26);
  Wire.write(0x19);
  Wire.endTransmission();
}


void get_scd40_data(uint16_t *co2, uint16_t *temperature, uint16_t *humidity) {
  // get readings from scd40 sensor without conversion

  uint8_t data[12];

  Wire.beginTransmission(SCD40_ADDRESS);
  Wire.write(0xec);
  Wire.write(0x05);
  Wire.endTransmission();

  read_i2c(data, 12, SCD40_ADDRESS);

  *co2 = ((uint16_t)data[0] << 8 | data[1]);
  *temperature = ((uint16_t)data[3] << 8 | data[4]);
  *humidity = ((uint16_t)data[6] << 8 | data[7]);
}


void get_sgp41_data(uint16_t temperature, uint16_t humidity, uint16_t *raw_voc, uint16_t *raw_nox) {
  // send temperature and humidity readings to sgp41 for a more accurate reading
  // read VOC and NOx data

  uint8_t data[6], temp_data[2], hum_data[2];

  hum_data[0]=(humidity >> 8);
  hum_data[1]=humidity & 0xff;

  temp_data[0]=(temperature >> 8);
  temp_data[1]=temperature & 0xff;

  Wire.beginTransmission(SGP41_ADDRESS);
  Wire.write(0x26);
  Wire.write(0x19);

  // write relative humidity
  Wire.write(hum_data[0]);
  Wire.write(hum_data[1]);
  Wire.write(CalcCrc(hum_data));

  // write temperature
  Wire.write(temp_data[0]);
  Wire.write(temp_data[1]);
  Wire.write(CalcCrc(temp_data));
  Wire.endTransmission();

  delay(50);

  read_i2c(data, 6, SGP41_ADDRESS);

  *raw_voc = ((uint16_t)data[0] << 8 | data[1]);
  *raw_nox = ((uint16_t)data[3] << 8 | data[4]);
}


void change_led (uint16_t co2) {
  // change color of the LED according to co2 concentration
  // does not call for the LED update

  if (co2 >= 1600) { // set red if concentration > 1600ppm
    ws2812b.setPixelColor(0, ws2812b.Color(10, 0, 0));
    return;
  }

  if (co2 >= 1000) { // set red if concentration between 1600 and 1000ppm
    ws2812b.setPixelColor(0, ws2812b.Color(5, 5, 0));
    return;
  }

  if (co2 >= 300) { // set green if concentration between 1000 and 300ppm
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

  // while(!Serial);
  // Serial.print("setup started\n");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  setup_scd40();
  setup_sgp41();

  // Serial.print("setup complete\n");
  ws2812b.clear();
  ws2812b.show();
}


void loop() {
  HTTPClient http;
  uint16_t co2, temperature, humidity, raw_voc, raw_nox;
  
  get_scd40_data(&co2, &temperature, &humidity);
  change_led(co2);
  ws2812b.show(); 

  StaticJsonDocument<384> doc;

  doc["controller_name"] = CONTROLLER_NAME;
  doc["api_key"] = API_KEY;

  // floating point conversion according to datasheet
  doc["carbon_dioxide"] = co2;
  doc["temperature"] = -45 + 175 * (float)(temperature) / 65536;
  doc["humidity"] = 100 * (float)(humidity) / 65536;

  JsonArray raw_voc_data = doc.createNestedArray("raw_voc_data");
  JsonArray raw_nox_data = doc.createNestedArray("raw_nox_data");

  for(int i=1; i<=5; i++) {
    get_sgp41_data(temperature, humidity, &raw_voc, &raw_nox); 
    raw_voc_data.add(raw_voc);
    raw_nox_data.add(raw_nox);

    delay(950);
  }

  http.begin(SERVER_URL + String("api/create_sensor_reading"));

  String requestBody;
  serializeJson(doc, requestBody);
  http.POST(requestBody);

  http.end();
}
