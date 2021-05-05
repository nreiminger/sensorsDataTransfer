String sensorSPS30() {
  /*
    // RESET device
    delay(1000);
    SetPointer(0xD3, 0x04);
    delay(1000);
  */

  //Start Measurement
  Wire1.beginTransmission(Address);

  //////////////////start mesurment
  Wire1.write(0x00);
  Wire1.write(0x10);
  /////////////////Read Measured values
  Wire1.write(0x03);
  Wire1.write(0x00);
  uint8_t data[2] = {0x03, 0x00};
  Wire1.write(CalcCrc(data));
  Wire1.endTransmission();

  /*
    //Start Fan Cleaning
    Serial.println("clean");
    Start fan cleaning
    SetPointer(0x56, 0x07);
    delay(12000);
    Serial.println("clean end");
    delay(100);
  */
  String res = "";
  //Read data ready flag
  SetPointer(0x02, 0x02);
  Wire1.requestFrom(Address, 3);
  w1 = Wire1.read();
  w2 = Wire1.read();
  w3 = Wire1.read();


  if (w2 == 0x01) {           //0x01: new measurements ready to read
    SetPointer(0x03, 0x00);
    Wire1.requestFrom(Address, 24);
    for (int i = 0; i < 60; i++) {
      ND[i] = Wire1.read();
      //    for(int i=0;i<30;i++) { ND[i]=Wire.read();     //without Wire.h lib modification only first 5 values
      //Serial.print(i);Serial.print(": ");Serial.println(ND[i],HEX);
    }
    // Result: PM1.0/PM2.5/PM4.0,PM10[Î¼g/mÂ³] , PM0.5,PM1.0/PM2.5/PM4.0,PM10 [[#/cmÂ³], Typical Particle Size [Î¼m]
    int pm = 1;

    for (int i = 0; i < 24; i++) {
      if ((i + 1) % 3 == 0)
      {
        byte datax[2] = {ND[i - 2], ND[i - 1]};
        //Serial.print("crc: ");Serial.print(CalcCrc(datax),HEX);
        //Serial.print("  "); Serial.println(ND[i],HEX);
        if (tmp == 0) {
          tmp = ND[i - 2];
          tmp = (tmp << 8) + ND[i - 1];
        }
        else {
          tmp = (tmp << 8) + ND[i - 2];
          tmp = (tmp << 8) + ND[i - 1];
          //Serial.print(tmp,HEX);Serial.print(" ");
          measure = (*(float*) &tmp);
          if (pm != 3) {
            res += 'A';
            res += round(measure);
          }
          tmp = 0;
          pm += 1;
        }
      }
    }
    return res;
  }
}

void SetPointer(byte P1, byte P2)
{
  Wire1.beginTransmission(Address);
  Wire1.write(P1);
  Wire1.write(P2);
  Wire1.endTransmission();
}
 
 
// from datasheet:
byte CalcCrc(byte data[2]) {
  byte crc = 0xFF;
  for(int i = 0; i < 2; i++) {    crc ^= data[i];    for(byte bit = 8; bit > 0; --bit) {
      if(crc & 0x80) {
      crc = (crc << 1) ^ 0x31u;
      } else {
        crc = (crc << 1);
       }
     }
   }
  return crc;
}
