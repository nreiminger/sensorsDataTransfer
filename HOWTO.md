# Configuration de l'environnement de dévelopement

## Installation de Arduino et de la carte flipper: 
Pour installer l'ide, veulliez suivre ce [lien](https://www.arduino.cc/en/software) pour l'installation de Arduino IDE.

Une fois avoir installer l'ide, il faut installer la carte Flipper, ainsi que toutes ses librairies dans l'ide : 
- il faut au préalable ajouter ce [lien](https://gitlab.com/strataggem1/software/device/arduino/arduino_dev_tools/strataggem_arduino_boards/-/raw/master/package_strataggem_index.json) dans la zone de texte dans le menu `File > Preferences > Additional Boards Manager URLs`

![preference](https://github.com/nreiminger/sensorsDataTransfer/blob/main/images/preference.png)
- Installer la carte dans l'ide, pour ce faire aller dans le menu `Tools > Board > Boards Manager`, puis rechercher `Strataggem`. Vous pouvez ensuite l'installer en cliquant sur le bouton `Install`.
![install](https://github.com/nreiminger/sensorsDataTransfer/blob/main/images/installFlipper.png)

A ce stade là, vous pouvez tester si la configuration de la carte est bien faite en uplodant sur la carte :
```c
    void setup() {
    }

    void loop() {
     digitalWrite(LED_RED, HIGH);
     delay(1000);
     digitalWrite(LED_RED, LOW);
     delay(1000);
    }
```
Si la compiliation ne passe pas, veullez verifier que vous avez bien selectionner la carte Flipper dans le menu `Tools > Board > Strataggem nrf52 board > Stataggem Flipper nrf82840 board` 
![selectCart](https://github.com/nreiminger/sensorsDataTransfer/blob/main/images/boardChoose.png)
## Installation des différantes bibliotheques neccesaire pour la confilation du sketch : 
Pour installer une bibliotheque nous avons deux méthodes soit:
- Installer la bibliotheque directement depuis l'IDE, en passant dans par `SKETCH > Include Library > Manage Libraries ...`
- Soit en incluant directement le fichier zipper de la bibliotheque qui peuvent être téléchagées directement depuis gitlab. 

Voici la liste des bibliotheque necessaire pour la conpilation du programme : 
1. SdFat Library (SdFat by Bill Greiman, tested version: 1.1.4)

    Pour utiliser cette bibliotheque, il faut changer la configuration par defaut, dans le fichier SdFatConfig.h :
    - #define USE_STANDARD_SPI_LIBRARY 2 
    - #define SD_HAS_CUSTOM_SPI 2
2. [TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus) Library
3. [StackList](https://playground.arduino.cc/Code/StackList/) Library   
