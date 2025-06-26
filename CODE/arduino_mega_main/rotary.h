#ifndef ROTARY_H
#define ROTARY_H

class Rotary {
  public:
    int step_position;
    int stp_pin;
    int dir_pin;
    int ena_pin;

    // Constructors
    Rotary(const int stp_pin, const int dir_pin, const int ena_pin) {
        this->stp_pin = stp_pin;
        this->dir_pin = dir_pin;
        this->ena_pin = ena_pin;

        this->step_position = 0;
    }
    // Initialization: sets pin modes and enables rotary
    void init() {
        pinMode(this->stp_pin, OUTPUT);
        pinMode(this->dir_pin, OUTPUT);
        pinMode(this->ena_pin, OUTPUT);
        digitalWrite(this->ena_pin, LOW);

        #ifdef ROTARY_SERIAL
        ROTARY_SERIAL.println("Initalized Rotary");
        #endif
    }

    void on() {
      digitalWrite(this->ena_pin, LOW);
    }
    void off() {
      digitalWrite(this->ena_pin, HIGH);
    }
    void dirCCW() {
      digitalWrite(this->dir_pin, HIGH);
    }
    void dirCW() {
      digitalWrite(this->dir_pin, LOW);
    }

    // 250 steps/sec
    void step() {
      digitalWrite(this->stp_pin, HIGH);
      delayMicroseconds(2000);
      digitalWrite(this->stp_pin, LOW);
      delayMicroseconds(2000);
    }

};

#endif