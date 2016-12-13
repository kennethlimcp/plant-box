### Introduction

#### Features

- safe OTA firmware update - turns off all peripherals
- water tank height monitoring
- moisture level sensing
- RGB lightning

#### Components

1. [Particle Photon](https;//particle.io)
2. [Motor shield by Hootie81](https://github.com/Hootie81/Dual-DC-Motor)
3. [Power shield by @Hootie81](https://github.com/Hootie81/Power-Shield)
4. [Moisture sensor](https://www.dfrobot.com/index.php?route=product/product&filter_name=SEN0114&product_id=599)
5. [Diaphragm Liquid Pump](http://www.sgbotic.com/index.php?dispatch=products.view&product_id=2119)
6. [Neopixel ring](https://www.adafruit.com/products/1643)

#### Pinout


| Peripheral|Pin| Photon USB|Pin|Peripheral|
| :--------:|:-:|:---------:|:--:|:-------:|
||Vin||3V3||
||GND||RST||
||TX||VBAT||
||RX||GND||
||WKP||D7||
||DAC||D6||
|SPI MOSI|A5||D5||
|SPI MISO|A4||D4||
|SPI SCK|A3||D3||
|SPI SS|A2||D2|ECHO|
|Moisture sensor|A1||D1|TRIG|
|Motor SS|A0||D0|Neopixel|

#### Resources

1. [Firebase <-> Particle Cloud tutorial](https://github.com/rickkas7/firebase_tutorial)
2. [pulseIn for ultrasonic sensor](https://gist.github.com/technobly/349a916fb2cdeb372b5e)
3. [Spark Interval Timer](https://github.com/pkourany/SparkIntervalTimer)
