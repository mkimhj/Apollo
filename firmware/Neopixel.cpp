#include <Arduino.h>
#include "Pins.h"

const uint16_t number_of_pixels = 12;
const uint16_t number_of_LEDs = number_of_pixels * 3;
uint8_t led_GRB_array[number_of_LEDs];

uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

void show() {
  noInterrupts();

  for (int i = 0; i < number_of_LEDs; i++) {
    for (int j = 7; j >= 0; j--) {
      NRF_GPIO->OUTSET = (1UL << LED);
      if (led_GRB_array[i] & (0x01 << j)) {
        __ASM ( \
                " NOP\n\t" \
                " NOP\n\t" \
                " NOP\n\t" \
                " NOP\n\t" \
                " NOP\n\t" \
              );
        NRF_GPIO->OUTCLR = (1UL << LED);
      }
      else {
        NRF_GPIO->OUTCLR = (1UL << LED);
        __ASM ( \
                " NOP\n\t" \
                " NOP\n\t" \
                " NOP\n\t" \
              );
      }
    }
  }
  delayMicroseconds(50); // latch and reset WS2812.
  interrupts();
}

void begin() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  // Initialize the ledBar array - all LEDs OFF.
  for (int i = 0; i < number_of_LEDs; i++) {
    led_GRB_array[i] = 0;
  }
  show();

  delay(1);
}

void set_pixel_color(uint16_t n, uint32_t c) {
  if (n < number_of_pixels) {
    uint8_t
    r = (uint8_t)(c >> 16),
    g = (uint8_t)(c >>  8),
    b = (uint8_t)c;
    led_GRB_array[n * 3] = g;
    led_GRB_array[(n * 3) + 1] = r;
    led_GRB_array[(n * 3) + 2] = b;
  }
}

void red_loop() {
  for (int i = 0; i < 12; i++) {
    set_pixel_color(i, Color(255, 0, 0));
    show();
    delay(100);
    set_pixel_color(i, Color(0, 0, 0));
    show();
  }
}

