void readLastData() {
  Serial.println("---------"+String(bleuart.notifyEnabled()));
  // Ouverture de fichier en mode lecture
  Serial.println("History reading");
  myFile = SD.open("test.txt", FILE_READ);
  bool reading = true; 
  int millis_prec;
  int millis_suiv;
  int diffMilli=0;
  String mi;
  //instanciation de la pile qui va acceullir toutes les mesures
  if (myFile) {
    //envoie de la première données 
    String data = myFile.readStringUntil('\n');
    mi = getValue(data, ';', 5);
    millis_prec = getValue(mi,'=', 1).toInt();
    Serial.println(millis_prec);
    for (int i = 0; i < 5; i++) {
      Serial.print(String(getValue(data, ';', i))+";");
      bleuart.print(String(getValue(data, ';', i)));
    }
    bleuart.print("m=0"); 
    Serial.println(" ***********m=0");
    while (myFile.available() && reading) {
      String m = "m=";
      //lecture de la ligne courante, la fin de la ligne est marqué par '\n'
      String data = myFile.readStringUntil('\n');
      mi = getValue(data, ';', 5);
      millis_suiv = getValue(mi,'=', 1).toInt();
      if(millis_prec > millis_suiv){
        for (int i = 0; i < 5; i++) {
          Serial.print(String(getValue(data, ';', i))+";");
          bleuart.print(String(getValue(data, ';', i)));
        }
        diffMilli = diffMilli + (millis_prec - millis_suiv);
        Serial.print(";");
        Serial.print(millis_prec - millis_suiv);
        m += diffMilli;
        Serial.println("*******"+m);
        bleuart.print(m);
        millis_prec = millis_suiv; 
      }else{
        reading=false;
      }
        
  }
  myFile.close();
  //ecriture d'un flag pour spécifier a quelle endroit est ce qu'on a fini de lire le fichier
  }
}
