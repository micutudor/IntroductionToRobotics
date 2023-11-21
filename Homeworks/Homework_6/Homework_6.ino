const int RED = 2;
const int GREEN = 1;
const int BLUE = 0;

// PIN DEFINITIONS
const int LEDPins[] = { 3, 5, 6 };

const int TrigPin = 10;
const int EchoPin = 9;

const int PCPin = 0;

int incomingByte = 0;
int currentMenu = 0;

int interval = 0;

int ultrasonicDistance = 0;
int refDistance = 0;

int PCValue = 0;
int refPCValue = 0;

int latestRefDistances[10], latestRefPCValues[10];

long lastSamplingTime = 0;

long duration = 0;

bool isAuto = true;
int red = 0, green = 0, blue = 0;

void setup() 
{
  for (int i = 0; i < 3; i ++)
    pinMode(LEDPins[i], OUTPUT);

  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);

  Serial.begin(9600);
  MainMenu();
}

void loop() 
{
  if (Serial.available() > 0) 
  {
    incomingByte = Serial.read() - '0';

    switch (currentMenu) 
    {
      case 0: { HandleMainMenu(); break; }
      case 1: { SensorSettingsMenu(); break; }
      case 2: { ResetLoggerDataMenu(); break; }
      case 3: { SystemStatusMenu(); break; }
      case 4: { RGBLedControlMenu(); break; }
    }
  }

  HandleUltrasonicSensor();
  HandleLDRSensor();

  if (isAuto) analogWrite(LEDPins[GREEN], HIGH);
  else analogWrite(LEDPins[GREEN], green);

  if ((millis() - lastSamplingTime) >= interval * 1000) 
  {
    if (refDistance < ultrasonicDistance) 
    {
      Serial.print("Distance: ");
      Serial.println(refDistance);

      if (isAuto) analogWrite(LEDPins[RED], HIGH);
      else analogWrite(LEDPins[RED], red);

      analogWrite(LEDPins[GREEN], LOW);

      for (int i = 9; i >= 0; i --)
          latestRefDistances[i] = latestRefDistances[i - 1];

      latestRefDistances[0] = refDistance;

      Serial.println("Near object detected!");
      Serial.println();
    } 
    else
      analogWrite(LEDPins[RED], LOW);

    if (refPCValue < PCValue) 
    {
      Serial.print("LDR: ");
      Serial.println(refPCValue);

      if (isAuto) analogWrite(LEDPins[BLUE], HIGH);
      else analogWrite(LEDPins[BLUE], blue);

      analogWrite(LEDPins[GREEN], LOW);

      for (int i = 9; i >= 0; i --)
          latestRefPCValues[i] = latestRefPCValues[i - 1];

      latestRefPCValues[0] = refPCValue;

      Serial.println("It's night!");
      Serial.println();
    }
    else
      analogWrite(LEDPins[BLUE], LOW);

    lastSamplingTime = millis();
  }
}

void MainMenu() 
{
  Serial.println("Menu:");
  Serial.println("1. Sensor Settings");
  Serial.println("2. Reset Logger Data");
  Serial.println("3. System Status");
  Serial.println("4. RGB LED Control");
  Serial.println();
}

void HandleMainMenu()
{
  switch (incomingByte) 
  {
    case 1: 
    {
      Serial.println("1.1 Sensors Sampling Interval");
      Serial.println("1.2 Ultrasonic Alert Threshold");
      Serial.println("1.3 LDR Alert Threshold");
      Serial.println("1.4 Back");
      currentMenu = 1;
      break;
    }

    case 2: 
    {
      Serial.println("2.1 Yes");
      Serial.println("2.2 No");
      currentMenu = 2;
      break;
    }

    case 3: 
    {
      Serial.println("3.1 Current Sensor Readings");
      Serial.println("3.2 Current Sensor Settings");
      Serial.println("3.3 Display Logged Data");
      Serial.println("3.4 Back");
      currentMenu = 3;
      break;
    }

    case 4: 
    {
      Serial.println("4.1 Manual Color Control");
      Serial.println("4.2 LED: Toggle Automatic ON/OFF");
      Serial.println("4.3 Back");
      currentMenu = 4;
      break;
    }
  }

  Serial.println();
}

