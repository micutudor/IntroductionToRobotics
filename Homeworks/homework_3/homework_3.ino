const int MAX_FLOORS = 3;

// LEDs
const int ledsPins[] = {12, 11, 10};
const int doorsLedPin = 8;

// BUZZER
const int buzzerPin = 9;    

// BUTTONS
const int buttonsPins[] = {2, 3, 4};    
bool buttonsStates[] = {false, false, false};
bool lastButtonsStates[] = {false, false, false};

const int debounceTime = 50;
unsigned long lastDebounceTime[] = {0, 0, 0};

// FLOOR STATES
bool inMotion = false;
int currentFloor = 0, targetFloor = -1;

void setup() 
{
  for (int i = 0; i < MAX_FLOORS; i ++) 
    pinMode(ledsPins[i], OUTPUT);

  for (int i = 0; i < MAX_FLOORS; i ++) 
    pinMode(buttonsPins[i], INPUT_PULLUP);

  pinMode(doorsLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Initial setup
  digitalWrite(ledsPins[0], HIGH);
  digitalWrite(doorsLedPin, HIGH);
}

void loop() 
{
  for (int i = 0; i < MAX_FLOORS; i ++)
  {
    int reading = digitalRead(buttonsPins[i]);

    if (reading != lastButtonsStates[i]) 
    {
      lastDebounceTime[i] = millis();
    }
    
    if ((millis() - lastDebounceTime[i]) > debounceTime) 
    {
      if (reading != buttonsStates[i]) 
      {
        buttonsStates[i] = reading;

        if (!buttonsStates[i] && !inMotion && targetFloor == -1 && i != currentFloor) 
        {
            targetFloor = i;
            inMotion = true;

            digitalWrite(doorsLedPin, LOW);
            tone(buzzerPin, 1000);

            delay(1000);

            tone(buzzerPin, 500);
        }
      }
    }

    lastButtonsStates[i] = reading;
  }

  if (inMotion) 
  {
    if (currentFloor < targetFloor) currentFloor ++;
    else if (currentFloor > targetFloor) currentFloor --;
    else 
    {
      inMotion = false;

      digitalWrite(doorsLedPin, HIGH);
      tone(buzzerPin, 1000);

      delay(1000);

      noTone(buzzerPin);

      targetFloor = -1;
    }

    for (int i = 0; i < MAX_FLOORS; i ++) 
      if (i == currentFloor)
        digitalWrite(ledsPins[i], HIGH);
      else
        digitalWrite(ledsPins[i], LOW);

    delay(2000);
  }
}