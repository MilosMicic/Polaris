#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <TimedAction.h>

#define CE_PIN   8
#define CSN_PIN 10
#define JOYSTICK_X A0
#define JOYSTICK_Y A1


const uint64_t pipe = 0xE8E8F0F0E1LL;

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

int joystick[3];  // 3 element array holding Joystick readings
boolean a = true;
TimedAction sData = TimedAction (20, sendData);
TimedAction bBlink = TimedAction(500, blueblink);
void setup()
{
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  radio.printDetails();
}

void loop()
{
  digitalWrite(3, HIGH);
  sData.check();
  bBlink.check();
}
void sendData() {
  joystick[0] = analogRead(JOYSTICK_X);
  joystick[1] = analogRead(JOYSTICK_Y);
  joystick[2] = 100;
  radio.write( joystick, sizeof(joystick) );
  Serial.println(analogRead(JOYSTICK_X));
  Serial.println(analogRead(JOYSTICK_Y));

}
void blueblink() {
  if (a) {
    a = false;
    digitalWrite(3, LOW);
  } else {
    a = true;
    digitalWrite(3, HIGH);
  }

}