void SensorSettingsMenu()
{
  switch (incomingByte) 
  {
    case 1: 
    {
      Serial.println("Sampling interval (1 - 10 seconds): ");
      ClearSerial();

      while (!Serial.available()) {}

      interval = Serial.read() - '0';

      Serial.print("Sampling interval is ");
      Serial.println(interval);
      Serial.println();

      break;
    }

    case 2: 
    {
      Serial.println("Ultrasonic Threshold");
      ClearSerial();

      while (!Serial.available()) {}

      ultrasonicDistance = Serial.parseInt();

      Serial.print("Ultrasonic min value: ");
      Serial.println(ultrasonicDistance);
      Serial.println();

      break;
    }

    case 3: 
    {
      Serial.println("LDR Threshold");
      ClearSerial();

      while (!Serial.available()) {}

      PCValue = Serial.parseInt();

      Serial.print("LDR min value: ");
      Serial.println(PCValue);
      Serial.println();

      break;
    }

    case 4: 
    {
      currentMenu = 0;
      MainMenu();
      break;
    }
  }
}

void ResetLoggerDataMenu()
{
  switch (incomingByte) 
  {
    case 1: 
    {
      ClearData();

      currentMenu = 0;
      MainMenu();
      break;
    }

    case 2: 
    {
      currentMenu = 0;
      MainMenu();
      break;
    }
  }
}

void SystemStatusMenu()
{
  switch (incomingByte) 
  {
    case 1: 
    {
      ClearSerial();

      while (Serial.available() == 0) 
      {
        if ((millis() - lastSamplingTime) >= interval * 1000) 
        {
          HandleUltrasonicSensor();
          HandleLDRSensor();

          Serial.print("Current refDistance: ");
          Serial.println(refDistance);

          Serial.print("Current lighting: ");
          Serial.println(refPCValue);

          Serial.print("Done? Press 0.");
          Serial.println();

          lastSamplingTime = millis();
        }
      }

      if (Serial.read() - '0' == 4) 
          currentMenu = 3;

      break;
    }

    case 2: 
    {
      Serial.print("Sampling Interval: ");
      Serial.println(interval);

      Serial.print("Ultrasonic Threshold: ");
      Serial.println(ultrasonicDistance);

      Serial.print("LDR Threshold: ");
      Serial.println(PCValue);

      Serial.println();

      break;
    }

    case 3: 
    {
      Serial.println("Last 10 Values:");
      Serial.println();

      for (int i = 0; i < 10; i ++) 
      {
        if (latestRefDistances[i] != 0)
        {
          Serial.print("Distance: ");
          Serial.println(latestRefDistances[i]);
        }

        if (latestRefPCValues[i] != 0)
        {
          Serial.print("LDR: ");
          Serial.println(latestRefPCValues[i]);
        }

        Serial.println();
      }

      Serial.println();

      break;
    }

    case 4: 
    {
      currentMenu = 0;
      MainMenu();
      break;
    }
  }
}

void RGBLedControlMenu()
{
  switch (incomingByte) 
  {
    case 1: 
    {
      ClearSerial();
      Serial.print("Enter the Red value (0-255): ");

      while (!Serial.available()) {}

      red = Serial.parseInt();

      ClearSerial();
      Serial.print("Enter the Green value (0-255): ");

      while (!Serial.available()) {}

      green = Serial.parseInt();

      ClearSerial();
      Serial.print("Enter the Blue value (0-255): ");

      while (!Serial.available()) {}

      blue = Serial.parseInt();

      red = constrain(red, 0, 255);
      green = constrain(green, 0, 255);
      blue = constrain(blue, 0, 255);

      Serial.print("Selected RGB values: ");

      Serial.print("Red: ");
      Serial.print(red);

      Serial.print(", Green: ");
      Serial.print(green);

      Serial.print(", Blue: ");
      Serial.println(blue);
      break;
    }

    case 2: 
    {
      isAuto = !isAuto;

      if (isAuto) Serial.println("Auto turned ON.");
      else Serial.println("Auto turned OFF.");

      Serial.println();
      break;
    }

    case 3: {
      currentMenu = 0;
      MainMenu();
      break;
    }
  }
}

void HandleUltrasonicSensor() 
{
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(TrigPin, LOW);

  duration = pulseIn(EchoPin, HIGH);
  refDistance = duration * 0.034 / 2;
}

void HandleLDRSensor() 
{
  refPCValue = analogRead(PCPin);
}

void ClearSerial() 
{
  char buffer[1];

  delay(100);

  while (Serial.available() != 0) 
    Serial.readBytes(buffer, 1);

  delay(1000);
}

void ClearData() 
{
  for (int i = 0; i < 10; i ++) 
  {
     latestRefPCValues[i] = 0;
     latestRefDistances[i] = 0;
  }
}