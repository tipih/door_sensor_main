# door_sensor_main
Reed and temperatur sensor for the main door
Sensor is based for the arduino pro mini and combination with the HC12 Radio
Radio is powered via a BS170 mosfet, to save power, BS170 is connected to pin 6

Reed sensor is connected to pin2 to be used to for external interrupt
Sleep is set to external interrupt and 20000mSec delay.
Sending is set to 20 wakeup so if no external interrupt is detected sendig sensor input is done
20*20000mSec = 400 sec = approx 6 min 40 sec
