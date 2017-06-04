// Servo-Timer with Arduino Pro Micro
// written by Achim Oelkers (AchimO)
// look into the forum Raketenmodellbau.org for further details
// Thanks go to Michal Rinott for his example for controlling 
// the servo position using a potentiometer

/*****************************************************************************
* Copyright (c) 2016 Hans-Joachim Oelkers
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.

* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgement in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.

Further editing done by Soeren Weber and Lennart Wilde in order to improve 
reliabilty on the Attiny85 and accuracy of code execution.

***************************************************************************/

#include <SoftwareServo.h>

/**************************************************************************/
// set these values to your personal needs, so code below not to be changed

int servoinitpos = 90;      // Position to start at
int servodeploypos = 0;     // Position to deploy to
float timeperunit = 0.1;    // time in secs for 1 unit: 1 sec: 1, 0.5 sec: 0.5o

/**************************************************************************/

SoftwareServo swservo;      // create servo object to control a servo
int potpin = A1;            // analog pin used to connect the potentiometer
int triggerpin = 3;         // going to high indicates start of rocket
int ledpin = 4;             // LED
int servopin = 1;           // servo
int val;                    // variable to read the value from the analog pin
int oldVal = 0;             // variable to store previous value from the analog pin
int timerVal = 1;           // potentiometer pin value mapped to 0 .. 63
int starttime = 0;          // indicates timer has not been started
int millistowait = 0;       // time to wait in ms
float timetowait = 0;       // time to wait in sec
int secondstowait = 0;      // seconds to wait
int tenthstowait = 0;       // tenths of a second to wait

unsigned long val_previousMillis = 0; // only update time to wait every so often...
const long val_interval = 200;        // ...200 milliseconds to be exact

/**************************************************************************/

void setup() 
{
  pinMode(potpin, INPUT);
  pinMode(triggerpin, INPUT_PULLUP);  // use the internal pullup resistor
  pinMode(ledpin, OUTPUT);            // digit weight 1

  swservo.attach(servopin);           // attaches the servo to the servo object
  swservo.write(servodeploypos);      // turn the servo to its deploy position
  servoDelay();                       // wait for the servo to move
  swservo.write(servoinitpos);        // turn the servo to its initial position
  servoDelay();                       // wait for the servo to move
  swservo.detach();                   // detaches the servo
  update_val();                       // read the desired time from potentiometer's position
  showTime();                         // read out the potentiometer position and show LED accordingly
}

void loop() 
{
  update_val();                           // read the desired time position
  if (starttime == 0)                     // has timer been started ?
  {
    if (digitalRead(triggerpin) == 0)     // not yet started, trigger pins opened ?
    {
      showTime();                         // show LED according to wanted time
    }
    else
    {
      starttime = millis();                         // just set the flag
      millistowait = timerVal * 1000 * timeperunit; // val = 1: wait 0.1 secs, val = 63: wait 6.3 secs
      delay(millistowait);

      swservo.attach(servopin);                     // attaches the servo on pin D1 to the servo object
      swservo.write(servodeploypos);                // then move the servo
      servoDelay();                                 // wait for the servo to move
      swservo.detach();                             // detaches the servo on pin D1
    }
  } else
  {
    if(digitalRead(triggerpin) == 0)
    {
      neustart();                                   // restart if jumper pins have been reconnected
    }
  }
}

void servoDelay()
{
   for (int i = 0; i < 100; i++)
   {
     delay(10);
     swservo.refresh();
   }
}

void update_val()
{
  unsigned long currentMillis = millis();

  if (currentMillis - val_previousMillis >= val_interval) 
  {
    val_previousMillis = currentMillis;
    val = analogRead(potpin);                         // reads the value of the potentiometer (value between 0 and 1023)
    timerVal = map(val, 0, 1023, 0, 63);              // scale it to 0 .. 63
    timetowait = timerVal*timeperunit;                // calculate time to wait
    secondstowait = (int) timetowait;                 // calculate seconds to wait
    tenthstowait = (timetowait - secondstowait)*10;   // calculate tenths of seconds to wait
  }
  
}

void showTime()
{
  if (timerVal != oldVal)
  {
    for(int i = 0; i<secondstowait;i++)               // blink long for the seconds
    {
      warte(300);
      blinken(400);
    }
    warte(500);                                       // wait 0.5 sec between blinking
    for(int j = 0; j<tenthstowait;j++)                // blink short for the tenths
    {
      warte(200);
      blinken(200);
    }
  }
  oldVal = timerVal;
}

void blinken(int mills)
{
  digitalWrite(ledpin, HIGH);
  warte(mills);
  digitalWrite(ledpin, LOW);
}

void warte(int zeit)                                  // bodge to work around delay and shorten uses of delay significantly
{
  for(int i = 0; i<20 ; i++)
  {
    if (digitalRead(triggerpin) == 1) {return;}
    delay(zeit/20);
  }
}

void neustart()                   // initialize all the variables to prestart conditions
{
  delay(1000);                    // wait a second before restart
  val = 0;                        // variable to read the value from the analog pin
  oldVal = 0;                     // variable to store previous value from the analog pin
  timerVal = 1;                   // potentiometer pin value mapped to 0 .. 63
  starttime = 0;                  // indicates timer has not been started
  millistowait = 0;               // time to wait in ms
  timetowait = 0;                 // time to wait in sec
  
  swservo.attach(servopin);       // attaches the servo to the servo object
  swservo.write(servodeploypos);  // turn the servo to its deploy position
  servoDelay();                   // wait for the servo to move
  swservo.write(servoinitpos);    // turn the servo to its initial position
  servoDelay();                   // wait for the servo to move
  swservo.detach();               // detaches the servo
  update_val();                   // read the desired time from potentiometer's position
  showTime();                     // read out the potentiometer position and show LED accordingly
}
