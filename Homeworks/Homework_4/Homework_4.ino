const int MAX_SEGMENTS = 9;

const int pinA = 12;
const int pinB = 10;
const int pinC = 9;
const int pinD = 8;
const int pinE = 7;
const int pinF = 6;
const int pinG = 5;
const int pinDP = 4;

const int segments[] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};
 
int currentSegment = 1;
 
const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;
 
void(* resetFunc)(void) = 0;
 
byte swState = LOW;
byte lastSwState = LOW;
 
unsigned long currentMillis;
unsigned long previousMillis = 0;
byte ledState = LOW;
 
long currentMillisSw;
long previousMillisSw = 0;
 
int savedSegments[MAX_SEGMENTS];
int savedSegmentsTotal = 0;
 
int xValue;
int yValue;
 
int pos;
int lastPos = 0;
 
 
void setup() 
{
  for (int i = 0; i < MAX_SEGMENTS; i ++) 
    pinMode(segments[i], OUTPUT);

  pinMode(pinSW, INPUT_PULLUP);

  digitalWrite(pinDP, HIGH);
}
 
void loop() 
{
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
 
  pos = 0;
 
  if (xValue > 800) pos = 1;
  else if (xValue < 200) pos = 2;
  else if (yValue > 800) pos = 3;
  else if (yValue < 200) pos = 4;
 
  if (pos != lastPos)
  {
    switch (pos)
    {
      case 1:
      {
        if (currentSegment == 2 || currentSegment == 3 || currentSegment == 4)
            currentSegment = 6;
        else if (currentSegment == 0 || currentSegment == 1 || currentSegment == 5 || currentSegment == 6)
            currentSegment = 0;

        break;
      }

      case 2:
      {
        if (currentSegment == 0 || currentSegment == 1 || currentSegment == 5)
            currentSegment = 6;
        else if (currentSegment == 2 || currentSegment == 3 || currentSegment == 4 || currentSegment == 6)
            currentSegment = 3;

        break;
      }

      case 3:
      {
        if (currentSegment == 0 || currentSegment == 1 || currentSegment == 5)
            currentSegment = 1;
        else if (currentSegment == 3 || currentSegment == 4)
            currentSegment = 2;
        else if (currentSegment == 2)
            currentSegment = 7;
        else
            currentSegment = 6;

        break;
      }

      case 4:
      {
        if (currentSegment == 0 || currentSegment == 1 || currentSegment == 5)
            currentSegment = 5;
        else if (currentSegment == 2 || currentSegment == 3 || currentSegment == 4)
            currentSegment = 4;
        else if (currentSegment == 7)
            currentSegment = 2;
            
        break;
      }
    }

    setAllPins(LOW);
  }

  lastPos = pos;
 
  swState = digitalRead(pinSW);

  if (swState != lastSwState && swState == LOW) 
  {
    savedSegments[savedSegmentsTotal] = currentSegment;
    savedSegmentsTotal ++;
  }

  lastSwState = swState;
 
  for (int i = 0; i < savedSegmentsTotal; i ++)
    if (savedSegments[i] != currentSegment)
      digitalWrite(segments[savedSegments[i]], HIGH); 
 
  currentMillis = millis();

  if (currentMillis - previousMillis >= 500) 
  {
    previousMillis = currentMillis;
    ledState = (ledState == LOW) ? HIGH : LOW;
    digitalWrite(segments[currentSegment], ledState);
  }
 
  swState = digitalRead(pinSW);

  if (swState == LOW)
    currentMillisSw = millis();

  if (swState == HIGH)
    previousMillisSw = millis();
 
  if (currentMillisSw - previousMillisSw > 1500)
  {
    setAllPins(LOW);
    resetFunc();
  }
 
}

void setAllPins(int value) 
{
  for(int i = 0; i < MAX_SEGMENTS; i ++)
    digitalWrite(segments[i], value);
}
