#include "keyboard_thinknode_m9.h"
#include <Wire.h>

#define KB_I2C_ADDR   0x5F
#define KB_SDA_PIN    20
#define KB_SCL_PIN    21
#define KB_INT_PIN    12

static volatile bool g_keyAvailable = false;

static void IRAM_ATTR kb_interrupt_handler() {
  g_keyAvailable = true;
}

void ThinkNodeKeyboard::begin() {
  Wire.begin(KB_SDA_PIN, KB_SCL_PIN, 400000);
  pinMode(KB_INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(KB_INT_PIN), kb_interrupt_handler, FALLING);
}

bool ThinkNodeKeyboard::hasKey() {
  return g_keyAvailable || (digitalRead(KB_INT_PIN) == LOW);
}

uint8_t ThinkNodeKeyboard::readKey() {
  Wire.requestFrom((uint8_t)KB_I2C_ADDR, (uint8_t)1);
  if (Wire.available()) {
    uint8_t rawChar = Wire.read();
    g_keyAvailable = false;
    return mapKeyToLauncher(rawChar);
  }
  g_keyAvailable = false;
  return 0;
}

// Maps ThinkNode physical keycodes to M5Launcher standardized navigation codes
uint8_t ThinkNodeKeyboard::mapKeyToLauncher(uint8_t raw) {
  switch (raw) {
    case 0x0D: // Enter key
    case '\n':
      return LAUNCHER_KEY_ENTER;

    case 0x1B: // Esc / Back
    case 0x08: // Backspace
      return LAUNCHER_KEY_BACK;

    case 0xB5: // Up arrow
    case 'w':
    case 'W':
      return LAUNCHER_KEY_UP;

    case 0xB6: // Down arrow
    case 's':
    case 'S':
      return LAUNCHER_KEY_DOWN;

    case 0xB4: // Left arrow
    case 'a':
    case 'A':
      return LAUNCHER_KEY_LEFT;

    case 0xB7: // Right arrow
    case 'd':
    case 'D':
      return LAUNCHER_KEY_RIGHT;

    case '\t': // Tab
      return LAUNCHER_KEY_TAB;

    default:
      return raw; // Standard alphanumeric key
  }
}
