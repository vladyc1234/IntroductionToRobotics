const int latchPin = 11; // STCP to 12 on Shift Register
const int clockPin = 10; // SHCP to 11 on Shift Register
const int dataPin = 12; // DS to 14 on Shift Register

const int segD1 = 7;
const int segD2 = 6;
const int segD3 = 5;
const int segD4 = 4;

const int pinSW = 2; // digital pin connected to switch output
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output

const byte regSize = 8; // 1 byte aka 8 bits

byte swReading = LOW;

bool joyMoved = false;
int xValue = 0; 
int yValue = 0; 
int minThreshold = 300;
int maxThreshold = 700;
int resetMinThreshold = 450;
int resetMaxThreshold = 650;

int blinkDP = 0;
unsigned long blinkTimerDP = 0;

int multiplex = 0;
unsigned long multiplexTimer = 0;

int state = 1;
int PressStart = 0; // check if button pressed
unsigned long longPressTimer = 0;

int currentDisplay = 0;

int displayDigits[] = {
  segD1, segD2, segD3, segD4
};

const int encodingsNumber = 16;

int encodingArray[encodingsNumber][regSize] = {
  {1, 1, 1, 1, 1, 1, 0, 0},  // 0
  {0, 1, 1, 0, 0, 0, 0, 0},  // 1
  {1, 1, 0, 1, 1, 0, 1, 0},  // 2
  {1, 1, 1, 1, 0, 0, 1, 0},  // 3
  {0, 1, 1, 0, 0, 1, 1, 0},  // 4
  {1, 0, 1, 1, 0, 1, 1, 0},  // 5
  {1, 0, 1, 1, 1, 1, 1, 0},  // 6
  {1, 1, 1, 0, 0, 0, 0, 0},  // 7
  {1, 1, 1, 1, 1, 1, 1, 0},  // 8
  {1, 1, 1, 1, 0, 1, 1, 0},  // 9
  {1, 1, 1, 0, 1, 1, 1, 0},  // A
  {0, 0, 1, 1, 1, 1, 1, 0},  // b
  {1, 0, 0, 1, 1, 1, 0, 0},  // C
  {0, 1, 1, 1, 1, 0, 1, 0},  // d
  {1, 0, 0, 1, 1, 1, 1, 0},  // E
  {1, 0, 0, 0, 1, 1, 1, 0},  // F
};

const int displayCount = 4;

// The number shown on the displays
int number[displayCount] = {
  0, 0, 0, 0
};

void setup() 
{
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(pinSW, INPUT_PULLUP);

  for (int i = 0; i < displayCount; i++ ) 
  {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
  Serial.begin(9600);
}

int index = 0;

void loop() 
{
  xValue = analogRead(pinY);
  yValue = analogRead(pinX);
  swReading = digitalRead(pinSW);

  if (state == 1)
  {
    joyStick(xValue);
  }
  if (state == 2)
  {
    changeNumber(yValue);
  }

  writeOnDisplay();
  buttonPress();
}

void activateDisplay(int display)
{
  for (int i = 0; i < displayCount; i++)
  {
    digitalWrite(displayDigits[i], HIGH);
  }
  digitalWrite(displayDigits[display], LOW);
}

void setDpState(int state)
{
  for (int i = 0; i < encodingsNumber; i++)
  {
    encodingArray[i][regSize - 1] = state;
  }
}
void writeOnDisplay()
{
  int indexNumber = 0;
  int displayNumber = 3;

  while (indexNumber < 4)
  {
    if (multiplex == 0)
    {
      multiplexTimer = millis();
      multiplex = 1;
    }
    if (currentDisplay == displayNumber)
    {
      if (blinkDP == 0)
      {
        blinkTimerDP = millis();
        blinkDP = 1;
      }
      if (millis() - blinkTimerDP >= 300 && blinkDP == 1 && state == 1)
      {
        setDpState(1);
        blinkDP = 0;
      }
      if (state == 2)    
      {
        setDpState(1);        
      }
    }
    else
    {
      setDpState(0);
    }
    
    writeReg(encodingArray[number[indexNumber]]);
    activateDisplay(displayNumber);

    if (millis() - multiplexTimer >= 5)
    {
      indexNumber++;
      displayNumber--;
      multiplex = 0;
    }
  }
}

void writeReg(int encoding[]) 
{
  digitalWrite(latchPin, LOW);

  for (int i = 0; i < regSize; i++) 
  {  
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, encoding[i]);
    digitalWrite(clockPin, HIGH);
  }

  digitalWrite(latchPin, HIGH);
}

void changeNumber(int yValue)
{
  if (yValue < minThreshold && joyMoved == false && number[3 - currentDisplay] > 0) 
  {
    number[3 - currentDisplay]--;
    joyMoved = true;
  }
	if (yValue > maxThreshold && joyMoved == false && number[3 - currentDisplay] < 15) 
  {
    number[3 - currentDisplay]++;
    joyMoved = true;
  }
	if (yValue > resetMinThreshold && yValue < resetMaxThreshold) 
  {
    joyMoved = false;
  }
}

void joyStick(int xValue)
{
  if (xValue > maxThreshold && joyMoved == false && currentDisplay > 0) 
  {
    currentDisplay--;
    joyMoved = true;
  }
	if (xValue < minThreshold && joyMoved == false && currentDisplay < 3) 
  {
    currentDisplay++;
    joyMoved = true;
  }
	if (xValue > resetMinThreshold && xValue < resetMaxThreshold) 
  {
    joyMoved = false;
  }
}

void buttonPress()
{
  if (swReading == LOW && PressStart == 0) 
  {
    longPressTimer = millis(); 
    PressStart = 1;
  }
  if (millis() - longPressTimer >= 3000 && state == 1)
  {  
    for(int i = 0; i < 4 ; i++)
    {
      number[i] = 0;
    }
    currentDisplay = 0;
  }
  if (millis() - longPressTimer >= 100 && millis() - longPressTimer <= 3000 && swReading != LOW)
  {
    if (state == 1)
    {
      state = 2;
    }
    else if (state == 2)
    {
      state = 1;
    }
  }
  if(swReading != LOW)
  {
    longPressTimer = millis(); 
    PressStart = 0;
  }
}