#include <SoftwareSerial.h>

SoftwareSerial sim800l(3, 2); // RX, TX

const char* phoneNumber = "+44xxxxxx"; //phone number
const char* message = "your bike is currently at these coordinates : https://maps.google.com/?q=50.3021563,-4.758964 please type R back to repeat"; // Message to send
bool spamprotection = false; // Flag to track if the message was repeated (as else is just keeps spamming)

void setup() {
    Serial.begin(115200); // 115200 baud rate
    sim800l.begin(115200); // SIM800L at 115200 baud rate

    Serial.println("Initialising SIM800L");
    delay(10000); // Allows time for netwrok connection

    sendSMS(phoneNumber, message); // Send SMS
    setupComs(); // Config SIM800L for receiving messages
}

void loop() {
    incoming();
}

// Function to configure SIM800L
void setupComs() {
    sim800l.println("AT"); // Handshake
    updateSerial();

    sim800l.println("AT+CMGF=1"); // Set SMS to text mode
    updateSerial();

    sim800l.println("AT+CNMI=1,2,0,0,0"); // Configure how SMS messages are handled (defult for now)
    updateSerial();
}

// Function to send an SMS
void sendSMS(const char* phoneNumber, const char* message) {
    sim800l.println("AT+CMGF=1"); // Set SMS to text mode
    delay(1000);

    sim800l.print("AT+CMGS=\""); // SMS start command
    sim800l.print(phoneNumber);
    sim800l.println("\"");
    delay(1000);

    sim800l.print(message); // SMS send
    delay(1000);

    sim800l.write(26); // Tells SIM800 that the message has ended and can send
    delay(3000);
    Serial.println("SMS sent");
}

// Function to check for incoming messages and repeat if R is recived 
void incoming() {
    if (sim800l.available()) {
        String incomingMessage = "";

        while (sim800l.available()) {
            char c = sim800l.read();
            incomingMessage += c;
        }

        Serial.println("Received message: " + incomingMessage);

        // Check if 'R' is received in the message and if the message hasn't been repeated yet
        if ((incomingMessage.indexOf("R") >= 0 || incomingMessage.indexOf("r") >= 0) && !spamprotection) {
            Serial.println("Repeating");
            sendSMS(phoneNumber, message); // Resend last message
            spamprotection = true; // Flag is true if R to stopp repeate exicutions
        } else if (incomingMessage.indexOf("R") < 0 && incomingMessage.indexOf("r") < 0) {
            spamprotection = false; // Reset the flag if a different message is received
        }
    }
}

// Function to update Serial output
void updateSerial() {
    delay(500);
    while (Serial.available()) {
        sim800l.write(Serial.read()); // Forward Serial data to Software Serial
    }
    while (sim800l.available()) {
        Serial.write(sim800l.read()); // Forward Software Serial data to Serial
    }
}

