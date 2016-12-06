/*
Taken from: https://gist.github.com/technobly/349a916fb2cdeb372b5e
Modified by: kennethlimcp
Created on: 22 Nov 2016
*/

#include "application.h"
#include "SparkJson.h"
String deviceName = "";
String owner = "";
String lightSetting = "";
String waterSetting = "";

const char *PUBLISH_EVENT_NAME = "plantData";

uint32_t ping(pin_t trig_pin, pin_t echo_pin, uint32_t wait, bool info)
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

		return cm;
}

void publishData(uint32_t a, uint32_t b) {
	if(deviceName == "") {
		Particle.publish("spark/device/name");
	}
	else {
		char buf[256];
		snprintf(buf, sizeof(buf), "{\"a\":%lu,\"b\":%lu,\"n\":\"%s\"}", a, b, deviceName.c_str());
		Serial.printlnf("publishing %s", buf);
		Particle.publish(PUBLISH_EVENT_NAME, buf, PRIVATE);
	}
}

void getData(void) {
	if(deviceName == "") {
		Particle.publish("spark/device/name");
	}
	else {
		char buf1[20];
		snprintf(buf1, sizeof(buf1), "{\"n\":\"%s\"}", deviceName.c_str());
		Particle.publish("get-Pdata", buf1, PRIVATE);
	}
}

void deviceNameHandler(const char *topic, const char *data) {
	deviceName = data;
}

void infoHandler(const char *topic, const char *data) {
	// This isn't particularly efficient; there are too many copies of the data floating
	// around here, but the data is not very large and it's not kept around long so it
	// should be fine.

StaticJsonBuffer<256> jsonBuffer;
char *mutableCopy = strdup(data);
JsonObject& root = jsonBuffer.parseObject(mutableCopy);
free(mutableCopy);

	const char* o = root["owner"];
	const char* l = root["required_light"];
	const char* w = root["required_water"];

	owner = String(o);
	lightSetting = String(l);
	waterSetting = String(w);

//	Serial.printlnf("a=%s b=%s c=%s", o, l, w);
}
