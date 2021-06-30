void readLastData() {
  Serial.println("History reading");
  myFile = SD.open("data", FILE_READ);
  bool reading = true; 
  int millis_prec;
  int millis_suiv;
  int diffMilli=0;
  String mi;
  //instanciation de la pile qui va acceullir toutes les mesures
  if (myFile) {
    //envoie de la première données 
    int posAvantDernierChar = myFile.size() -3;  
    String data = readNextLine(posAvantDernierChar); //read last line
    mi = getValue(data, ';', 5);
    for (int i = 0; i < 5; i++) {
          Serial.print(String(getValue(data, ';', i))+";");
          bleuart.print(String(getValue(data, ';', i)));        
    }
    Serial.println("m=0");
    millis_prec = getValue(mi,'=', 1).toInt();
    while (myFile.available() && reading) {
      String m = "m=";
      //lecture de la ligne courante, la fin de la ligne est marqué par '\n'
      String data = readNextLine(myFile.position()-3);
      mi = getValue(data, ';', 5);
      millis_suiv = getValue(mi,'=', 1).toInt();
      if(millis_prec > millis_suiv && myFile.position() >= 0 && data != "deconnexion"){
        for (int i = 0; i < 5; i++) {
          Serial.print(String(getValue(data, ';', i))+";");
          bleuart.print(String(getValue(data, ';', i)));
        }
        diffMilli = diffMilli + (millis_prec - millis_suiv);
        m += diffMilli;
        Serial.println(";"+m);
        bleuart.print(m);
        millis_prec = millis_suiv; 
      }else{
        reading=false;
      }   
  }
  myFile.close();
  Serial.println("End history reading");
 }
}

String readNextLine(int pos){
    String line = "";
    myFile.seek(pos); //postionné le curseur juste avant le dernier char après y a encore un \n donc d'ou le -3
    char cara = myFile.read(); 
    while( cara != '\n'){
      line = cara + line;  
      myFile.seek(myFile.position() -2);
      cara = myFile.read();
    }
    return line;     
} 
