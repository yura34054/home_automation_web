void setup_pmsa003 () {
  Serial1.begin(9600, SERIAL_8N1, RXD_PIN, TXD_PIN);
  uint8_t message_active[7] = {0x42, 0x4d, 0xe1, 0x00, 0x01, 0x01, 0x71};
  Serial1.write(message_active, 7);
  delay(500);

  // flush rx buffer to remove unwanted data
  while (Serial1.available() > 0){
    Serial1.read();
  }
}


int read_pmsa003 (uint16_t *PM1_0, uint16_t *PM2_5, uint16_t *PM10) {
  uint8_t data[32];
  uint16_t checksum = 0;

  Serial1.readBytes(data, 32);

  for (int i=0; i<30; i++) {
    checksum += data[i];
  }

  if (checksum != ((uint16_t)data[30] << 8 | data[31])) {
    return 0;
  }

  *PM1_0 = ((uint16_t)data[10] << 8 | data[11]);
  *PM2_5 = ((uint16_t)data[12] << 8 | data[13]);
  *PM10 = ((uint16_t)data[14] << 8 | data[15]);
  return 1;
}
