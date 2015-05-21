// polaris main routine - - - - - - - - - - - - - - - -
// Milos Micic 2014 micicmilos@gmail.com
//
//                 '
//           *          .
//                   *       '
//             *                *
//
//
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Code is for Arduino MEGA/MEGA SDK/ MEGA r3 it can be used on other Arduino platforms with pin revision
//  Drive motor is based on wiper motor (ford escort focus .... any 12V motor ) with IBT2 H - bridge servo controler (bts7960 based).
//  Servo for wheel control is also wiper motor with IBT2 H bridge and custom added pot for position ( it can be some hobby servo with high tourqe but this is too expensive).
//  For wheel i'm using some logitec with flapy pedals shifters for gear change reverse+neutral+forward+forward speed.
//  Gas pedal is also logitec

#include <TimedAction.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define CE_PIN   8
#define CSN_PIN 53

const uint64_t pipe = 0xE8E8F0F0E1LL;
RF24 radio(CE_PIN, CSN_PIN);
int joystick[3];
const int gasPin = 0;                                                // pin for gas potentiometer.
const int wheelPin = 1;                                              // pin for wheel potentimoeter.
const int servoPot = 2;                                              // pin for servo position pot.
const int motorLPWM = 4;                                             // pin for left pwm motor rotation.
const int motorRPWM = 5;                                             // pin for right pwm motor rotation.
const int servoLPWM = 2;
const int servoRPWM = 3;
const int PWMenable = 6;                                             // pin for enabling left rotation.

int echoPin = 22;                                                    // echo pin for HC-SR04 ultrasonic range sensor
int trigPin = 23;                                                    // trigger pin for HC-SR04 ultrasonic range sensor
int currentGear = 0;
int smalldelta = 55;                                                 // error customization for wheel servo
int wheelValue;
int servoValue;
int leftOffset = 150;
int rightOffset = 900;
int currentSpeed;
int gearUpPin = 32;
int gearUpPinCounter = 0;
int gearUpPinState = 0;
boolean stopit = false;                                               // stop or no
boolean remotecontrol = false;
TimedAction wheels = TimedAction(50, adjustWheels);
TimedAction gas = TimedAction(50, adjustGas);
TimedAction gear = TimedAction(50, checkGear);
TimedAction range = TimedAction(1000, checkStop);
TimedAction remote = TimedAction(20, checkJoystick);




// - - - - - - - - - - - -

void setup() {
  pinMode(gasPin, INPUT);
  pinMode(wheelPin, INPUT);
  pinMode(servoPot, INPUT);
  pinMode(motorLPWM, OUTPUT);
  pinMode(motorRPWM, OUTPUT);
  pinMode(servoLPWM, OUTPUT);
  pinMode(servoLPWM, OUTPUT);
  pinMode(PWMenable, OUTPUT);
  pinMode(gearUpPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(PWMenable, HIGH);
  Serial1.begin(9600);                          // only for debug purpose
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
  radio.printDetails();

}

void loop() {
  gas.check();
  wheels.check();
  gear.check();
  range.check();
  remote.check();

}


void adjustWheels() {
  wheelValue = analogRead(wheelPin);
  servoValue = analogRead(servoPot);


  if (servoValue > wheelValue && servoValue > leftOffset && abs(wheelValue - servoValue) > smalldelta && servoValue < rightOffset) {
    analogWrite(servoLPWM, 250);
  } else {
    analogWrite(servoLPWM, 0);
  }
  if (servoValue < wheelValue  && servoValue < rightOffset && abs(wheelValue - servoValue) > smalldelta && servoValue > leftOffset) {
    analogWrite(servoRPWM, 250);
  } else {
    analogWrite(servoRPWM, 0);
  }

}

void checkStop() {

}

void adjustGas() {
  if (!remotecontrol) {
    currentSpeed = analogRead(gasPin);
    if (currentSpeed > 350 ) {

      currentSpeed = (currentSpeed - 300) / 2;
      analogWrite(motorLPWM, currentSpeed);

    }
    else {
      analogWrite(motorLPWM, 0);
    }

  }
  if (remotecontrol) {
    currentSpeed = joystick[0];
    if (currentSpeed < 480) {
      currentSpeed = (500 - currentSpeed) / 2;
      analogWrite(motorLPWM, currentSpeed);
      analogWrite(motorRPWM, 0);
      Serial1.println("idem napred");
    } else if (currentSpeed > 520 ) {
      currentSpeed = (currentSpeed - 520) / 2;
      analogWrite(motorRPWM, currentSpeed);
      analogWrite(motorLPWM, 0);
      Serial1.println("idem nazad");
      Serial1.println(currentSpeed);
    } else if (currentSpeed > 480 && currentSpeed < 520) {
      analogWrite(motorRPWM, 0);
      analogWrite(motorLPWM, 0);
      Serial1.println("stojim");

    }
  }
}

void checkGear() {


}
void checkJoystick() {
  if ( radio.available() ) {
    remotecontrol = true;
    bool done = false;
    while (!done)
    {

      done = radio.read(joystick, 3);

    }
    Serial1.println(joystick[0]);
    Serial1.println(joystick[1]);

  } else {
    //  remotecontrol = false;
    //  Serial1.println("remoteoff");
  }
}

