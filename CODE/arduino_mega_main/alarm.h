#ifndef ALARM_H
#define ALARM_H

#include <Wire.h>
#include <RTClib.h>

class Alarm {
    public:
        RTC_DS3231 rtc;
        DateTime now;
        DateTime future;
        int alarmPin; // interrupt pin: DS3231 output

        // Constructors
        Alarm(const int alarmPin) {
            this->alarmPin = alarmPin;
        }
        // Initialization: sets pin modes and enables rotary
        void init() {
            // Set up the interrupt pin (SQW → Arduino D18)
            pinMode(this->alarmPin, INPUT_PULLUP);
            
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
            rtc.disableAlarm(2);
            rtc.clearAlarm(1);
            rtc.clearAlarm(2);
            // Turn off the square‑wave generator so alarms drive SQW
            rtc.writeSqwPinMode(DS3231_OFF);

            #define TEST_MODE
            #ifdef TEST_MODE
            DateTime alarmTime(0, 0, 0, 0, 0, 45);
            rtc.setAlarm1(alarmTime, DS3231_A1_Second);
            #else

            // Set alarm 1 to trigger every day at midnight (00:00:00)
            DateTime midnight = DateTime(now.year(), now.month(), now.day(), 0, 0, 0);
            // If it's past midnight now, set the alarm for the next day
            if (now >= midnight) {
                midnight = midnight + TimeSpan(1, 0, 0, 0); // add 1 day
            }
            // Set Alarm 1 to trigger when hour, minute, and second match (daily)
            rtc.setAlarm1(midnight, DS3231_A1_PerDay);

            #endif

            rtc.clearAlarm(1);

            #ifdef ALARM_SERIAL
            ALARM_SERIAL.println("Initalized Alarm");
            #endif
        }
      
      void stop()
      {
        rtc.clearAlarm(1);
      }
};

#endif