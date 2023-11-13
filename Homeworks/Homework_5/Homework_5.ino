const int latchPin = 11; 
const int clockPin = 10;  
const int dataPin = 12;   

// Pin configurations for the 4 display 7 segments
int displayDigits[] = { 4, 5, 6, 7 };
const int displayCount = 4;  
const int encodingsNumber = 10;

// 7-segment display encodings for each digit (0-9) and also with the dp on
byte byteEncodings[encodingsNumber][2] = {
  {B11111100, B11111101},  // 0
  {B01100000, B01100001},  // 1
  {B11011010, B11011011},  // 2
  {B11110010, B11110011},  // 3
  {B01100110, B01100111},  // 4
  {B10110110, B10110111},  // 5
  {B10111110, B10111111},  // 6
  {B11100000, B11100001},  // 7
  {B11111110, B11111111},  // 8
  {B11110110, B11110111}   // 9
};

// Data for displaying the countdown
unsigned long lastIncrement = 0;
unsigned long delayCount = 100;  
unsigned long number = 0;       

// Pin configurations for the buttons
const int buttonPins[] = {2, 3, 9};
const int debounceDelay = 200;

// Buttons
byte triggerButtonState;
byte lastTriggerButtonState = HIGH;
byte triggerButtonActive = LOW;
long triggerDebounceNumber = 0;

byte resetButtonState;
byte lastResetButtonState = HIGH;
byte resetButtonActive = LOW;
long lastResetDebounceNumber = 0;

byte lapButtonState;
byte lastLapButtonState = HIGH;
long lastLapDebounceNumber = 0;

// Laps data
const int laps_count = 5;
int laps[5];
int current_lap = -1;

void setup() 
{
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  for (int i = 0; i < displayCount; i ++) 
  {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }

  for(int i = 0; i < 3; i ++)
    pinMode(buttonPins[i], INPUT_PULLUP);
}

void loop() 
{
  // Handle button events
  triggerButtonState = digitalRead(buttonPins[0]);

  if (millis() - triggerDebounceNumber > debounceDelay && triggerButtonState == LOW) 
  {
    if (triggerButtonState != lastTriggerButtonState) 
    {
      triggerButtonActive = !triggerButtonActive;
      triggerDebounceNumber = millis();
    }
  }

  lastTriggerButtonState = triggerButtonState;

  if (millis() - lastIncrement > delayCount && triggerButtonActive == 1) 
  {
    number++;
    number %= 10000;
    lastIncrement = millis();
  }

  // Reset button handling
  resetButtonState = digitalRead(buttonPins[1]);

  if (millis() - lastResetDebounceNumber > debounceDelay && resetButtonState == LOW) 
  {
    if (triggerButtonActive == LOW) 
    {
      number = 0;
      resetButtonActive = !resetButtonActive;
    } 
    else
      number = number;
  
    lastResetDebounceNumber = millis();
    lastResetButtonState = resetButtonState;
  }

  // Lap button handling
  lapButtonState = digitalRead(buttonPins[2]);

  if ((millis() - lastLapDebounceNumber > debounceDelay) && (lapButtonState == LOW)) 
  {
    if (resetButtonActive == LOW) 
    {
      current_lap = (current_lap + 1) % laps_count;
      laps[current_lap] = number;
    } 
    else
      renderLap();

    lastLapDebounceNumber = millis();
    lastLapButtonState = lapButtonState;
  }

  // Display the current number
  int currentNumber = number;
  int lastDigit;
  int displayDigit = 3;

  if (currentNumber == 0)
    for (int i = 0; i < displayCount; i ++) 
    {
      activateDisplay(i);

      if (i == 2)
        writeReg(byteEncodings[0][1]);
      else
        writeReg(byteEncodings[0][0]);

      delay(0);

      writeReg(B00000000);
    }
  else 
  {
    while (currentNumber != 0) 
    {
      lastDigit = currentNumber % 10;

      activateDisplay(displayDigit);

      if (displayDigit == 2) 
        writeReg(byteEncodings[lastDigit][1]);
      else
        writeReg(byteEncodings[lastDigit][0]);

      delay(0);

      writeReg(B00000000);
      displayDigit--;
      currentNumber /= 10;
    }

    for (int i = displayDigit; i >= 0; i--) 
    {
      activateDisplay(i);
      writeReg(B11111100);

      delay(0);

      writeReg(B00000000); 
    }
  }
}

void renderLap() 
{
  while (current_lap >= 0) 
  {
    number = laps[current_lap];
    current_lap --;
    return;
  }

  current_lap = laps_count - 1;
}

void activateDisplay(int displayNumber) 
{
  for (int i = 0; i < displayCount; i ++)
    digitalWrite(displayDigits[i], HIGH);

  digitalWrite(displayDigits[displayNumber], LOW);
}

void writeReg(int digit) 
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, digit);
  digitalWrite(latchPin, HIGH);
}