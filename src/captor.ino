//press Ctrl+/ to uncomment

#include <TinyGPS++.h>
#include "Wire.h"
#include <SPI.h>
#include <SdFat.h>
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <StackList.h>
#include "defs.h"

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

String name = "RSens.V1."; //permet de donné le debut du nom de l'appareil, il va être complété par un id
bool history = false; //permet de determiner si on a le droit de recuperer d'historique (stocké sur la carte SD), on ne peux recuperer l'historique que lorsqu'un se connecte. 

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
  Serial.begin(115200); //Initialize the serial to communicate with user
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

  //ouverture du fichier de configuration pour definir le nom du device, c'est ID qui est important
  myFile = SD.open("config");
  if (myFile) {
    name += getValue(myFile.readStringUntil('\n'), '=', 1);
    Serial.println(name.c_str());
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
  if (myFile) {
    myFile.println("end");
    myFile.close();
  }
}

void loop() {
  if (history) {
    Serial.println("*En attente de connexion : "+String(bleuart.notifyEnabled()));
    while(!bleuart.notifyEnabled()){
      ;
    }
    Serial.println("*Connecter : "+String(bleuart.notifyEnabled()));
    readLastData();
    history = false;
  }
  delay(5000);
  sendData();
}

void sendData() {
  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    //struct ou va etre placé toute 
    Value data;
    sensorGPS(&data);
    if(data.pms != "pm=A0A0A0"){
      myFile.println(data.date+";"+data.time+";"+data.pms+";"+data.lattitude+";"+data.longitude+";"+data.millis);  
    }
    myFile.close(); //enregistrement sur la carte SD. Sans fermer le fichier, celui-ci ne save pas 
    String milli = "m=";  
    Serial.println("Connected : "+String(bleuart.notifyEnabled()));
    if (bleuart.notifyEnabled()) { //si connecté envoie des données
      bleuart.print(data.date);
      bleuart.print(data.time);
      bleuart.print(data.pms);
      bleuart.print(data.lattitude);
      bleuart.print(data.longitude);
      bleuart.print(milli);
    }
  }
}

//////////////////////////////////////////////////////////
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

////////fonction utile pour le BLE
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
  history = true;
}

/**
   Callback invoked when a connection is dropped
   @param conn_handle connection where this event happens
   @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
*/
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    myFile.println("end");
    myFile.close();
  }
}
