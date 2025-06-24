#ifndef SWITCH_H
#define SWITCH_H

class Switch { //Active 
    public:
        const int no_pin;
        int switchState;

        // Constructors
        Switch(const int no_pin) {
            this->no_pin = no_pin;
        }
        // Initialization
        void init() {
            pinMode(this->no_pin, INPUT_PULLUP);
            
            #ifdef SWITCH_SERIAL
            SWITCH_SERIAL.println("Initalized Switch");
            #endif
        }
        bool isSwitchPressed(const int no_pin) {
            switchState = digitalRead(no_pin) == LOW;
            return switchState == LOW; // active LOw
        }
        
        
};

#endif