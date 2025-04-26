#include <SoftwareSerial.h>

SoftwareSerial sim800l(11,10); // RX, TX

void setup() {
    Serial.begin(115200); // Initialize Serial Monitor at 115200 baud rate
    sim800l.begin(115200); // Initialize SIM800L at 115200 baud rate (important: 9600 doesn't work)

    Serial.println("Initialising SIM800L"); // Print to show it’s initializing
    delay(10000);

    const char* phoneNumber = "+44xxxxxx"; 
    const char* message = "your bike is currently at these coordinates please type r back to repeate"; 

    Serial.print("Sending SMS to: "); 
    Serial.println(phoneNumber); 

    sendSMS(phoneNumber, message); 
}

void loop() {
} //Since we only sending once atm

// Calling function to send the SMS to phone
void sendSMS(const char* phoneNumber, const char* message) {
    sim800l.println("AT+CMGF=1"); // Set SMS to text mode
    delay(1000);

    sim800l.print("AT+CMGS=\""); // SMS start command
    sim800l.print(phoneNumber); 
    sim800l.println("\""); // End line
    delay(1000);

    sim800l.print(message);
    delay(1000);

    sim800l.write(26); // Ctrl+Z to send the SMS
    delay(3000); 
    Serial.print("sent");
}
