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
String controlCommand = "";

//Function prototypes
void checkWiFi();
void otaHandler();
int plantControl(String cmd);
void turnOffPeripherals();

void setup() {
	WiFi.on();
	System.disableUpdates();
 System.on(firmware_update_pending, otaHandler);

	strip.begin();
	strip.show(); // Initialize all pixels to 'off'

	Serial.begin(9600);
	pinMode(moisturePin, INPUT);
	pinMode(D7, OUTPUT);
	digitalWrite(D7, LOW);

	Particle.function("control", plantControl);

	if (singleCard.begin()){
		Particle.publish("plant/status/motorShield", "detected");

		singleCard.resume();
		singleCard.A(STOP, 0);
		singleCard.A(CW);
	}
	else {
		Particle.publish("plant/status/motorShield", "missing");
		//while(1) Particle.process();
	}
}


void loop() {
	checkWiFi();
}

void checkWiFi() {
		if(millis() - old_time >= 2000){
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

void otaHandler () {
	turnOffPeripherals();

	System.enableUpdates();
	digitalWrite(D7, HIGH);
}

void turnOffPeripherals() {
	singleCard.A(STOP, 0);
	singleCard.standby();
	strip.show();
}

void processControl(String arg) {
	if(arg == "safemode") {
			turnOffPeripherals();
			System.enterSafeMode();
	}
	controlCommand = "";
}

int plantControl(String cmd) {
		if(cmd == "safemode") {
			controlCommand = "safemode";
			return 1;
		}
		else
			return 0;
}
