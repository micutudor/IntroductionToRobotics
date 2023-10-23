const int MAX_PINS = 3;

// LEDS
const int ledPins[3] = {9, 10, 11};

// POTENTIOMETERS
const int potPins[3] = {A0, A1, A2};

void setup() {
  // Setup pins mode
  for (int i = 0; i < MAX_PINS; i ++)
    pinMode(ledPins[i], OUTPUT);
}

void loop() {
  // Update pin value based on potentiometer value
  for (int i = 0; i < MAX_PINS; i ++)
  {
      int potValue = analogRead(potPins[i]);
      int ledValue = map(potValue, 0, 1023, 0, 255);
      analogWrite(ledPins[i], ledValue);
  }

  delay(1);
}
