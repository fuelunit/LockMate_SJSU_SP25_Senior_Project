/**
 * Author:  Angel Chavez
 *          Yipeng Liu
 * Date:    03/19/2025
 */

#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

#define TRIG_PIN 6   // HC-SR04 Trigger pin
#define ECHO_PIN 7   // HC-SR04 Echo pin
#define SERVO_PIN 5  // SG-90 Servo pin
#define DISTANCE_THRESHOLD 44 // Locking distance in cm

#define SS_PIN 53   // RFID SS pin
#define RST_PIN 9   // RFID RST pin

const int OPEN_POSITION = 60;   // Servo angle when OPEN
const int CLOSED_POSITION = 150; // Servo angle when CLOSED
const int UNLOCK_TIMER = 10000; // Time passed since last RFID unlock: 10 seconds

const String AUTHORIZED_TAG = "F684EE96"; // Authorized RFID tag

Servo myServo;
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create RFID object

bool isClosed = false; // Track if the servo is "CLOSED"
unsigned long lastOpenedTime = 0; // Store last open time

void setup() {
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init(); // Initialize RFID module

    myServo.attach(SERVO_PIN);
    myServo.write(OPEN_POSITION); // Default to "OPEN" position

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    Serial.println("System Initialized. Waiting for RFID or Ultrasonic trigger...");
}

void loop() {
    unsigned long currentTime = millis();

    // Only check distance if NOT closed and at least 10 seconds have passed since last RFID unlock
    if (!isClosed && (currentTime - lastOpenedTime >= UNLOCK_TIMER)) {
        int distance = getDistance();
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" cm");

        if (distance > 0 && distance < DISTANCE_THRESHOLD) {
            myServo.write(CLOSED_POSITION); // Move to "CLOSED" position
            isClosed = true;
            Serial.println("Box CLOSED by Ultrasonic Sensor.");
        }
    }

    if (checkRFID()) {
        myServo.write(OPEN_POSITION); // Move to "OPEN" position
        isClosed = false;  // Re-enable ultrasonic detection after delay
        lastOpenedTime = millis(); // Record the time of unlocking
        Serial.println("Box OPENED by RFID. Ultrasonic sensor disabled for 10 seconds.");
    }

    delay(500); // Adjust delay as needed
}

int getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    int distance = duration * 0.034 / 2; // Convert to cm
    return distance;
}

bool checkRFID() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return false;
    }

    Serial.print("RFID UID: ");
    String tagID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        tagID += String(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println(tagID);

    if (tagID.equalsIgnoreCase(AUTHORIZED_TAG)) {
        return true;
    }

    Serial.println("Unauthorized RFID Tag.");
    return false;
}