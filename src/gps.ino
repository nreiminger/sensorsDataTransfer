void sensorGPS(Value* data){
  String date;
  String time;
  double lt;
  double lg;
  //date
  if ( gps.location.isValid() && gps.location.age() < 5000 )
  {
    digitalWrite(LED_GREEN,HIGH);
    lt = gps.location.lat();
    lg = gps.location.lng();
  }
  else
  {
    digitalWrite(LED_GREEN,LOW);
    lt = 00.000000;
    lg = 00.000000;
  }
  if (gps.date.isValid() && gps.time.isValid() && gps.date.year() > 2020) {
    digitalWrite(LED_RED,HIGH);   
    //date 
    date = gps.date.day();
    date += "/";
    date += gps.date.month();
    date += "/";
    date += gps.date.year();
    
    //hour
    time = gps.time.hour();
    time += ":";
    time += gps.time.minute();
    time += ":";
    time += gps.time.second();
  }
  else
  {
    digitalWrite(LED_RED,LOW);
    date = "0/0/2000";
    time = "0:0:0";
  }
    
  data->date += date;
  data->time += time;  
  data->pms += sensorSPS30();
  data->lattitude = lt;
  data->longitude = lg;
  data->millis += millis();

  
}
