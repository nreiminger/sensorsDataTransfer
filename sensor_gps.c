#include <TinyGPS++.h>
#include "Wire.h"
#include <SPI.h>
#include <SdFat.h>
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <StackList.h>

int Address = 0x69; // device address of SPS30 (fixed).
byte w1, w2, w3;
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

bool connected = false;
String name = "RSens.V1.";
bool history = false;

void setup() {
  //enabling VDD_SW power rail to give 5V source to the dolfin board
  pinMode(SW_VDD_EN, OUTPUT);
  digitalWrite(SW_VDD_EN, HIGH);
  //enabling 5V_sensor power rail to switch on the sensor
  pinMode(EXP_36, OUTPUT);
  digitalWrite(EXP_36, HIGH);
  //enabling 3V3_SW power rail to pull the I2C bus up via the 10K resistors
  pinMode(SW_3V3_EN, OUTPUT);
  digitalWrite(SW_3V3_EN, HIGH);
  //wait for the power sources to be turned on
  delay(1000);

  Wire1.begin(); // Initiate the Wire1 instance for the I2C communication
  Serial1.begin(9600); //Initialize the Serial1 from the UART communication
  Serial.begin(115200);
  pinMode(SDCARD_EN_PIN, OUTPUT);
  digitalWrite(SDCARD_EN_PIN, HIGH);
  
  delay(100);
  //enabling XLB_VDD power rail to give power to the module
  pinMode(XLB_EN, OUTPUT);
  digitalWrite(XLB_EN, HIGH);
  delay(1000);

   Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");


  myFile = SD.open("config");
  if (myFile) {
    name+= getValue(myFile.readStringUntil('\n'),'=',1);
    Serial.print(name.c_str());
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening config.txt");
  }
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
  Bluefruit.setName(name.c_str());
  //Bluefruit.setName(getMcuUniqueID()); // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback); //calbback utilisé lorsqu'un appareil se connecte a la carte
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback); //callbacl appeler lorsqu'un appareil se deconnecte de la carte

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();


  // Set up and start advertising
  startAdv();  
  
  //ecrire "end" pour indiquer que c'est le debut de la mesure
  myFile = SD.open("test.txt", FILE_WRITE);
  if(myFile){
    myFile.println("end");
    myFile.close();
  }
}

void loop() {
  if(history){
    Serial.println("histor");
    readLastData();
    history=false;  
  }
  displayInfo();  
}

void displayInfo() {  
  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    String d = "d=";
    String h = "h=";
    String pm = "pm=";
    String lt = "lt=";
    String lg = "lg=";
    String milli = "m=";
    digitalWrite(LED_RED, HIGH);
    delay(200);
    digitalWrite(LED_RED, LOW);
    if (gps.date.isValid()) {
      d += gps.date.day();
      d += "/";
      d += gps.date.month();
      d += "/";
      d += gps.date.year();
    }
    else
    {
      d += "0/0/2000";
    }
    delay(5000);
    unsigned long mi = millis();
    milli += mi;
    

    if (gps.time.isValid())
    {
      /////get the hours data
      h += gps.time.hour();
      h += ":";
      h += gps.time.minute();
      h += ":";
      h += gps.time.second();
    }
    else
    {
      h += "0:0:0";
    }   
    String s = sensor();
    if (s == "") {
      pm += "A0A0A0";
    } else {
      pm += s;
    }
   
    if ( gps.location.isValid() && gps.location.age() < 5000 )
    {
      //////get the location data
      Serial.print(gps.location.lat(), 6);
      lt += gps.location.lat();
      Serial.print(F(","));
      Serial.print(gps.location.lng(), 6);
      lg += gps.location.lng();
      digitalWrite(LED_BLUE, HIGH);
      delay(200);
      digitalWrite(LED_BLUE, LOW);
    }
    else
    {
      lt += "00.000000";
      lg += "00.000000";
    }
    Serial.println("--------");
    /*Serial.print(d);
    Serial.print(F(" "));
    Serial.print(h);
    Serial.print(" ");
    Serial.print(pm);
    Serial.print(" ");
    Serial.print(lt);
    Serial.print(" ");
    Serial.print(lg);
    Serial.print(milli);*/
    
    if (pm != "pm=A0A0A0") {
      myFile.print(String(d));
      myFile.print(";");
      myFile.print(String(h));
      myFile.print(";");
      myFile.print(String(pm));
      myFile.print(";");
      myFile.print(String(lt));
      myFile.print(";");
      myFile.print(String(lg));
      myFile.print(";");
      myFile.println(String(milli));
      milli = "m=";
      if (connected) {
        bleuart.print(d);
        bleuart.print(h);
        bleuart.print(pm);
        bleuart.print(lt);
        bleuart.print(lg);
        bleuart.print(milli);
      }
    }
    //Fermeture du fichier pour enregirstrement
    myFile.close();
  }

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

void readLastData() {
  // Ouverture de fichier en mode lecture
  Serial.println("last");
  myFile = SD.open("test.txt", FILE_READ);
  //instanciation de la pile qui va acceullir toutes les mesures
  StackList <String> stack;
  if (myFile) {
    while (myFile.available()) {
      //lecture de la ligne courante, la fin de la ligne est marqué par '\n'
      String data = myFile.readStringUntil('\n');
      //empiler la ligne sur la pile.
      stack.push(data);
    }
  }
  myFile.close();
  //ecriture d'un flag pour spécifier a quelle endroit est ce qu'on a fini de lire le fichier
  myFile = SD.open("test.txt", FILE_WRITE);
  if(myFile){
    Serial.println("end");
    myFile.println("end");
    myFile.close();
  }
  //tant que le fichier la liste n'est pas vide, depiler la valeur se trouvant au sommet.
  //Comme la dernière valeur empiler est la dernière ligne du fichier, la permière valeur a être dépiler est la valeur la plus recente.
  int diffMillis = 0;
  String prev = stack.pop();
  Serial.println(prev+"*******");
  for (int i = 0; i < 5; i++) {
    bleuart.print(String(getValue(prev, ';', i)));
  }
  bleuart.print("m=0");
  bleuart.print("**********");
  String mi1 = getValue(prev, ';', 5);
  int m1 = getValue(mi1, '=', 1).toInt();
  bool stop = false;
  while (!stack.isEmpty() && !stop) {
    String m = "m=";
    String suiv = stack.pop();
    String mi2 = getValue(suiv, ';', 5);
    int m2 = getValue(mi2, '=', 1).toInt();
    if (m2 > m1 || suiv == "end") {
      stop = true;
    } else {
      diffMillis += m1 - m2;
      for (int j = 0; j < 5; j++) {
        Serial.print(String(getValue(suiv, ';', j))+";");
        bleuart.print(String(getValue(suiv, ';', j)));
      }
      m += diffMillis;
      bleuart.print(m);
      bleuart.println("**********");
      Serial.println("*********");
      m1 = m2;
    }
  }
}


/**
   fonction qui va simuler un split commencant a 1.
*/
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
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
  for (int i = 0; i < 2; i++) {
    crc ^= data[i];    for (byte bit = 8; bit > 0; --bit) {
      if (crc & 0x80) {
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
     - Enable auto advertising if disconnected
     - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
     - Timeout for fast mode is 30 seconds
     - Start(timeout) with timeout = 0 will advertise forever (until connected)

     For recommended advertising interval
     https://developer.apple.com/library/content/qa/qa1931/_index.html
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
  connected = true;
  history = true;
  Serial.println(history);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
  myFile = SD.open("test.txt", FILE_WRITE);
  if(myFile){
    Serial.println("end");
    myFile.println("end");
    myFile.close();
  }
}
