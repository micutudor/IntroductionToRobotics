#include "LedControl.h"

// Pin configurations
const int DATA_INPUT_PIN = 12;
const int CLOCK_INPUT_PIN = 11;
const int LATCH_INPUT_PIN = 10;
const int X_AXIS_PIN = A0;
const int Y_AXIS_PIN = A1;
const int SWITCH_PIN = A2;

// LED matrix configurations
LedControl matrixController = LedControl(DATA_INPUT_PIN, CLOCK_INPUT_PIN, LATCH_INPUT_PIN, 1);
const int NUMBER_OF_MATRICES = 3;
const byte MATRIX_SIZE = 8;
byte matrixPatterns[NUMBER_OF_MATRICES][MATRIX_SIZE][MATRIX_SIZE] = {
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 1, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 1, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 1, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
  },
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 1, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 1, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 1, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
  },
  {
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
  }
};

// Player configurations
byte xPosition = 0;
byte yPosition = 0;
byte xPreviousPos = 0;
byte yPreviousPos = 0;
int selectedMatrix = 0;
boolean switchState = false;
boolean existence = false;
int xBlinking = -1;
int yBlinking = -1;
int lastPositionUpdateTime;

// Timing and intervals
const int MIN_THRESHOLD_VALUE = 200;
const int MAX_THRESHOLD_VALUE = 600;
const byte MOVEMENT_INTERVAL = 100;
unsigned long lastMovementTime = 0;
bool matrixAltered = true;

// Brightness configuration
byte brightnessLevel = 2;

void setup() {
  matrixController.shutdown(0, false);
  matrixController.setIntensity(0, brightnessLevel);
  matrixController.clearDisplay(0);
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  matrixPatterns[selectedMatrix][xPosition][yPosition] = 1;

  updateMatrixDisplay();
}

void loop() 
{
  boolean currentSwitchState = digitalRead(SWITCH_PIN);

  if (currentSwitchState == LOW) 
  {
    switchState = !switchState;
  }

  if (millis() - lastMovementTime >= MOVEMENT_INTERVAL) 
  {
    xPreviousPos = xPosition;
    yPreviousPos = yPosition;
    updateCoordinates();

    if ((xPreviousPos != xPosition || yPreviousPos != yPosition) && switchState == true) {
        matrixPatterns[selectedMatrix][xPreviousPos][yPreviousPos] = 1;
        matrixController.setLed(1, xPreviousPos, yPreviousPos, matrixPatterns[selectedMatrix][xPreviousPos][yPreviousPos]);

        lastPositionUpdateTime = millis();

        existence = true;
        xBlinking = xPreviousPos;
        yBlinking = yPreviousPos;
        switchState = false;
    }
    lastMovementTime = millis();
  }

  if (existence) 
  {
    blinkAndUpdate(xBlinking, yBlinking, 200);
  }

  if (existence && millis() - lastPositionUpdateTime >= 3000) 
  {
    matrixPatterns[selectedMatrix][xBlinking][yBlinking] = 0;
    matrixController.setLed(1, xBlinking, yBlinking, matrixPatterns[selectedMatrix][xBlinking][yBlinking]);

    updateAdjacentLEDs(xBlinking, yBlinking);

    existence = false;
    updateMatrixDisplay();
  }

  if (matrixAltered) 
  {
    updateMatrixDisplay();
    matrixAltered = false;
  }

  blinkAndUpdate(xPosition, yPosition, 400);
}

void updateMatrixDisplay() {
  for (int i = 0; i < MATRIX_SIZE; i ++)
    for (int j = 0; j < MATRIX_SIZE; j ++)
      matrixController.setLed(0, i, j, matrixPatterns[selectedMatrix][i][j]);
}

void updateCoordinates() 
{
  int xValue = analogRead(X_AXIS_PIN);
  int yValue = analogRead(Y_AXIS_PIN);
  int hasMoved = false;
  byte newXPos = xPosition;
  byte newYPos = yPosition;

  if (xValue < MIN_THRESHOLD_VALUE && newXPos > 0 && hasMoved == false) 
  {
    newXPos--;
    hasMoved = true;
  } 
  else if (xValue > MAX_THRESHOLD_VALUE && newXPos < MATRIX_SIZE - 1 && hasMoved == false) 
  {
    newXPos++;
    hasMoved = true;
  }

  if (yValue < MIN_THRESHOLD_VALUE && newYPos < MATRIX_SIZE - 1 && hasMoved == false) 
  {
    newYPos++;
    hasMoved = true;
  } 
  else if (yValue > MAX_THRESHOLD_VALUE && newYPos > 0 && hasMoved == false) {
    newYPos--;
    hasMoved = true;
  }

  if (matrixPatterns[selectedMatrix][newXPos][newYPos] == 0) 
  {
      matrixAltered = true;
      matrixPatterns[selectedMatrix][xPosition][yPosition] = 0;
      matrixPatterns[selectedMatrix][newXPos][newYPos] = 1;
      xPreviousPos = xPosition;
      yPreviousPos = yPosition;
      xPosition = newXPos;
      yPosition = newYPos;
  }
}

void updateMatrixPattern(byte newXPos, byte newYPos, bool hasMoved) 
{
  matrixAltered = true;
  matrixPatterns[selectedMatrix][xPosition][yPosition] = 0;
  matrixPatterns[selectedMatrix][newXPos][newYPos] = 1;

  if (hasMoved) {
      xPreviousPos = xPosition;
      yPreviousPos = yPosition;
      xPosition = newXPos;
      yPosition = newYPos;
  }
}

void blinkAndUpdate(byte x, byte y, unsigned int blinkInterval) 
{
  static unsigned long lastBlinkTime = 0;
  static bool isOn = true;

  if (millis() - lastBlinkTime >= blinkInterval) 
  {
      isOn = !isOn;
      matrixController.setLed(0, x, y, isOn);
      lastBlinkTime = millis();
  }
}

void updateAdjacentLEDs(byte x, byte y) 
{
  if (x > 0) 
  {
    matrixPatterns[selectedMatrix][x - 1][y] = 0;
    matrixController.setLed(1, x - 1, y, matrixPatterns[selectedMatrix][x - 1][y]);
  }

  if (x < (MATRIX_SIZE - 1)) 
  {
    matrixPatterns[selectedMatrix][x + 1][y] = 0;
    matrixController.setLed(1, x + 1, y, matrixPatterns[selectedMatrix][x + 1][y]);
  }

  if (y > 0) 
  {
    matrixPatterns[selectedMatrix][x][y - 1] = 0;
    matrixController.setLed(1, x, y - 1, matrixPatterns[selectedMatrix][x][y - 1]);
  }

  if (y < (MATRIX_SIZE - 1)) 
  {
    matrixPatterns[selectedMatrix][x][y + 1] = 0;
    matrixController.setLed(1, x, y + 1, matrixPatterns[selectedMatrix][x][y + 1]);
  }
}