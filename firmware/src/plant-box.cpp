#include "SPI_Motor.h"
#include "neopixel-fn.h"
#include "functions.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
STARTUP(pinMode(PIXEL_PIN, INPUT));

SPI_Motor motor(A0);	//Hootie81's motor shield using A0 CS pin

#define moisturePin A1

unsigned long wifiCheckTime = 0;
unsigned long publishCheckTime = 0;
const unsigned long PUBLISH_PERIOD_MS = 60000;

uint8_t retryCount = 0;
String controlCommand = "";

//Function prototypes
void checkWiFi();
void otaHandler();
int plantControl(String cmd);
void processControl();
void turnOffPeripherals();
void publishData();
void rainbow(uint8_t wait);
void publishData();
void deviceNameHandler(const char *topic, const char *data);
uint32_t ping(pin_t trig_pin, pin_t echo_pin, uint32_t wait, bool info);

void setup() {
	WiFi.on();
	System.disableUpdates();
 System.on(firmware_update_pending, otaHandler);

	led.begin();
 colorWipe(led.Color(0, 0, 0), 0); //off
	led.show(); // Initialize all pixels to 'off'

	Serial.begin(9600);
	pinMode(moisturePin, INPUT);
	pinMode(D7, OUTPUT);
	digitalWrite(D7, LOW);

	Particle.function("control", plantControl);
 Particle.subscribe("spark/", deviceNameHandler);

	if (motor.begin()){
		Particle.publish("plant/status/motorShield", "detected");

		motor.resume();
		motor.A(STOP, 0);
		motor.A(CW);
	}
	else {
		Particle.publish("plant/status/motorShield", "missing");
	}
}


void loop() {
	checkWiFi();

	processControl();

	if(millis() - publishCheckTime > PUBLISH_PERIOD_MS) {
 	uint32_t height = 8 - ping(D1, D2, 20, false);
		uint32_t moisture = analogRead(moisturePin);
		publishData(height, moisture);
		getData();
	}
}

void checkWiFi() {
		if(millis() - wifiCheckTime >= 1000){
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
			wifiCheckTime = millis();
		}
}

void otaHandler () {
	turnOffPeripherals();

	System.enableUpdates();
	digitalWrite(D7, HIGH);
}

void turnOffPeripherals() {
	motor.A(STOP, 0);
	motor.standby();
 colorWipe(led.Color(0, 0, 0), 0); //off
	led.show();
}

void processControl() {
	if(controlCommand == "safemode") {
			turnOffPeripherals();
			System.enterSafeMode();
	}
	else if(controlCommand == "testlight") {
		rainbow(20);
		colorWipe(led.Color(0, 0, 0), 0); //off
		led.show();
	}
	else if(controlCommand == "testmotor") {
		motor.A(255);
		delay(3000);
		//motor.A(STOP, 0);
		motor.A(0);
	}
	else if(controlCommand == "motoron") {
		motor.A(255);
	}
	else if(controlCommand == "motoroff") {
		motor.A(0);
	}
	else if(controlCommand == "lighton") {
		colorWipe(led.Color(255, 255, 255), 0); //off
		led.show();
	}
	else if(controlCommand == "lightoff") {
		colorWipe(led.Color(0, 0, 0), 0); //off
		led.show();
	}
	controlCommand = "";
}

int plantControl(String cmd) {
		if(cmd == "safemode") {
			controlCommand = "safemode";
			return 1;
		}
		else if(cmd == "testlight") {
			controlCommand = "testlight";
			return 2;
		}
		else if(cmd == "testmotor") {
			controlCommand = "testmotor";
			return 3;
		}
		else if(cmd == "motoron") {
			controlCommand = "motoron";
			return 4;
		}
		else if(cmd == "motoroff") {
			controlCommand = "motoroff";
			return 5;
		}
		else if(cmd == "lighton") {
			controlCommand = "lighton";
			return 6;
		}
		else if(cmd == "lightoff") {
			controlCommand = "lightoff";
			return 7;
		}
			return -1;
}
