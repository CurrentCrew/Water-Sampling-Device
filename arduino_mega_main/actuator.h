#ifndef ACTUATOR_H
#define ACTUATOR_H

class Actuator {
    public:
        const int act_pin1;
        const int act_pin2;

        // Constructors
        Actuator(const int act_pin1, const int act_pin2) {
            this->act_pin1 = act_pin1;
            this->act_pin2 = act_pin2
        }
        // Initialization
        void init() {
            pinMode(this->act_pin1, OUTPUT);
            pinMode(this->act_pin2, OUTPUT);

            #ifdef ACTUATOR_SERIAL
            ACTUATOR_SERIAL.println("Initalized Actuator");
            #endif
        }
        void extendActuator(const int act_pin1, const int act_pin2) {
            digitalWrite(act_pin1, HIGH);
            digitalWrite(act_pin2, LOW);
        }
        void retractActuator(const int act_pin1, const int act_pin2) {
            digitalWrite(act_pin1, LOW);
            digitalWrite(act_pin2, HIGH);
        }
        
};

#endif