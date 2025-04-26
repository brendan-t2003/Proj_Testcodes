#include <SoftwareSerial.h>
SoftwareSerial mySerial(3, 2); 2

void setup()
{
  //Begin serial communication
  Serial.begin(115200);
  
  //Begin serial communication with SIM800L
  mySerial.begin(115200);

  Serial.println("Initialising"); 
  delay(1000);

  mySerial.println("AT"); //Handshake
  updateSerial();
  
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
}

void loop()
{
  updateSerial();
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward recived serial
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward recived serial to prt
  }
}

