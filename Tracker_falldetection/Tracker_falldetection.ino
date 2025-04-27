#include <Wire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// MPU6050 config
#define MPU6050_ADDR 0x68  // I2C address MPU6050 for coms
const float FALL_THRESHOLD = 120.0; // Tilt angle threshold for fall detection (degrees MUST CHANGE TO VALUES FROM TEST)
unsigned long lastMPUCheck = 0;
const unsigned long MPU_INTERVAL = 15000; // 15 seconds
static const int GPSRX = 4, GPSTX = 5;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ssGPS(GPSRX, GPSTX); //GPS
const char* phoneNumber = "+44xxxxxxxx"; 
bool gsmBusy = false; // Flag to track GSM state
// GPS lat lng storage strings used throughout
String latString = "";
String lngString = "";
// Flag to prevent spamming fall alerts once fallen
bool fallDetected = false;

void setup() {
    Serial.begin(115200);   
    Serial1.begin(115200);    
    ssGPS.begin(GPSBaud);  //(9600)

    Serial.println("Initialising SIM800L");
    delay(1000); //note this needs to change to the average antiuation time as atm too quick 

    Serial.println("Initialising MPU6050");
    Wire.begin();
    delay(500);

    // Wake up MPU6050
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x6B);  // PWR register
    Wire.write(0x00);  // Wake up MPU6050
    if (Wire.endTransmission() != 0) {
        Serial.println("Failed to communicate with MPU6050"); 
        while (1);
    }
    Serial.println("MPU6050 online");

    delay(1000); 

    setupComs(); 
    sendStartupMessage(); 
    Serial.println("Setup complete");
}

void loop() {
    GPSread(); /
    sampleGPS(); /
    incoming(); 

    // Check MPU6050 at set interval (every 15 seconds)
    if (millis() - lastMPUCheck >= MPU_INTERVAL) {
        lastMPUCheck = millis();
        checkMPU(); // Check the tilt angle
    }
}

// Function to continuously read GPS raw data
void GPSread() {
    while (ssGPS.available() > 0) {
        gps.encode(ssGPS.read());
    }
}

// Function to sample GPS data into lat and lng strings
void sampleGPS() {
    if (gps.location.isUpdated()) {
        latString = String(gps.location.lat(), 6);
        lngString = String(gps.location.lng(), 6);
        Serial.print("Latitude: ");
        Serial.print(latString);
        Serial.print(" Longitude: ");
        Serial.println(lngString);
    }
}

// Function to check MPU6050 and detect bike fall
void checkMPU() {
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x3B);  // Register for acelaration data
    Wire.endTransmission(false);
   Wire.requestFrom((uint8_t)MPU6050_ADDR, (uint8_t)14, (bool)true);
    int16_t ax = Wire.read() << 8 | Wire.read();
    int16_t ay = Wire.read() << 8 | Wire.read();
    int16_t az = Wire.read() << 8 | Wire.read();

    // Convert to aceleration in G
    float ax_g = ax / 16384.0;
    float ay_g = ay / 16384.0;
    float az_g = az / 16384.0;

    // Calculate tilt angle in degrees
    float angleX = atan2(ay_g, az_g) * 180.0 / PI;
    float angleY = atan2(ax_g, az_g) * 180.0 / PI;

    Serial.print("Tilt Angle X: ");
    Serial.print(angleX, 2);
    Serial.print("° | Tilt Angle Y: ");
    Serial.print(angleY, 2);
    Serial.println("°");

    // Check if bike is tilted beyond the threshold angle
    if (abs(angleX) > FALL_THRESHOLD || abs(angleY) > FALL_THRESHOLD) {
        if (!fallDetected) {  // Prevent spamming
            Serial.println("fall detected Sending alert**");
            sendFallAlert();
            fallDetected = true;
        }
    } else {
        fallDetected = false; // Reset flag if bike is back upright
    }
}

// Configuration Fuction for SIM800L
void setupComs() {
    Serial1.println("AT"); // Handshake
    updateSerial();

    Serial1.println("AT+CMGF=1"); // Set SMS to text mode
    updateSerial();

    Serial1.println("AT+CNMI=1,2,0,0,0"); // Configure SMS handling
    updateSerial();
}

// Function to send startup message
void sendStartupMessage() {
    String startupMessage = "Motorcycle Tracker online. Please wait 1 min before replying with 'C.' for Coordinates";
    sendSMS(phoneNumber, startupMessage.c_str());
}

// Function to send an SMS alert if bike falls (above threshold angle)
void sendFallAlert() {
    String alertMessage = "ALERT: The bike seems to have fallen Please check";
    sendSMS(phoneNumber, alertMessage.c_str());
}

// Function to send an SMS
void sendSMS(const char* phoneNumber, const char* message) {
    gsmBusy = true; // Mark GSM as busy
    Serial1.println("AT+CMGF=1"); // Set SMS to text mode
    delay(1000);

    Serial1.print("AT+CMGS=\"");
    Serial1.print(phoneNumber);
    Serial1.println("\"");
    delay(1000);

    Serial1.print(message);
    delay(1000);

    Serial1.write(26); // Ctrl+Z to send the SMS
    delay(3000);
    Serial.println("SMS sent");
    gsmBusy = false; // Mark GSM as idle
}

// Function to check for incoming SMS and send GPS data if "C." is received
void incoming() {
    if (Serial1.available()) {
        gsmBusy = true; // Mark GSM as busy
        String incomingMessage = Serial1.readString(); //string is problamatic this is why i am getting garbled responses 
        incomingMessage.trim(); 

        Serial.println("Received message: " + incomingMessage);

        // Check for "C." command
        if (incomingMessage.indexOf("C.") >= 0 || incomingMessage.indexOf("c.") >= 0) {
            String gpsMessage = "Your bike is currently at: https://maps.google.com/?q=" + latString + "," + lngString;
            sendSMS(phoneNumber, gpsMessage.c_str());
        }
        gsmBusy = false; // Mark GSM as idle
    }
}

// Function to update Serial output 
void updateSerial() {
    delay(500);
    while (Serial.available()) {
        Serial1.write(Serial.read());
    }
    while (Serial1.available()) {
        Serial.write(Serial1.read());
    }
}
