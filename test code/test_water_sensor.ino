// Beige: 5V
// Blue: GND
// Yellow: Digital 2

#define BLACK_PIN 7
#define YELLOW_PIN 2



void setup() {
    pinMode(BLACK_PIN, OUTPUT);
    pinMode(YELLOW_PIN, INPUT);
    Serial.begin(9600);

    digitalWrite(BLACK_PIN, HIGH); // Set BLACK wire to HIGH (Yellow outputs HIGH when liquid is detected)
}

void loop() {
    int sensorState = digitalRead(YELLOW_PIN);

    if (sensorState == HIGH) {
        Serial.println("Liquid Detected!");
    } else {
        Serial.println("No Liquid Detected.");
    }

    delay(500); // Wait for stability
}