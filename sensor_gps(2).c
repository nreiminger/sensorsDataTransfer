#include <TinyGPS++.h>
#include "Wire.h"
#include <SPI.h>
#include <SdFat.h>
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
 
 int Address = 0x69; // device address of SPS30 (fixed).
 byte w1, w2,w3;
 byte ND[60];
 long tmp;
 float measure;
 
// The TinyGPS++ object
TinyGPSPlus gps;
SdFat SD(&SPI1);
#define SD_CS_PIN SS1
File myFile;

// BLE Service
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // battery

bool connected =false;
bool heure_date = true;  
void setup()
{
   //enabling VDD_SW power rail to give 5V source to the dolfin board
 pinMode(SW_VDD_EN, OUTPUT);
 digitalWrite(SW_VDD_EN,HIGH);
 //enabling 5V_sensor power rail to switch on the sensor
 pinMode(EXP_36, OUTPUT);
 digitalWrite(EXP_36,HIGH);
 //enabling 3V3_SW power rail to pull the I2C bus up via the 10K resistors
 pinMode(SW_3V3_EN, OUTPUT);
 digitalWrite(SW_3V3_EN,HIGH);
 //wait for the power sources to be turned on
 delay(1000);
 
  Wire1.begin(); // Initiate the Wire1 instance for the I2C communication 
  Serial1.begin(9600); //Initialize the Serial1 from the UART communication
  Serial.begin(115200);
  
  delay(100);
  //enabling XLB_VDD power rail to give power to the module
  pinMode(XLB_EN, OUTPUT); 
  digitalWrite(XLB_EN, HIGH);
  delay(1000);


#if CFG_DEBUG
  // Blocking wait for connection when debug mode is enabled via IDE
  while ( !Serial ) yield();
#endif
  /*while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }*/
  pinMode(SDCARD_EN_PIN,OUTPUT);
  digitalWrite(SDCARD_EN_PIN, HIGH); 

  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}

void loop() {
  // This sketch displays information every time a new sentence is correctly encoded.
  delay(2000);
  while (Serial1.available() > 0){
    myFile = SD.open("data.txt", FILE_WRITE);
    if(myFile){
      if (gps.encode(Serial1.read())){
        displayInfo();
      }
      myFile.close();
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  } 
}

void displayInfo()
{
  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    digitalWrite(LED_BLUE,HIGH);
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
    myFile.print(gps.date.month());
    myFile.print(F("/"));
    myFile.print(gps.date.day()); 
    myFile.print(F("/"));
    myFile.print(gps.date.year());
    myFile.print(";");
    heure_date=true;
  }
  else
  {
    digitalWrite(LED_BLUE,LOW);
    Serial.print(F("INVALID"));
    heure_date=false;
  }

  Serial.print(F(" "));
  
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) {
      Serial.print(F("0"));
      myFile.print(F("0"));
    }
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    myFile.print(gps.time.hour());
    myFile.print(F(":"));
    if (gps.time.minute() < 10) {
      Serial.print(F("0"));
      myFile.print(F("0"));
    }
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    myFile.print(gps.time.minute());
    myFile.print(F(":"));
    if (gps.time.second() < 10) {
      Serial.print(F("0"));
      myFile.print(F("0"));
    }
    Serial.print(gps.time.second());
    myFile.print(gps.time.second());
    myFile.print(";");

  }
  else
  {
    Serial.print(F("INVALID"));
    heure_date=false;
  }
  Serial.print(" ");
  Serial.print(sensor());
  if(heure_date)
    myFile.print(sensor());
  Serial.print(" ");
  Serial.print(F("Location: ")); 
  if ( gps.location.isValid() && gps.location.age() < 5000 )
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    myFile.print(gps.location.lat(), 6);
    myFile.print(F(","));
    myFile.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }
  Serial.println();
  if(heure_date)
    myFile.println();
}

String sensor() {
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
  uint8_t data[2]={0x03, 0x00};
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
  w1=Wire1.read();
  w2=Wire1.read();
  w3=Wire1.read();


  if (w2==0x01){              //0x01: new measurements ready to read
    SetPointer(0x03,0x00);
    Wire1.requestFrom(Address, 24);
    for(int i=0;i<60;i++) { ND[i]=Wire1.read();                        
//    for(int i=0;i<30;i++) { ND[i]=Wire.read();     //without Wire.h lib modification only first 5 values
    //Serial.print(i);Serial.print(": ");Serial.println(ND[i],HEX);
    }
    // Result: PM1.0/PM2.5/PM4.0,PM10[Î¼g/mÂ³] , PM0.5,PM1.0/PM2.5/PM4.0,PM10 [[#/cmÂ³], Typical Particle Size [Î¼m]
    int pm=1;
    
    for(int i=0;i<24;i++) { 
       if ((i+1)%3==0)
       {
         byte datax[2]={ND[i-2], ND[i-1]};
         //Serial.print("crc: ");Serial.print(CalcCrc(datax),HEX);
         //Serial.print("  "); Serial.println(ND[i],HEX);
         if(tmp==0) {
           tmp= ND[i-2]; 
           tmp= (tmp<<8) + ND[i-1]; 
          }
         else{
           tmp= (tmp<<8)+ ND[i-2];
           tmp= (tmp<<8) + ND[i-1];
           //Serial.print(tmp,HEX);Serial.print(" ");
           measure= (*(float*) &tmp);
           if(pm != 3){
            res += 'A';
            int val = round(measure);
            String valStr = String(val);
            int taille = valStr.length();
            for(int t=0;t<(3-taille);t++){
              res+="0";
            }
            res += round(measure);  
           }
           tmp=0;
           pm+=1;
         } 
       }
      }
      return res;
  }

// }

//  Stop Meaurement
//  SetPointer(0x01, 0x04);
 
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
