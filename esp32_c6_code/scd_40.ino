void setup_scd40() {
  // setup scd40 according to datasheet
  // you have to wait 5 seconds for the first measurement

  Wire.beginTransmission(SCD40_ADDRESS);
  Wire.write(0x21);
  Wire.write(0xb1);
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