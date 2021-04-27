
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
  //while(1){
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

  // }

  //  Stop Meaurement
  //  SetPointer(0x01, 0x04);

}

void sensorGPS(Value* data){
  String date;
  String time;
  //date
  if (gps.date.isValid()) {
    date = gps.date.day();
    date += "/";
    date += gps.date.month();
    date += "/";
    date += gps.date.year();
  }
  else
  {
    date = "0/0/2000";
  }
  
  //time  
  if (gps.time.isValid())
  {
    time = gps.time.hour();
    time += ":";
    time += gps.time.minute();
    time += ":";
    time += gps.time.second();
  }
  else
  {
    time = "0:0:0";
  }
  
   //pm
  String pm = sensorSPS30();
  if (pm == "") {
    pm = "A0A0A0";
  } 

  //location
  double lt;
  double lg;
  if ( gps.location.isValid() && gps.location.age() < 5000 )
  {
    lt = gps.location.lat();
    lg = gps.location.lng();
  }
  else
  {
    lt = 00.000000;
    lg = 00.000000;
  }
  data->date += date;
  data->time += time;  
  data->pms += pm;
  data->lattitude += lt;
  data->longitude += lg;
}
