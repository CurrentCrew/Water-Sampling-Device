#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

const int alarmPin = 2;  // RTC SQW
volatile bool alarmTriggered = false;

void onAlarm() {
  alarmTriggered = true;
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power; setting time to compile time.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  rtc.disable32K();

  // Clear any existing alarms
  rtc.clearAlarm(1);
  rtc.disableAlarm(1);
  rtc.clearAlarm(2);
  rtc.disableAlarm(2);

  pinMode(alarmPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(alarmPin), onAlarm, FALLING);

  DateTime now = rtc.now();

  // Set Alarm 1 to trigger every minute when seconds == 0
  rtc.setAlarm1(DateTime(0, 0, 0, 0, 0, 0), DS3231_A1_Second); 
  // Alarm triggers when seconds match

  Serial.println("Alarm set to trigger every minute at seconds == 0");
}

void loop() {
  if (alarmTriggered) {
    Serial.println("Alarm triggered!");

    // Clear alarm flag so it can trigger again
    rtc.clearAlarm(1);

    alarmTriggered = false;
  }
}
