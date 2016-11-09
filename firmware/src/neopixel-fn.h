/*
Written by: kennethlimcp
Created on: 05 Nov 2016
*/

#include "application.h"
#include "neopixel.h"


#define PIXEL_COUNT 12
#define PIXEL_PIN D3
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel led = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

//Declare function prototypes
void rainbow(uint8_t wait);
uint32_t Wheel(byte WheelPos);
void colorWipe(uint32_t c, uint8_t wait);

void rainbow(uint8_t wait) {
	uint16_t i, j;

	for(j=0; j<256; j++) {
		for(i=0; i<led.numPixels(); i++) {
			led.setPixelColor(i, Wheel((i+j) & 255));
		}
		led.show();
		delay(wait);
	}
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
	if(WheelPos < 85) {
		return led.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
		} else if(WheelPos < 170) {
			WheelPos -= 85;
			return led.Color(255 - WheelPos * 3, 0, WheelPos * 3);
			} else {
				WheelPos -= 170;
				return led.Color(0, WheelPos * 3, 255 - WheelPos * 3);
			}
		}

// Fill the dots one after the other with a color, wait (ms) after each one
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<led.numPixels(); i++) {
    led.setPixelColor(i, c);
    led.show();
    delay(wait);
  }
}
