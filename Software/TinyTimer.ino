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
*****************************************************************************/

#include <SoftwareServo.h>

/**************************************************************************/
// set these values to your personal needs, so code below not to be changed
//int servoinitpos = 130;  //original setting (Magdeburg Pilot Rocket Version 1)
int servoinitpos = 90;     // setting for Magdeburg Pilot Rocket Version 2   
int servodeploypos = 0;
float timeperunit = 0.5;  // time in secs for 1 unit: 1 sec: 1, 0.5 sec: 0.5
/**************************************************************************/

SoftwareServo swservo;     // create servo object to control a servo
bool servomoved = false;
int potpin = A1;           // analog pin used to connect the potentiometer
int triggerpin = 3;        // going to high indicates start of rocket
int ledpin = 4;            // LED
int servopin = 1;          // servo 
int val;                   // variable to read the value from the analog pin
int oldVal = 0;
int timerVal;              // potentiometer pin value mapped to 0 .. 15
int starttime = 0;         // indicates timer has not been started
int millistowait = 0;      // time to wait in ms


void setup()
{
  pinMode(potpin, INPUT);
  pinMode(triggerpin, INPUT_PULLUP);  // use the internal pullup resistor 
  pinMode(ledpin, OUTPUT);  // digit weight 1
  
  swservo.attach(servopin);       // attaches the servo to the servo object
  swservo.write(servodeploypos);  // turn the servo to its deploy position
  servoDelay();                   // wait for the servo to move
  swservo.write(servoinitpos);    // turn the servo to its initial position
  servoDelay();                   // wait for the servo to move    
  swservo.detach();               // detaches the servo 
  showTime();                     // read out the potentiometer position and show LED accordingly
}

void loop()
{
  if (starttime == 0)                    // has timer been started ?       
  {
    if (digitalRead(triggerpin) == 0)    // not yet started, trigger pins opened ?
    {
      showTime();                        // read out the potentiometer position and show LED accordingly
    }
    else
    { 
      starttime = millis();             // just set the flag
      millistowait = timerVal*1000*timeperunit;  // val = 1: wait 0.5 secs, val = 15: wait 7.5 secs !!! 
      delay(millistowait);  

      swservo.attach(servopin);         // attaches the servo on pin D1 to the servo object
      swservo.write(servodeploypos);    // then move the servo 
      servoDelay();
      swservo.detach();                 // detaches the servo on pin D1
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

void showTime()
{
  val = analogRead(potpin);   // reads the value of the
                              // potentiometer (value between
                              // 0 and 1023)
  if (val != oldVal)
  {        
    delay(200);               // wait some time for potentiometer move
                              // then read servo pos again
    val = analogRead(potpin); // reads the value of the
                              // potentiometer (value between
                              // 0 and 1023)
         
    timerVal = map(val, 0, 1023, 0, 15);  // scale it to 0 .. 15

    for (int i = 0; i < timerVal; i++)
    {
      shortBlink();
      delay(200);
    }
    delay(1000);
  }
  oldVal = val;
}

void shortBlink()
{
  digitalWrite(ledpin, HIGH);
  delay(200);
  digitalWrite(ledpin, LOW);
}



