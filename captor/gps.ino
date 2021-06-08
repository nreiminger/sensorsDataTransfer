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
    date = gps.date.year();
    date += "-";
    if(gps.date.month() <10)
    date += "0";
    date += gps.date.month(); 
    date += "-";
    if(gps.date.day() <10)
    date += "0";
    date += gps.date.day();

    
    //hour
    if(gps.time.hour() <10)
    date += "0";
    time += gps.time.hour();
    time += ":";
    if(gps.time.minute() <10)
    date += "0";
    time += gps.time.minute();
    time += ":";
    if(gps.time.second() <10)
    date += "0";
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
