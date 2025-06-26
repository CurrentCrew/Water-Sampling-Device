#ifndef SENSOR_H
#define SENSOR_H

class Sensor {
    public:
        int ena_pin; 
        int out_pin;

        // Constructors
        Sensor(const int ena_pin, const int out_pin) {
            this->ena_pin = ena_pin;
            this->out_pin = out_pin;
        }
        // Initialization
        void init() {
            digitalWrite(ena_pin, HIGH);
            pinMode(out_pin, INPUT);

            #ifdef ALARM_SERIAL
            ALARM_SERIAL.println("Initalized Sensor");
            #endif
        }
};

#endif