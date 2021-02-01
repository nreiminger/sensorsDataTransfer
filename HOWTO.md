# Configuration de l'environnement de dévelopement

## Installation de Arduino et de la carte flipper: 
Pour installer l'ide, veulliez suivre ce [lien](https://www.arduino.cc/en/software) pour l'installation de Arduino IDE.

Une fois avoir installer l'ide, il faut installer la carte Flipper, ainsi que toutes ses librairies dans l'ide : 
- il faut au préalable ajouter ce [lien](https://gitlab.com/strataggem1/software/device/arduino/arduino_dev_tools/strataggem_arduino_boards/-/raw/master/package_strataggem_index.json) dans la zone de texte dans le menu `File > Preferences > Additional Boards Manager URLs`
- Installer la carte dans l'ide, pour ce faire aller dans le menu `Tools > Board > Boards Manager`, puis rechercher `Strataggem`. Vous pouvez ensuite l'installer en cliquant sur le bouton `Install`.

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
Si la conpiliation ne passe pas, veullez verifier que vous avez bien selectionner la carte Flipper dans le menu `Tools > Board > 
## Installation des différantes livraibries neccesaire pour la confilation du sketch : 

    
