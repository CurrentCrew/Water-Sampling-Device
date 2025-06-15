#ifndef ROTARY_H
#define ROTARY_H

class Rotary {
    public:
        int step_position;
        int stp_pin;
        int dir_pin;
        int ena_pin;

        Rotary(int stp_pin, int dir_pin, int ena_pin) {
            this->stp_pin = stp_pin;
            this->dir_pin = dir_pin;
            this->ena_pin = ena_pin;

            this->step_position = 0;
        }

        void init() {
            pinMode(this->stp_pin, OUTPUT);
            pinMode(this->dir_pin, OUTPUT);
            pinMode(this->ena_pin, OUTPUT);

            digitalWrite(this->ena_pin, LOW);

            #ifdef ROTARY_SERIAL
            ROTARY_SERIAL.println("Initalized Rotary");
            #endif
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


};

#endif