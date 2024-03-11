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