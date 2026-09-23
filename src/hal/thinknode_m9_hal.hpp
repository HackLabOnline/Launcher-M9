#pragma once
#include <LovyanGFX.hpp>
#include <Arduino.h>

class LGFX_ThinkNodeM9 : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789  _panel_instance;
  lgfx::Bus_SPI       _bus_instance;
  lgfx::Light_PWM     _light_instance;

public:
  LGFX_ThinkNodeM9() {
    { // Configure SPI Bus (ThinkNode M9 shared SPI: SCK=40, MOSI=47, MISO=38)
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000; // 40MHz safe for shared bus
      cfg.freq_read  = 16000000;
      cfg.pin_sclk = 40; // ESP32_SCK
      cfg.pin_mosi = 47; // ESP32_MOSI
      cfg.pin_miso = 38; // ESP32_MISO (shared with SD)
      cfg.pin_dc   = 15; // LCD_RS / DC
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    { // Configure Display Panel (ST7789 2.4" 320x240)
      auto cfg = _panel_instance.config();
      cfg.pin_cs           = 16; // LCD_CS
      cfg.pin_rst          = 14; // LCD_RESET
      cfg.pin_busy         = -1;
      cfg.panel_width      = 240;
      cfg.panel_height     = 320;
      cfg.offset_x         = 0;
      cfg.offset_y         = 0;
      cfg.offset_rotation  = 1; // Landscape 320x240
      cfg.readable         = false;
      cfg.invert           = true;
      cfg.rgb_order        = false;
      cfg.dlen_16bit       = false;
      cfg.bus_shared       = true; // CRITICAL: Bus shared with SD & LoRa!
      _panel_instance.config(cfg);
    }

    { // Configure Backlight on BL_EN (GPIO 17)
      auto cfg = _light_instance.config();
      cfg.pin_bl = 17; // BL_EN (Backlight MOS control)
      cfg.invert = false;
      cfg.freq   = 44100;
      cfg.pwm_channel = 7;
      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    setPanel(&_panel_instance);
  }
};

// Global HAL Helper: Turns on Power Rails, Backlight, and Isolates Bus
inline void thinknode_m9_init_power_and_bus() {
  // CRITICAL 1: Turn ON peripheral LDO power rail (LCD, Sensors, SD Card)
  pinMode(18, OUTPUT);       // VDD_PERIPH_EN
  digitalWrite(18, HIGH);    // MUST BE HIGH for LCD to receive power!
  delay(50);                 // Settle time for power rail

  // CRITICAL 2: Isolate Semtech LR1110 LoRa & SD CS lines
  pinMode(39, OUTPUT);       // LR_NSS (LoRa Chip Select)
  digitalWrite(39, HIGH);    // De-assert LoRa to free MISO line!

  pinMode(48, OUTPUT);       // SD_CS
  digitalWrite(48, HIGH);    // De-assert SD CS until accessed

  pinMode(16, OUTPUT);       // LCD_CS
  digitalWrite(16, HIGH);    // De-assert Display CS

  // CRITICAL 3: Turn on LCD Backlight
  pinMode(17, OUTPUT);       // BL_EN
  digitalWrite(17, HIGH);    // Illuminate screen
}
