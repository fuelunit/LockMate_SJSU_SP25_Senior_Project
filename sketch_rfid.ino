#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9  // Configurable, connected to RST on RC522
#define SS_PIN 53  // Configurable, connected to SDA on RC522

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for Serial Monitor to open (for Arduino Mega)
  Serial.println("Initializing RFID module...");

  SPI.begin();          // Initialize SPI
  rfid.PCD_Init();      // Initialize RC522
  
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
  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Halt PICC
  rfid.PICC_HaltA();
}
