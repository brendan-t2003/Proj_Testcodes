
 
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 4, TXPin = 5;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ gps object
TinyGPSPlus gps;

// serial to arduino from NEO
SoftwareSerial ss(RXPin, TXPin);

void setup(){
  //setting serial comunication
  Serial.begin(9600);
  ss.begin(GPSBaud);
  Serial.println("Start");
}

void loop(){
  //while has a lock and can send 
  while (ss.available() > 0){
    //encode the data 
    gps.encode(ss.read());
    //if gps.location is different output long and lang
    if (gps.location.isUpdated()){
      Serial.print("Latitude= "); 
      Serial.print(gps.location.lat(), 6);
      Serial.print(" Longitude= "); 
      Serial.println(gps.location.lng(), 6);
    }
  }
}