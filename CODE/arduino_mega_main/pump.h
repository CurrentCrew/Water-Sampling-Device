#ifndef PUMP_H
#define PUMP_H

class Pump {
    public:
        int ena_pin;

        // Constructors
        Pump(const int ena_pin) {
            this->ena_pin = ena_pin;
        }
        // Initialization
        void init() {
            pinMode(this->ena_pin, OUTPUT);
            digitalWrite(this->ena_pin, LOW);

            #ifdef PUMP_SERIAL
            PUMP_SERIAL.println("Initalized Pump");
            #endif
        }
        void start() {
            digitalWrite(ena_pin, HIGH);
        }
        void stop() {
            digitalWrite(ena_pin, LOW);
        }
};

#endif