#include "idf/launcher_platform.h"
#include "powerSave.h"
#include <Wire.h>
#include <interface.h>

#define I2C_KB_ADDR 0x5F
#define KB_SDA 47
#define KB_SCL 48

void _setup_gpio() {
    // 1. Peripheral Power Gate (Enables LCD, SD, LoRa)
    launcherGpioOutput(18);
    launcherGpioWrite(18, HIGH);
    launcherDelayMs(50);

    // 2. LCD Backlight
    launcherGpioOutput(17);
    launcherGpioWrite(17, HIGH);

    // 3. SPI Bus Isolation (Keep LoRa, SD, and TFT de-asserted initially)
    launcherGpioOutput(39); // LR1110 NSS
    launcherGpioWrite(39, HIGH);
    launcherGpioOutput(34); // SDCARD CS
    launcherGpioWrite(34, HIGH);
    launcherGpioOutput(10); // TFT CS
    launcherGpioWrite(10, HIGH);
}

void _post_setup_gpio() {
    // Initialize I2C keyboard bus
    Wire.begin(KB_SDA, KB_SCL, 100000);
}

void InputHandler(void) {
    static unsigned long tm = 0;
    if (launcherMillis() - tm < 80) return;
    tm = launcherMillis();

    // Check I2C keyboard co-processor
    Wire.requestFrom((uint8_t)I2C_KB_ADDR, (uint8_t)1);
    if (Wire.available()) {
        uint8_t c = Wire.read();
        if (c > 0 && c != 0xFF) {
            AnyKeyPress = true;
            if (!wakeUpScreen()) yield();

            keyStroke key;
            key.word.push_back((char)c);
            key.pressed = true;

            // Map navigation shortcuts
            if (c == 0x0D || c == 0x0A || c == '\n') { // Enter
                SelPress = true;
                key.enter = true;
            } else if (c == 0x1B || c == 0x08 || c == 0x7F || c == '`') { // ESC or Backspace
                EscPress = true;
                key.del = true;
            } else if (c == ';' || c == 0xB1 || c == 0x1E) { // Up
                UpPress = true;
                PrevPress = true;
            } else if (c == '.' || c == 0xB2 || c == 0x1F) { // Down
                DownPress = true;
                NextPress = true;
            } else if (c == ',' || c == 0xB3) { // Left
                PrevPress = true;
            } else if (c == '/' || c == 0xB4) { // Right
                NextPress = true;
            }

            KeyStroke = key;
            return;
        }
    }

    KeyStroke.Clear();
}

void reboot() {
    launcherConsoleFlush();
    launcherConsoleEnd();
    vTaskDelay(pdMS_TO_TICKS(50));
    ESP.restart();
}
