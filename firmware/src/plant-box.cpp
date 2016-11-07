#include "SPI_Motor.h"
#include "neopixel.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

SPI_Motor singleCard(A0);	//Hootie81's motor shield using A0 CS pin

#define moisturePin A1

#define PIXEL_COUNT 12
#define PIXEL_PIN D3
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

unsigned long old_time = 0;
uint8_t retryCount = 0;

//Function prototypes
void checkWiFi();

void setup() {
	WiFi.on();

	strip.begin();
	strip.show(); // Initialize all pixels to 'off'

	Serial.begin(9600);
	pinMode(moisturePin, INPUT);

	if (singleCard.begin()){
		Particle.publish("plant/status/motorShield", "detected");

		singleCard.resume();
		singleCard.A(STOP, 0);
		singleCard.A(CW);
	}
	else {
		Particle.publish("plant/status/motorShield", "missing");
		while(1) Particle.process();
	}
}

void loop() {
	checkWiFi();

}

void checkWiFi() {
		if(millis() - old_time >= 5000){
			if(retryCount < 10){
				if(!WiFi.ready()){
					WiFi.connect();
					retryCount++;

				}
				else if (!Particle.connected()){
					Particle.connect();
					retryCount++;
				}
			}
			else{
				WiFi.off();
				retryCount = 0;
				WiFi.on();
			}
			old_time = millis();
		}
}
