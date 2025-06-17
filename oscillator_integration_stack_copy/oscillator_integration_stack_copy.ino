#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

int needleActuatorpin1 = 26;
int needleActuatorpin2 = 27;
int lockingActuatorpin1 = 24;
int lockingActuatorpin2 = 25;

const int wheelStepPin = 42;
const int wheelDirPin = 41;
const int wheelEnPin = 40;

const int pumpEnPin = 30;

//water sensor
const int sensorEnablePin = 36;
const int sensorPin = 3;

//DS3231 Clock IC output
const int alarmPin = 18;

//CHANGE THIS TO ADJUST HOW FAR THE STEPPER MOTOR ROTATES
const int stepPerFullRev = 3200;
//const int stepPerFullRev = 6400;
//const int stepPerFullRev = 12800;

//deprecated, use as fallback if timing IC doesn't work. multiply by 1000 if using delay() 
//  since delay uses ms: delay(secsBetweenSamples * 1000);
const int secsBetweenSamples = 3;

int sampleCounter = 1;

const int fillTubeSteps = 200;
const int flushSteps = 200;


// hardware interrupt flags
volatile bool waterFlowing = false;
volatile bool alarm = false;

DateTime now;
DateTime future;

int currentPosition;

void takeSample() {

  // stepWheel(currentPosition, 1);

  //rotates to correct position
  for(int i = 0; i < sampleCounter; i++) {
      stepWheel(stepPerFullRev/32, 1);
      delay(500);
    }

  lockTube();
  insertNeedle();
  fillTube();
  release();
}

//releases all linear actuators
void release() {
  digitalWrite(wheelEnPin, LOW);
  
  Serial.println("release 1");
  digitalWrite(needleActuatorpin1, HIGH);
  digitalWrite(needleActuatorpin2, LOW);

  delay(10000);

  Serial.println("release 2");
  digitalWrite(lockingActuatorpin1, LOW);
  digitalWrite(lockingActuatorpin2, HIGH);

  delay(3000);
  waterFlowing = false;
}

