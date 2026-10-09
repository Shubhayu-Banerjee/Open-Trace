/*
  OpenTrace — temporary bring-up firmware
  Target: Raspberry Pi RP2040
  Framework: Arduino-Pico core (Raspberry Pi Pico / RP2040)

  Purpose:
    - Confirm the RP2040 boots and enumerates as USB serial.
    - Confirm the eight SN74LVC245A output channels reach the expected GPIOs.
    - Print a channel bitmask when the input state changes.

  IMPORTANT:
    This is a low-speed GPIO polling test, NOT the final high-speed logic analyzer.
    It does not use PIO/DMA and cannot guarantee narrow-pulse capture.
*/

#include <Arduino.h>

static constexpr uint32_t SERIAL_BAUD = 115200;
static constexpr uint32_t REPORT_INTERVAL_MS = 100;

// Based on the current schematic mapping:
// CH1=A1->B1 -> GPIO25, CH2 -> GPIO24, ... CH8 -> GPIO18.
static constexpr uint8_t CHANNEL_PINS[8] = {
  25, 24, 23, 22, 21, 20, 19, 18
};

uint8_t readChannels() {
  uint8_t state = 0;
  for (uint8_t ch = 0; ch < 8; ++ch) {
    if (digitalRead(CHANNEL_PINS[ch]) == HIGH) {
      state |= (uint8_t)(1u << ch);
    }
  }
  return state;
}

void printState(uint8_t state) {
  Serial.print(F("CH1..CH8 = "));
  for (int8_t ch = 7; ch >= 0; --ch) {
    Serial.print((state >> ch) & 1u);
  }

  Serial.print(F("  HEX=0x"));
  if (state < 0x10) Serial.print('0');
  Serial.println(state, HEX);
}

void setup() {
  // Use INPUT: the SN74LVC245A actively drives these GPIOs.
  for (uint8_t ch = 0; ch < 8; ++ch) {
    pinMode(CHANNEL_PINS[ch], INPUT);
  }

  Serial.begin(SERIAL_BAUD);

  // Don't block indefinitely if no serial terminal is open.
  const uint32_t start = millis();
  while (!Serial && (millis() - start < 3000)) {
    delay(10);
  }

  Serial.println();
  Serial.println(F("OpenTrace temporary bring-up firmware"));
  Serial.println(F("RP2040 USB serial: OK"));
  Serial.println(F("8-channel GPIO polling test"));
  Serial.println(F("Mapping: CH1=GPIO25, CH2=GPIO24, CH3=GPIO23, CH4=GPIO22,"));
  Serial.println(F("         CH5=GPIO21, CH6=GPIO20, CH7=GPIO19, CH8=GPIO18"));
  Serial.println(F("Send 's' to print the current state."));
  Serial.println(F("NOTE: slow polling only; not final logic-analyzer capture."));
  printState(readChannels());
}

void loop() {
  static uint8_t previousState = 0xFF;
  static uint32_t lastReport = 0;

  const uint32_t now = millis();
  const uint8_t state = readChannels();

  // Print immediately whenever any channel changes.
  if (state != previousState) {
    Serial.print(millis());
    Serial.print(F(" ms  "));
    printState(state);
    previousState = state;
    lastReport = now;
  }

  // Allow a terminal to request a fresh state.
  while (Serial.available() > 0) {
    const char command = (char)Serial.read();
    if (command == 's' || command == 'S') {
      Serial.print(millis());
      Serial.print(F(" ms  "));
      printState(state);
    }
  }

  // Periodic heartbeat so the terminal shows the firmware is alive.
  if (now - lastReport >= REPORT_INTERVAL_MS) {
    Serial.print(millis());
    Serial.print(F(" ms  "));
    printState(state);
    lastReport = now;
  }
}
