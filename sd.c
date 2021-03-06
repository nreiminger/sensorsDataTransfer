/**
 * Librairie utilisée : 
 * https://playground.arduino.cc/Code/StackList/
 */

#include <StackList.h>
#include <SPI.h>
#include <SdFat.h>

SdFat SD(&SPI1);

#define SD_CS_PIN SS1
File myFile;
/**
 * lecture d'un fichier qui enregistre les données des capteurs, les données les plus ancienne sont stocké au début du fichier. 
 * Pour avoir la valeur la valeur de la fichier la plus ;l;llpopopop
    ; // wait for serial port to connect. Needed for native USB port only
  }
 */
void setup(){
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  pinMode(SDCARD_EN_PIN,OUTPUT);
  digitalWrite(SDCARD_EN_PIN, HIGH); 

  Serial.print("Initializing SD card...");
  //vérifier si la carte SD est bien insérer dans la carte Arduino
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  // Ouverture de fichier en mode lecture
  myFile = SD.open("test.txt",FILE_READ);
  //instanciation de la pile qui va acceullir toutes les mesures
  StackList <String> stack;
  if (myFile) {
    while(myFile.available()){
     //lecture de la ligne courante, la fin de la ligne est marqué par '\n'
     String data = myFile.readStringUntil('\n');
     //empiler la ligne sur la pile.
     stack.push(data);
   }
  }
  
  //tant que le fichier la liste n'est pas vide, depiler la valeur se trouvant au sommet.
  //Comme la dernière valeur empiler est la dernière ligne du fichier, la permière valeur a être dépiler est la valeur la plus recente. 
  int diffMillis = 0;
  String prev = stack.pop();
  Serial.println(diffMillis);
  Serial.println(prev);
  String mi1= getValue(prev,';',5);
  int m1 = getValue(mi1,'=',1).toInt();
  bool stop = false; 
  while(!stack.isEmpty() && !stop){
    String suiv = stack.pop();
    String mi2=getValue(suiv,';',5);
    int m2 = getValue(mi2,'=',1).toInt(); 
    if(m2 > m1){
      stop = true;
    }else{
      diffMillis += m1-m2;
      Serial.println(diffMillis);
      Serial.println(suiv); 
      m1 = m2;
    }
  }
}



/**
 * fonction qui va simuler un split
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
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
void loop() {
  // nothing happens after setup
}
