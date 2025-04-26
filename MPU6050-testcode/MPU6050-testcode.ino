#include <Wire.h>

#define MPU6050_ADDR 0x68  // I2C address MPU6050

void setup() {
    Serial.begin(115200);
    Wire.begin();  // Start I2C communication
    delay(500);  // Allow MPU6050 to power up

    // Wake up MPU6050 
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x6B);  // PWR register
    Wire.write(0x00);  // Wake up MPU6050
    if (Wire.endTransmission() != 0) {
        Serial.println("Failed to communicate with MPU6050");
        while (1);
    }
    Serial.println("MPU6050 online");

    delay(100);
}

void loop() {
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x3B);  // Register where acceleration data starts
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDR, 14, true);  // Request 14 bytes (ALL DATA)

    // Read acceleration data
    int16_t ax = Wire.read() << 8 | Wire.read();
    int16_t ay = Wire.read() << 8 | Wire.read();
    int16_t az = Wire.read() << 8 | Wire.read();

    // Convert and display acceleration
    Serial.print("Acceleration (m/s²): X=");
    Serial.print(ax / 16384.0, 2);
    Serial.print(" Y=");
    Serial.print(ay / 16384.0, 2);
    Serial.print(" Z=");
    Serial.println(az / 16384.0, 2);

    delay(500);
}
