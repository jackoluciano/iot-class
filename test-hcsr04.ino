#define PIN_TRIG 25
#define PIN_ECHO 32

void setup() {
  Serial.begin(115200);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  Serial.println("Test HC-SR04 dimulai...");
}

float readDistanceCM() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(PIN_TRIG, LOW);

  long duration = pulseIn(PIN_ECHO, HIGH, 30000UL);

  if (duration == 0) {
    return -1;
  }

  float distance = duration * 0.0343 / 2.0;
  return distance;
}

void loop() {
  float distance = readDistanceCM();

  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(1000);
}