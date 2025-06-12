int needleActuatorpin1 = 26;
int needleActuatorpin2 = 27;
int lockingActuatorpin1 = 24;
int lockingActuatorpin2 = 25;

const int wheelStepPin = 42;
const int wheelDirPin = 41;
const int wheelEnPin = 40;

const int pumpEnPin = 30;

const int sensorEnablePin = 36;
const int sensorPin = 3;

const int stepPerFullRev = 3200;

const int secsBetweenSamples = 3;

const int fillTubeSteps = 200;
const int flushSteps = 200;

volatile bool waterFlowing = false;

int currentPosition;

void takeSample() {

  stepWheel(currentPosition, 1);

  lockTube();
  //insertNeedle();
  //fillTube();
  release();
}

void release() {
  digitalWrite(wheelEnPin, LOW);
  
  Serial.println("release 1");
  digitalWrite(needleActuatorpin1, HIGH);
  digitalWrite(needleActuatorpin2, LOW);

  delay(10000);

  Serial.println("release 2");
  digitalWrite(lockingActuatorpin1, LOW);
  digitalWrite(lockingActuatorpin2, HIGH);

  delay(3000);
  waterFlowing = false;
}

void stepWheel(int n, int direction) {
  
  if (direction == 1) {
    digitalWrite(wheelDirPin, HIGH);
  } else {
    digitalWrite(wheelDirPin, LOW);
  }
  
  for (int x = 0; x < n; x++) {
    digitalWrite(wheelStepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(wheelStepPin, LOW);
    delayMicroseconds(500);
  }
  digitalWrite(wheelDirPin, LOW);
}

void lockTube() {
  digitalWrite(wheelEnPin, HIGH);
  
  digitalWrite(lockingActuatorpin1, HIGH);
  digitalWrite(lockingActuatorpin2, LOW);
  delay(3000);
}

void insertNeedle() {
  digitalWrite(needleActuatorpin1, LOW);
  digitalWrite(needleActuatorpin2, HIGH);

  delay(12000);

}

void fillTube() {
  //digitalWrite(pumpEnPin, HIGH);
  Serial.println("begin pump");
  while (!waterFlowing) {
    digitalWrite(pumpEnPin, HIGH);
  }
  Serial.println("end pump");
  digitalWrite(pumpEnPin, LOW);
  waterFlowing = false;
}

void setup() {
  Serial.begin(9600);

  //Set up water sensor
  digitalWrite(sensorEnablePin, HIGH);

  // Set up linearActuators
  pinMode(needleActuatorpin1, OUTPUT);
  pinMode(needleActuatorpin2, OUTPUT);
  pinMode(lockingActuatorpin1,  OUTPUT);
  pinMode(lockingActuatorpin2, OUTPUT);

  // Set up wheel stepper motor
  pinMode(wheelStepPin, OUTPUT);
  pinMode(wheelDirPin, OUTPUT);
  pinMode(wheelEnPin, OUTPUT);
  digitalWrite(wheelEnPin, LOW);

  // Set up pump
  pinMode(pumpEnPin, OUTPUT);
  digitalWrite(pumpEnPin, LOW);

  release();

  Serial.println("move stepper");
  stepWheel(stepPerFullRev/32, 1);
  currentPosition = stepPerFullRev/32;
  lockTube();

  attachInterrupt(digitalPinToInterrupt(sensorPin),stopPump,RISING);
  // pinMode(sensorPin, INPUT);

}

void loop() {
  takeSample();
  delay(secsBetweenSamples*1000);
}

void stopPump() {
  waterFlowing = true;
}
