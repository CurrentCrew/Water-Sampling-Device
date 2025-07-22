#ifndef ROTARY_H
#define ROTARY_H

class Rotary {
  public:
    int step_position;
    int stp_pin;
    int dir_pin;
    int ena_pin;
    int inversion;

    // Constructors
    Rotary(const int stp_pin, const int dir_pin, const int ena_pin, int inversion) {
        this->stp_pin = stp_pin;
        this->dir_pin = dir_pin;
        this->ena_pin = ena_pin;
        this->inversion = inversion;

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
      if (this->inversion == 0)
        digitalWrite(this->dir_pin, HIGH);
      else
        digitalWrite(this->dir_pin, LOW);
    }
    void dirCW() {
      if (this->inversion == 0)
        digitalWrite(this->dir_pin, LOW);
      else
        digitalWrite(this->dir_pin, HIGH);
    }

    // 250 steps/sec
    void step() {
      digitalWrite(this->stp_pin, HIGH);
      delayMicroseconds(1000);
      digitalWrite(this->stp_pin, LOW);
      delayMicroseconds(1000);
    }

};

#endif