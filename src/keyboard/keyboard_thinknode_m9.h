#pragma once
#include <Arduino.h>

#ifndef LAUNCHER_KEY_UP
#define LAUNCHER_KEY_UP     1
#define LAUNCHER_KEY_DOWN   2
#define LAUNCHER_KEY_LEFT   3
#define LAUNCHER_KEY_RIGHT  4
#define LAUNCHER_KEY_ENTER  5
#define LAUNCHER_KEY_BACK   6
#define LAUNCHER_KEY_TAB    7
#endif

class ThinkNodeKeyboard {
public:
  static void begin();
  static bool hasKey();
  static uint8_t readKey();
  static uint8_t mapKeyToLauncher(uint8_t raw);
};
