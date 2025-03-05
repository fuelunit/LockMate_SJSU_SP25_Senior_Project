/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep

 * Author: Yipeng Liu
 * Date: 03/04/2025
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
const int servo_pin = 5;// servo controll pin should be connected to pin #5 on an arduino mega board
int pos = 0;    // variable to store the servo position
int pos_lock = 150; // variable to store the locked servo position
int pos_unlock = 70; // variable to store the unlocked servo position

void setup() {
  myservo.attach(servo_pin);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  unlock();
  lock();
}

void unlock() {
  for (pos = pos_lock; pos >= pos_unlock; pos -= 1) { // goes from 150 degrees to 60 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
}

void lock() {
  for (pos = pos_unlock; pos <= pos_lock; pos += 1) { // goes from 60 degrees to 150 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
}