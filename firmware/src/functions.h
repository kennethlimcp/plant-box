/*
Taken from: https://gist.github.com/technobly/349a916fb2cdeb372b5e
Modified by: kennethlimcp
Created on: 22 Nov 2016
*/

#include "application.h"

void ping(pin_t trig_pin, pin_t echo_pin, uint32_t wait, bool info)
{
	uint32_t duration, inches, cm;
	static bool init = false;
	if (!init) {
		pinMode(trig_pin, OUTPUT);
		digitalWriteFast(trig_pin, LOW);
		pinMode(echo_pin, INPUT);
		delay(50);
		init = true;
	}

	/* Trigger the sensor by sending a HIGH pulse of 10 or more microseconds */
	digitalWriteFast(trig_pin, HIGH);
	delayMicroseconds(10);
	digitalWriteFast(trig_pin, LOW);

	duration = pulseIn(echo_pin, HIGH);

	/* Convert the time into a distance */
	// Sound travels at 1130 ft/s (73.746 us/inch)
	// or 340 m/s (29 us/cm), out and back so divide by 2
	// Ref: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
	inches = duration / 74 / 2;
	cm = duration / 29 / 2;

	if (info) { /* Visual Output */
		Particle.publish("plant/tank/height", String(cm));
		//Serial.printf("%2d:", inches);
		//for(int x=0;x<inches;x++) Serial.print("#");
		//Serial.println();
	} //else { /* Informational Output */
		//Serial.printlnf("%6d in / %6d cm / %6d us", inches, cm, duration);
	//}
	delay(wait); // slow down the output
}
