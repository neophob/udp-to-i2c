// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

 
#include <Wire.h>
 
void setup()
{
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(115200);           // start serial for output
  Serial.println("hello!");
}

void loop()
{
  delay(1000);
  Serial.println("still alive!");
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  int a=0;
  while(Wire.available()>0) // loop through all but the last
  {
    Wire.read(); // receive byte as a character
    a++;
  }
  Serial.print("read bytes: ");
  Serial.println(a);         // print the integer
}
