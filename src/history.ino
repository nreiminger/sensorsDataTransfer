void readLastData() {
   Serial.println("---------Connected : "+String(bleuart.available()));
  // Ouverture de fichier en mode lecture
  Serial.println("History reading");
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
  if (myFile) {
    myFile.println("end");
    myFile.close();
  }
  bool stop = false;
  //tant que le fichier la liste n'est pas vide, depiler la valeur se trouvant au sommet.
  //Comme la dernière valeur empiler est la dernière ligne du fichier, la permière valeur a être dépiler est la valeur la plus recente.
  int diffMillis = 0;
  int m1; //valeur precedante
  int m2; //valeur courante
  
  String prev = stack.pop();
  if(prev.substring(0,3)=="end"){ //si la dernière ligne du fichier (date la plus recente) et égale a end c'est a dire qu'auncune mesure n'a encore été éffecté
    stop = true;
  }else{
    Serial.println(prev + "*******");
    for (int i = 0; i < 5; i++) {
      bleuart.print(String(getValue(prev, ';', i)));
    }
    bleuart.print("m=0"); 
    bleuart.print("**********");
    String mi1 = getValue(prev, ';', 5);
    m1 = getValue(mi1, '=', 1).toInt();
  }
  
  while (!stack.isEmpty() && !stop) {
    String m = "m=";
    String suiv = stack.pop();
    String mi2 = getValue(suiv, ';', 5);
    m2 = getValue(mi2, '=', 1).toInt();
    if (m2 > m1 || suiv.substring(0,3) == "end") {
      stop = true;
    } else {
      diffMillis += m1 - m2;
      for (int j = 0; j < 5; j++) {
        Serial.print(String(getValue(suiv, ';', j)) + ";");
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
