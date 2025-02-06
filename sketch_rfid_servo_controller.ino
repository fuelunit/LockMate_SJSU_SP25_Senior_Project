/**
 * Author: Yipeng Liu
 * Date: 02/05/2025
 */
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#define RST_PIN 9       // Configurable, connected to RST on RC522
#define SS_PIN 53       // Configurable, connected to SDA on RC522

Servo myservo;          // create Servo object to control a servo
const int servo_pin = 5;// servo controll pin should be connected to pin #5 on an arduino mega board
const char uid_stored[] = "F684EE96"; // The only serial number or uid allowed to turn the servo
int pos = 0;    // variable to store the servo position
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for Serial Monitor to open (for Arduino Mega)
  Serial.println("Initializing RFID module...");

  SPI.begin();          // Initialize SPI
  rfid.PCD_Init();      // Initialize RC522
  myservo.attach(servo_pin);  // attaches the servo on pin 9 to the Servo object
  
  Serial.println("Testing SPI communication...");

  // Try writing and reading from SPI
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(53, LOW);  // SS Pin LOW
  byte response = SPI.transfer(0x00);  // Send dummy data
  digitalWrite(53, HIGH);  // SS Pin HIGH
  SPI.endTransaction();

  Serial.print("SPI Response: ");
  Serial.println(response, HEX);

  // Check version
  Serial.println("RFID Reader Ready. Bring a card near...");
  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.print("Version: ");
  Serial.println(version, HEX);

  if (version == 0x00 || version == 0xFF) {
    Serial.println("ERROR: RFID module not detected. Check wiring.");
  } else {
    Serial.println("RC522 module detected!");
  }
}

void loop() {
  // Look for new cards
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  // Print UID
  char uid_string[2 * rfid.uid.size + 1]; // Each byte is 2 hex chars, +1 for null terminator

  // Convert the UID bytes to a hexadecimal string
  for (byte i = 0; i < rfid.uid.size; i++) {
    sprintf(&uid_string[i * 2], "%02X", rfid.uid.uidByte[i]);
  }

  // Print the UID string
  Serial.print("Card UID: ");
  Serial.println(uid_string);

  // Comparison
  if (strcmp(uid_stored, uid_string) == 0) {
    // The strings are equal
    Serial.println("Permit");
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15 ms for the servo to reach the position
    }
    delay(5000);                       // waits 5 s
    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15 ms for the servo to reach the position
    }
  } else {
    // The strings are different
    Serial.println("Deny");
  }

  // Halt PICC
  rfid.PICC_HaltA();
}
