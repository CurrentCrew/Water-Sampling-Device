const int enaPin = 19;
const int dirPin = 14;
const int stpPin = 15;
const int m2Pin = 16;
const int m1Pin = 17;
const int m0Pin = 18;


void setup() {
  Serial.begin(9600);
  pinMode(enaPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(stpPin, OUTPUT);
  pinMode(m2Pin, OUTPUT);
  pinMode(m1Pin, OUTPUT);
  pinMode(m0Pin, OUTPUT);
  digitalWrite(enaPin, LOW);
  digitalWrite(dirPin, 1);
  digitalWrite(m2Pin, 0);
  digitalWrite(m1Pin, 1);
  digitalWrite(m0Pin, 0);
  // digitalWrite(enaPin, HIGH);
  
  for(int i=0; i<3200; i++) {
    
    digitalWrite(stpPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stpPin, LOW);
    delayMicroseconds(1000);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
