/*
   ESP32 FULL LOCK EMULATOR
   Replays your captured session exactly

   Board: ESP32 DevKit V1 (38 pin)
   TX2 -> GPIO17
   RX2 -> GPIO16
   GND common with lock interface

   Arduino IDE:
   Board = ESP32 Dev Module
*/

#include <HardwareSerial.h>

HardwareSerial LOCK(2);

// ---------------- UART CONFIG ----------------
#define RXD2 16
#define TXD2 17

#define BAUDRATE 9600
#define SERIAL_MODE SERIAL_8N1
// if no reply try:
// SERIAL_8E1
// SERIAL_8O1

// ------------------------------------------------

void hexPrint(uint8_t b) {
  if (b < 16) Serial.print("0");
  Serial.print(b, HEX);
  Serial.print(" ");
}

void dumpRX(uint32_t timeoutMs) {
  uint32_t t = millis();

  while (millis() - t < timeoutMs) {
    while (LOCK.available()) {
      uint8_t b = LOCK.read();
      hexPrint(b);
      t = millis();
    }
  }
  Serial.println();
}

void sendPkt(const uint8_t *buf, uint16_t len, uint16_t waitMs = 30) {
  Serial.print("TX: ");
  for (int i = 0; i < len; i++) hexPrint(buf[i]);
  Serial.println();

  LOCK.write(buf, len);
  LOCK.flush();

  delay(waitMs);

  Serial.print("RX: ");
  dumpRX(400);
}

// ---------------- CAPTURED PACKETS ----------------

// Wake
const uint8_t WAKE[] = {0xFF, 0x16};

// Sync
const uint8_t SYNC[] = {0xFF, 0x06};

// pkt1860
const uint8_t P1[] = {
  0x02,0x10,0x99,0x99,0xE9,0x49,0x17,0x00,
  0x46,0xC4,0xA7,0x8B,0x30,0x9C,0xDA,0x7B,
  0xAB,0x31,0x4D,0x4F,0xC7,0xD9
};

// pkt1874
const uint8_t P2[] = {
  0x02,0x10,0x99,0x99,0xE7,0x2A,0xE9,0x0B,
  0xFA,0x05,0xF8,0x85,0xBB,0x22,0xE1,0x03,
  0xF2,0x0D,0xF0,0x8D,0xB3,0x4D
};

// pkt1880
const uint8_t P3[] = {
  0x02,0x10,0x99,0x99,0xE9,0x92,0xA9,0x00,
  0xC2,0x0A,0x74,0x71,0x41,0xED,0xD0,0x0F,
  0x20,0xDF,0x31,0xFE,0x49,0x65
};

// pkt1894
const uint8_t P4[] = {
  0x02,0x10,0x99,0x99,0xE7,0xBF,0xA3,0x2E,
  0xE8,0x0C,0xFC,0x87,0xBA,0xB7,0xAB,0x26,
  0xE0,0x04,0xF4,0x8F,0xB2,0x75
};

// pkt1900
const uint8_t P5[] = {
  0x02,0x10,0x99,0x99,0xE9,0x00,0x1E,0x35,
  0xE2,0xD2,0xBB,0x03,0x2C,0x3A,0xED,0x80,
  0xC5,0xE0,0x17,0x24,0x5D,0x00
};

// pkt1914
const uint8_t P6[] = {
  0x02,0x10,0x99,0x99,0xE7,0x00,0x7C,0xC1,
  0x9F,0x37,0xE1,0x89,0xBD,0x08,0x74,0xC9,
  0x97,0x3F,0xE9,0x81,0xB5,0x63
};

// ------------------------------------------------

bool wakeLock() {

  Serial.println("Wake sequence...");

  for (int i = 0; i < 35; i++) {
    LOCK.write(WAKE, sizeof(WAKE));
    LOCK.flush();

    delay(40);

    if (LOCK.available()) {
      Serial.print("RX: ");
      dumpRX(500);
      Serial.println("LOCK AWAKE");
      return true;
    }
  }

  Serial.println("No response.");
  return false;
}

// ------------------------------------------------

void runSession() {

  sendPkt(P1, sizeof(P1), 40);

  sendPkt(SYNC, sizeof(SYNC), 20);
  sendPkt(P2, sizeof(P2), 40);

  sendPkt(P3, sizeof(P3), 40);

  sendPkt(SYNC, sizeof(SYNC), 20);
  sendPkt(P4, sizeof(P4), 40);

  sendPkt(P5, sizeof(P5), 40);

  sendPkt(SYNC, sizeof(SYNC), 20);
  sendPkt(P6, sizeof(P6), 40);
}

// ------------------------------------------------

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("ESP32 FULL LOCK EMULATOR");

  LOCK.begin(BAUDRATE, SERIAL_MODE, RXD2, TXD2);

  delay(200);

  if (wakeLock()) {
    delay(100);
    runSession();
  }
}

void loop() {

}
