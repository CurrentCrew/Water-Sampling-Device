#ifndef PUMP_H
#define PUMP_H

class Pump {
    public:
        const int ena_pin;

        // Constructors
        Pump(const int ena_pin) {
            this->ena_pin = ena_pin;
        }
        // Initialization
        void init() {
            pinMode(pumpEnPin, OUTPUT);
            digitalWrite(pumpEnPin, LOW);

            #ifdef PUMP_SERIAL
            PUMP_SERIAL.println("Initalized Pump");
            #endif
        }
        void startPump(const int ena_pin) {
            digitalWrite(ena_pin, HIGH);
        }
        void stopPump(const int ena_pin) {
            digitalWrite(ena_pin, LOW);
        }
};

#endif