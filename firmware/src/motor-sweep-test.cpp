#include <SPI_Motor.h>

SYSTEM_MODE(MANUAL);

SPI_Motor singleCard(A0);	//Hootie81's motor shield using A0 CS pin

void setup() {
	Serial.begin(9600);
	while (!Serial.available()) Particle.process();

	if (singleCard.begin()){
		Serial.println("Single card Initialised!");
	}
	else
		Serial.println("Single card not found :(");

	singleCard.resume();
	singleCard.A(STOP, 0);
	singleCard.A(CW);
}

void loop() {
	int i;

	singleCard.A(STOP, 0);

	Serial.println("Ramping up all motors now");
	for(i=0; i<255; i++){  //create a loop with a increasing number
		singleCard.A(i);
		Serial.println("Ramp up speed: " + String(i));
		delay(50); //ramp from 0 to 255 should take about 30sec (maybe as a guess!!)
	}

	Serial.println("Ramping Down all motors now");
	for(i=255; i>0; i--){
		singleCard.A(i);
		Serial.println("Ramp down speed: " + String(i));
		delay(50); //ramp from 0 to 255 should take about 30sec (maybe as a guess!!)
	}

	//singleCard.standby();
	//singleCard.resume();
}
