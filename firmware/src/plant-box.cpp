#include "SPI_Motor.h"
#include "neopixel-fn.h"
#include "functions.h"
#include "SparkIntervalTimer.h"


SYSTEM_MODE(SEMI_AUTOMATIC);
STARTUP(pinMode(PIXEL_PIN, INPUT));

SPI_Motor motor(A0);	//Hootie81's motor shield using A0 CS pin

#define moisturePin A1

unsigned long wifiCheckTime = 0;
unsigned long publishCheckTime = 0;
unsigned long getInfoCheckTime = 0;
unsigned long oldSixtySec = 0;
const unsigned long PUBLISH_PERIOD_MS = 30*60*1000;
const unsigned long GET_PERIOD_MS = 5000;

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
void getData();
void updateRefillStatus(uint32_t a);
void infoHandler(const char *topic, const char *data);
void deviceNameHandler(const char *topic, const char *data);
uint32_t ping(pin_t trig_pin, pin_t echo_pin, uint32_t wait, bool info);

IntervalTimer lightsTimer;
bool sixtySecSignal = false;
bool lightsOffSignal = false;
bool requireRefill = false;

void sixtySecFunction() {
		sixtySecSignal = true;
}

void lightoffFunction() {
	lightsOffSignal = true;
}

void setup() {
	WiFi.on();
	System.disableUpdates();
 System.on(firmware_update_pending, otaHandler);

	led.begin();
 colorWipe(led.Color(255, 255, 255), 0); //on
	led.show(); // Initialize all pixels to 'off'

	Serial.begin(9600);
	pinMode(moisturePin, INPUT);
	pinMode(D7, OUTPUT);
	digitalWrite(D7, LOW);

	Particle.function("control", plantControl);
 Particle.subscribe("spark/", deviceNameHandler);
 Particle.subscribe("hook-response/get-Pdata/0", infoHandler, MY_DEVICES);


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

	//publish information to firebase
	if(millis() - publishCheckTime > PUBLISH_PERIOD_MS) {
 	uint32_t height = 8 - ping(D1, D2, 20, false);

		if(height <= 2)
			updateRefillStatus(true);
		else
			updateRefillStatus(false);

		uint32_t moisture = analogRead(moisturePin);
		publishData(height, moisture);

		publishCheckTime = millis();
	}


	if(millis() - getInfoCheckTime > GET_PERIOD_MS) {
		getData();

		getInfoCheckTime = millis();

		requireRefill = !requireRefill;
		updateRefillStatus(requireRefill);

		if(owner == "") {
					colorWipe(led.Color(0, 0, 0), 0); //off
					motor.A(0);
		}
	}

	if(millis() - oldSixtySec > 60*60*1000) {
		sixtySecFunction();
		oldSixtySec = millis();
	}

	if(sixtySecSignal) {
		if(owner == "") {
					/*colorWipe(led.Color(0, 0, 0), 0); //off*/
					motor.A(0);
		}
		else {
			double pumpDuration = constrain(waterSetting.toInt(), 0, 100);
			pumpDuration = 3.0*(pumpDuration/100.0);	//% of 3seconds which is the max pump turn on time

			motor.A(255);
			delay(pumpDuration*1000);
			motor.A(0);

			/*int lightIntensity = constrain(lightSetting.toInt(), 0, 100);
			lightIntensity = map(lightIntensity, 0, 100, 0, 255);
			colorWipe(led.Color(lightIntensity, lightIntensity, lightIntensity), 0); //on
			lightsTimer.begin(lightoffFunction, 10*1000*2, hmSec);	//30s*/
			sixtySecSignal = false;
			lightsOffSignal = false;

			/*if (DEBUG) Particle.publish("plant001/value", String(lightIntensity) +  "," + String(pumpDuration));*/
			if(DEBUG) Particle.publish("plant-watered");
		}
	}

	/*if(lightsOffSignal) {
		colorWipe(led.Color(0, 0, 0), 0); //off
		lightsTimer.end();
		lightsOffSignal = false;

		if (DEBUG) Particle.publish("timer/lightsoff");
		}*/
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
		colorWipe(led.Color(255, 255, 255), 0); //on
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
