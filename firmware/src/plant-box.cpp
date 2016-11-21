#include "SPI_Motor.h"
#include "neopixel-fn.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

SPI_Motor motor(A0);	//Hootie81's motor shield using A0 CS pin

#define moisturePin A1

unsigned long old_time = 0;
uint8_t retryCount = 0;
String controlCommand = "";

//Function prototypes
void checkWiFi();
void otaHandler();
int plantControl(String cmd);
void processControl();
void turnOffPeripherals();

void rainbow(uint8_t wait);

void setup() {
	WiFi.on();
	System.disableUpdates();
 System.on(firmware_update_pending, otaHandler);

	led.begin();
	led.show(); // Initialize all pixels to 'off'

	Serial.begin(9600);
	pinMode(moisturePin, INPUT);
	pinMode(D7, OUTPUT);
	digitalWrite(D7, LOW);

	Particle.function("control", plantControl);

	if (motor.begin()){
		Particle.publish("plant/status/motorShield", "detected");

		motor.resume();
		motor.A(STOP, 0);
		motor.A(CW);
	}
	else {
		Particle.publish("plant/status/motorShield", "missing");
		//while(1) Particle.process();
	}
}


void loop() {
	checkWiFi();

	processControl();
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
		delay(1000);
		//motor.A(STOP, 0);
		motor.A(0);
	}
	else if(controlCommand == "motoron") {
		motor.A(255);
	}
	else if(controlCommand == "motoroff") {
		motor.A(0);
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
			return -1;
}
