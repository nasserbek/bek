#include <Arduino.h>

#include <Arduino.h>

HardwareSerial Port(2);

static const int RX_PIN = 16;
static const int TX_PIN = 17;
static const uint32_t BAUD = 9600;

// ===================== PACKETS =====================

// --- TX ---
static const uint8_t pktWake[] = {0xFF, 0x16};
static const uint8_t pkt3642[] = {0x02,0x10,0x99,0x99,0xE9,0x41,0xE8,0x75,0xF0,0x55,0xF8,0x51,0x58,0xF5,0xC4,0x75,0xD1,0x7D,0x1E,0xA0,0xF4,0x23};
static const uint8_t pkt3654[] = {0xFF,0x06};
static const uint8_t pkt3656[] = {0x02,0x10,0x99,0x99,0xE7,0xEB,0x05,0x8E,0xC2,0x75,0x57,0x0C,0x89,0x4E,0x9F,0xE0,0x24,0x87,0xF6,0xD7,0x77,0x7A};
static const uint8_t pkt3662[] = {0x02,0x01,0x00,0x08,0xF5,0x01,0x01};
static const uint8_t pkt3670[] = {0x02,0x01,0x00,0x57,0xF5,0x01,0xB2};
static const uint8_t pkt3678[] = {0x02,0x01,0x00,0x57,0xF6,0x01,0xB1};
static const uint8_t pkt3684[] = {0x02,0x04,0x00,0x4C,0xF6,0xA0,0x63,0xE1,0x38,0x9E};
static const uint8_t pkt3694[] = {0x02,0x04,0x00,0x50,0xF6,0xB0,0x4C,0xF6,0x39,0x8B};
static const uint8_t pkt3704[] = {0x02,0x04,0x01,0x00,0x00,0x71,0x1F,0xFE,0x38,0x35};
static const uint8_t pkt3708[] = {0x02,0x02,0x00,0x05,0xF6,0xFE,0x38,0xCD};
static const uint8_t pkt3714[] = {0x02,0x02,0x00,0x4A,0xF6,0x43,0x83,0xF8};
static const uint8_t pkt3722[] = {0x02,0x00,0x00,0x00,0x01,0xFF};

// ===================== STATE =====================

enum State {
  WAKE, RUN, S2, S3, S4, S5,
  S6, S7, S8, S9, S10, S11, S12, DONE
};

State state = WAKE;

// ===================== HELPERS =====================

void sendPkt(const uint8_t* data, size_t len) {
  Port.write(data, len);

  Serial.print("TX: ");
  for (size_t i=0;i<len;i++) Serial.printf("%02X ", data[i]);
  Serial.println();
}

bool waitFor(uint8_t expected, uint32_t timeout) {
  uint32_t t0 = millis();

  while (millis() - t0 < timeout) {
    if (Port.available()) {
      uint8_t b = Port.read();
      Serial.printf("RX: %02X\n", b);
      if (b == expected) return true;
    }
  }
  return false;
}

void dumpRX(uint32_t timeout) {
  uint32_t t0 = millis();
  while (millis() - t0 < timeout) {
    while (Port.available()) {
      Serial.printf("%02X ", Port.read());
    }
  }
  Serial.println();
}

bool rxAvailable() {
  return Port.available() > 0;
}

uint8_t readByte() {
  return Port.read();
}
bool woke = false;
uint32_t start = millis();
uint32_t t0 = millis();
// ===================== SETUP =====================

void setup() {
  Serial.begin(115200);
  Port.begin(BAUD, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("UART state machine started");
}

// ===================== LOOP =====================

void loop() {

  switch (state) {

    // ================= WAKE STATE =================
    case WAKE: 

      Serial.println("Entering WAKE mode...");

       start = millis();
       woke = false;

      while (!woke) {

        // send wake command
        sendPkt(pktWake, sizeof(pktWake));

        // wait short window for response
        t0 = millis();
        while (millis() - t0 < 150) {

          if (rxAvailable()) {
            uint8_t b = readByte();

            Serial.printf("RX: %02X\n", b);

            // ANY response = device awake
            woke = true;
            break;
          }
        }

        delay(150); // pacing between retries

        // optional safety timeout
        if (millis() - start > 10000) {
          Serial.println("Wake timeout, retrying...");
          start = millis();
        }
      }
      delay(2000);
      Serial.println("Device responded → moving to RUN");
      state = RUN;
      break;

    case RUN:
      sendPkt(pkt3642, sizeof(pkt3642));
      dumpRX(300);
      state = S2;
      break;

    case S2:
      sendPkt(pkt3654, sizeof(pkt3654));
      waitFor(0x06, 200);
      state = S3;
      break;

    case S3:
      sendPkt(pkt3656, sizeof(pkt3656));
      waitFor(0x06, 200);
      state = S4;
      break;

    case S4:
      sendPkt(pkt3662, sizeof(pkt3662));
      dumpRX(200);
      state = S5;
      break;

    case S5:
      sendPkt(pkt3670, sizeof(pkt3670));
      dumpRX(200);
      state = S6;
      break;

    case S6:
      sendPkt(pkt3678, sizeof(pkt3678));
      waitFor(0x06, 200);
      state = S7;
      break;

    case S7:
      sendPkt(pkt3684, sizeof(pkt3684));
      waitFor(0x06, 200);
      state = S8;
      break;

    case S8:
      sendPkt(pkt3694, sizeof(pkt3694));
      waitFor(0x06, 200);
      state = S9;
      break;

    case S9:
      sendPkt(pkt3704, sizeof(pkt3704));
      waitFor(0x06, 200);
      state = S10;
      break;

    case S10:
      sendPkt(pkt3708, sizeof(pkt3708));
      waitFor(0x06, 200);
      state = S11;
      break;

    case S11:
      sendPkt(pkt3714, sizeof(pkt3714));
      waitFor(0x06, 200);
      state = S12;
      break;

    case S12:
      sendPkt(pkt3722, sizeof(pkt3722));
      waitFor(0x06, 200);
      Serial.println("TRACE COMPLETE");
      state = DONE;
      break;

    case DONE:
      delay(3000);
    //  state = WAKE;
      break;
  }

  delay(20);
}
