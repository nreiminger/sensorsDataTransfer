#include <TinyGPS++.h>
#include "Wire.h"

 int Address = 0x69; // device address of SPS30 (fixed).

 byte w1, w2,w3;
 byte ND[60];
 long tmp;
 float measure;
// The TinyGPS++ object
TinyGPSPlus gps;
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

// BLE Service
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // battery

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

  //BLE
    Serial.begin(115200);

#if CFG_DEBUG
  // Blocking wait for connection when debug mode is enabled via IDE
  while ( !Serial ) yield();
#endif
    // Setup the BLE LED to be enabled on CONNECT
  // Note: This is actually the default behavior, but provided
  // here in case you want to control this LED manually via PIN 19
  Bluefruit.autoConnLed(true);

  // Config the peripheral connection with maximum bandwidth 
  // more SRAM required by SoftDevice
  // Note: All config***() function must be called before begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("RSens.V1.007");
  //Bluefruit.setName(getMcuUniqueID()); // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();


  // Set up and start advertising
  startAdv();
  
}

void loop()
{
  /*// This sketch displays information every time a new sentence is correctly encoded.
  while (Serial1.available() > 0){
    if (gps.encode(Serial1.read())){
      displayInfo(); //get the gps data 
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }*/

   

}


  
/////////////////////////////////

void displayInfo()
{
  Serial.println("--------------");
  Serial.print("  Date/Time: ");
  String dh ="";
  String heure="";
  String pm="";
  String latit="";
  String longit="";
  if (gps.date.isValid())
  {
    dh = gps.date.year();
    dh += "/";
    dh += gps.date.month();
    dh += "/";
    dh += gps.date.day();
    //////get the date data 
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print("0/0/2000");
    dh = "0/0/2020";
  }
  
  Serial.print(F(" "));
  
  if (gps.time.isValid())
  {
    /////get the hours data 
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    heure = gps.time.hour();
    heure += ":";
    heure += gps.time.minute();
    heure +=":";
    heure += gps.time.second();
  }
  else
  {
    Serial.print(F("00:00:00"));
  }
  Serial.print(" ");
  
  delay(5000);
  pm = sensor();
  Serial.print(sensor());
  Serial.print(" ");
  
  Serial.print(F("Location: ")); 
  
  if ( gps.location.isValid() && gps.location.age() < 5000 )
  {
    //////get the location data 
    Serial.print(gps.location.lat(), 6);
    latit = gps.location.lat();
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    longit = gps.location.lng();
    digitalWrite(LED_BLUE,HIGH);
    delay(200);
    digitalWrite(LED_BLUE,LOW);
  }
  else
  {
    latit="000.000000";
    longit = "000.000000";
    Serial.print("000.000000");
    Serial.print(" ");
    Serial.print("000.000000");
  }
  Serial.println("");
  bleuart.print(dh);
  bleuart.print(heure);
  bleuart.print(pm);
  bleuart.print(latit);
  bleuart.print(longit);
}


//////////////////////////////////////////////////////////


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
void startAdv(void)
{
// Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
  while (Serial1.available() > 0){
    if (gps.encode(Serial1.read())){
      displayInfo(); //get the gps data 
    }
  }
  
}
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}