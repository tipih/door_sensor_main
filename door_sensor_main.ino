/*
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2018 Sensnology AB
 * Full contributor list: https://github.com/mysensors/MySensors/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * DESCRIPTION
 *
 * Interrupt driven binary switch example with dual interrupts
 * Author: Patrick 'Anticimex' Fallberg
 * Connect one button or door/window reed switch between
 * digital I/O pin 3 (BUTTON_PIN below) and GND and the other
 * one in similar fashion on digital I/O pin 2.
 * This example is designed to fit Arduino Nano/Pro Mini
 *
 */


// Enable debug prints to serial monitor
#define MY_DEBUG
#define MY_BAUD_RATE 9600
// Enable and select radio type attached
#define MY_NODE_ID 11
#define MY_RADIO_HC12
#define MY_HC12_SET_PIN 6
#define NUMBER_OF_WAKEUP 20
#define HC12_SLEEP_ENABLED

//#define MY_RADIO_RF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

#include <MySensors.h>
#include <DHT.h>

#define SKETCH_NAME "Main door"
#define SKETCH_MAJOR_VER "0"
#define SKETCH_MINOR_VER "1"


#define PRIMARY_CHILD_ID 20
#define SECONDARY_CHILD_ID 21
#define TEMPERATURE_CHILD_ID 22
#define HUMIDITY_CHILD_ID 23

#define PRIMARY_BUTTON_PIN 2   // Arduino Digital I/O pin for button/reed switch
#define SECONDARY_BUTTON_PIN 3 // Arduino Digital I/O pin for button/reed switch

#if (PRIMARY_BUTTON_PIN < 2 || PRIMARY_BUTTON_PIN > 3)
#error PRIMARY_BUTTON_PIN must be either 2 or 3 for interrupts to work
#endif
#if (SECONDARY_BUTTON_PIN < 2 || SECONDARY_BUTTON_PIN > 3)
#error SECONDARY_BUTTON_PIN must be either 2 or 3 for interrupts to work
#endif
#if (PRIMARY_BUTTON_PIN == SECONDARY_BUTTON_PIN)
#error PRIMARY_BUTTON_PIN and BUTTON_PIN2 cannot be the same
#endif
#if (PRIMARY_CHILD_ID == SECONDARY_CHILD_ID)
#error PRIMARY_CHILD_ID and SECONDARY_CHILD_ID cannot be the same
#endif

int oldBatteryPcnt = 0;
int BATTERY_SENSE_PIN = A0;  // select the input pin for the battery sense point
// Change to V_LIGHT if you use S_LIGHT in presentation below
MyMessage msg(PRIMARY_CHILD_ID, V_TRIPPED);
//MyMessage msg2(SECONDARY_CHILD_ID, V_TRIPPED);
MyMessage msgHum(HUMIDITY_CHILD_ID, V_HUM);
MyMessage msgTemp(TEMPERATURE_CHILD_ID, V_TEMP);
DHT dht(4, DHT11);
  static uint8_t sentValue=2;
  static uint8_t sentValue2=2;

void setup()
{
  // Setup the buttons
  pinMode(PRIMARY_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SECONDARY_BUTTON_PIN, INPUT_PULLUP);
  //pinMode(5,OUTPUT);
  //pinMode(4,OUTPUT);
  analogReference(INTERNAL);
  dht.begin();
  wait (500);

}

void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);

  // Register binary input sensor to sensor_node (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage.
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
  present(PRIMARY_CHILD_ID, S_DOOR);
 // present(SECONDARY_CHILD_ID, S_DOOR);
  present(HUMIDITY_CHILD_ID, S_HUM);
  present(TEMPERATURE_CHILD_ID, S_TEMP);
}

// Loop will iterate on changes on the BUTTON_PINs
void loop()
{
  uint8_t value,value1;

  static uint8_t timeout=0;
  float batteryV =0;
  static int batteryPcnt =0;

  static float temperature=0;
  static float h=0;
  static int num_of_valid_reading=0;
  // Short delay to allow buttons to properly settle
  delay(10);

  //Read the button (Reed sensors)
  value = digitalRead(PRIMARY_BUTTON_PIN);
  value1 = digitalRead(SECONDARY_BUTTON_PIN);
  wait(100);


 //Read the dht sensor
 float temperature_temp = dht.readTemperature();
 float h_temp = dht.readHumidity();
  if (isnan(temperature) || isnan(h)) 
  {
    Serial.println("Failed reading temperature from DHT!");
  } else{
    temperature=temperature + temperature_temp;
    h = h+h_temp;
    num_of_valid_reading++;
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Humidity: ");
    Serial.print(h);

  }


//Read the resistors 20 times
int sensorValue=0;
for (int a=0;a<20;a++){

sensorValue=sensorValue+analogRead(BATTERY_SENSE_PIN);
wait(10);
}
sensorValue=sensorValue/20;
// float batteryV    = sensorValue * 0.011621;
batteryV  =  (sensorValue * 0.00607);
    Serial.print("Battery Voltage: ");
    Serial.print(batteryV);
    Serial.println(" V");
    batteryPcnt = batteryPcnt + (sensorValue /6.9);
    Serial.print("Sensor Value: ");
    Serial.println(sensorValue);
    Serial.print("Battery percent: ");
    Serial.print(batteryPcnt);
    Serial.println(" %");

 wait(10); 
if (value!=sentValue)
{
Serial.println("Sending 1");
send(msg.set(value));
sentValue=value;  
}



wait(100);
if (timeout==NUMBER_OF_WAKEUP)
{
  Serial.println("Sending 3");
  
  delay(5);
  send(msg.set(value));
  delay(5);
   batteryPcnt = batteryPcnt /NUMBER_OF_WAKEUP+1;
  Serial.print("Battery :");
  Serial.println(batteryPcnt);
  if (batteryPcnt>100) batteryPcnt=100;
  sendBatteryLevel(batteryPcnt);
  batteryPcnt=0;
  sentValue=value;
  sentValue2=value1;   
  
}else if (timeout==NUMBER_OF_WAKEUP+1){
  timeout=0;
  h=h / num_of_valid_reading;
  temperature = temperature /num_of_valid_reading;
  
  //Send the humidity
  send(msgHum.set(h, 1));
  
  delay(5);
  //Send the temperature
  send(msgTemp.set(temperature, 1));
  
  delay(50);
  //Update the reed sensors values
  send(msg.set(value));
  h=0;
  temperature=0;
  num_of_valid_reading=0;
}

 timeout++;
  wait(10);
  // Sleep until something happens with the sensor
  //sleep(10000);
  //sleep(PRIMARY_BUTTON_PIN-2, CHANGE, SECONDARY_BUTTON_PIN-2, CHANGE,9000);
  digitalWrite(5,LOW);
  sleep(PRIMARY_BUTTON_PIN-2, CHANGE,20000);
  digitalWrite(5,HIGH);

}
