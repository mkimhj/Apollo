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

uint32_t get_pixel_color(uint16_t n){
    uint8_t
    g = led_GRB_array[n * 3],
    r = led_GRB_array[(n * 3) + 1],
    b = led_GRB_array[(n * 3) + 2];
    return Color(r, g, b);
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

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  
  for(j = 0; j < 256; j++) {
    for(i = 0; i < number_of_pixels; i++) {
      set_pixel_color(i, Wheel((i + j) & 255));
    }
    show();
    delay(wait);
  }

  for(i = 0; i < number_of_pixels; i++){
    set_pixel_color(i, Color(0, 0, 0));
  }
  show();
}