//handles conversion between sample number and step count
void stepWheel(int n, int direction) {
  //digitalWrite(wheelEnPin, HIGH);

  digitalWrite(wheelEnPin, LOW); 
  delayMicroseconds(100);
  if (direction == 1) {
    digitalWrite(wheelDirPin, HIGH);
  } else {
    digitalWrite(wheelDirPin, LOW);
  }
  
  for (int x = 0; x < n; x++) {
    digitalWrite(wheelStepPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(wheelStepPin, LOW);
    delayMicroseconds(2000);
  }
  digitalWrite(wheelDirPin, LOW);
}

//engages horizontal LA
void lockTube() {
  digitalWrite(wheelEnPin, HIGH);
  
  digitalWrite(lockingActuatorpin1, HIGH);
  digitalWrite(lockingActuatorpin2, LOW);
  delay(3000);
}

//engages vertical LA
void insertNeedle() {
  digitalWrite(needleActuatorpin1, LOW);
  digitalWrite(needleActuatorpin2, HIGH);

  delay(12000);

}

//runs pump for set amount of time
void purge() {
  //digitalWrite(pumpEnPin, HIGH);
  Serial.println("begin pump");
  digitalWrite(pumpEnPin, HIGH);
  delay(5000);
  Serial.println("end pump");
  digitalWrite(pumpEnPin, LOW);
  waterFlowing = false;
}

//runs pump until water sensor hardware interrupt
void fillTube() {
  //digitalWrite(pumpEnPin, HIGH);
  Serial.println("begin pump");
  while (!waterFlowing) {
    digitalWrite(pumpEnPin, HIGH);
  }
  Serial.println("end pump");
  digitalWrite(pumpEnPin, LOW);
  waterFlowing = false;
}

void setup() {
  Serial.begin(9600);

  //CLOCK SETUP CODE
  // Initialize the RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC!");
    while (1) delay(10);
  }
  // If power was lost, set time to compile time
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  // Disable the 32 kHz output to free the SQW pin
  rtc.disable32K();
  // Clear any existing alarms
  //rtc.disableAlarm(1); 
  rtc.disableAlarm(2);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  // Turn off the square‑wave generator so alarms drive SQW
  rtc.writeSqwPinMode(DS3231_OFF);
  // Set up the interrupt pin (SQW → Arduino D18)
  pinMode(alarmPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(alarmPin), onAlarm, FALLING);

  // Set alarm 1 to trigger every day at midnight (00:00:00)
  DateTime midnight = DateTime(now.year(), now.month(), now.day(), 0, 0, 0);
  // If it's past midnight now, set the alarm for the next day
  if (now >= midnight) {
    midnight = midnight + TimeSpan(1, 0, 0, 0); // add 1 day
  }
  // Set Alarm 1 to trigger when hour, minute, and second match (daily)
  rtc.setAlarm1(midnight, DS3231_A1_PerDay);
  // Enable Alarm 1 and clear any old alarm flags
  rtc.armAlarm(1, true);
  rtc.clearAlarm(1);


  //Set up water sensor
  digitalWrite(sensorEnablePin, HIGH);

  // Set up linearActuators
  pinMode(needleActuatorpin1, OUTPUT);
  pinMode(needleActuatorpin2, OUTPUT);
  pinMode(lockingActuatorpin1,  OUTPUT);
  pinMode(lockingActuatorpin2, OUTPUT);

  // Set up wheel stepper motor
  pinMode(wheelStepPin, OUTPUT);
  pinMode(wheelDirPin, OUTPUT);
  pinMode(wheelEnPin, OUTPUT);
  digitalWrite(wheelEnPin, LOW);

  // Set up pump
  pinMode(pumpEnPin, OUTPUT);
  digitalWrite(pumpEnPin, LOW);

  // lockTube();
  // insertNeedle();
  // purge();

  release();
  lockTube();

  // Serial.println("move stepper");
  // stepWheel(stepPerFullRev/32, 1);
  currentPosition = 0;
  sampleCounter = 1;
  // currentPosition = stepPerFullRev/32;
  //lockTube();

  attachInterrupt(digitalPinToInterrupt(sensorPin),stopPump,RISING);
  // pinMode(sensorPin, INPUT);

}

void loop() {
  if(alarm) {
    //turns off alarm
    rtc.clearAlarm(1);
    rtc.disableAlarm(1);

      // //selects next alarm: see RTClib for other approaches
      // future = rtc.now() + TimeSpan(120);
      // if (!rtc.setAlarm1(future, DS3231_A1_Minute)) {
      //   Serial.println("Failed to set Alarm1");
      // } 
    
    //runs until 31 samples have been taken
    if(sampleCounter < 32) {
      insertNeedle();
      purge();
      release();
      takeSample();

      //PURGE RETURN: REPLACE THIS BLOCK WITH LIMIT SWITCH LOGIC
      for(int i = 0; i < sampleCounter; i++) {
        stepWheel(stepPerFullRev/32, 0);
        delay(500);
      }

      //engage horizontal LA for overnight
      lockTube();
      sampleCounter ++;
    }

    alarm = false;
  }
  
  
  // for(sampleCounter = 1; sampleCounter < 32; sampleCounter++) {
  //   insertNeedle();
  //   purge();
  //   release();
  //   //currentPosition += stepPerFullRev/32;
  //   takeSample();
  //   for(int i = 0; i < sampleCounter; i++) {
  //     stepWheel(stepPerFullRev/32, 0);
  //     delay(500);
  //   }
  //   lockTube();
  //   delay(secsBetweenSamples*1000);
  // }
}


//hardware interrupt methods
void stopPump() {
  waterFlowing = true;
}

void onAlarm() {
  // Clear the alarm flag to allow the next day’s event
  alarm = true;
  //Serial.println("Daily alarm triggered!");
  // TODO: insert your 24 hr event handling code here
}
