// declare all the joystick pins
const int pinSW = 2; // digital pin connected to switch output
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output

// declare all the segments pins
const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;

const int segSize = 8;
const int noOfDigits = 8;

byte swReading = LOW;

volatile int blink = 0; // check if blink started
volatile unsigned long blinkTimer = 0;
int blinkInterval = 200;

bool joyMoved = false;
int xValue = 0; 
int yValue = 0; 
int currentSeg = 4;
int coordonateY = 1; // curent coordonates for horizontal plane
int coordonateX = 1; // curent coordonates for vertical plane
int minThreshold = 300;
int maxThreshold = 700;
int resetMinThreshold = 400;
int resetMaxThreshold = 600;

int state = 1;
int PressStart = 0; // check if button pressed
unsigned long longPressTimer = 0;

unsigned long lastDebounceTime = 0;
unsigned int debounceTime = 100;

int segments[segSize] = { 
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

byte digitMatrix[noOfDigits][segSize] = {
  {1, 0, 0, 0, 0, 0, 0, 0}, // a
  {0, 1, 0, 0, 0, 0, 0, 0}, // b
  {0, 0, 1, 0, 0, 0, 0, 0}, // c
  {0, 0, 0, 1, 0, 0, 0, 0}, // d
  {0, 0, 0, 0, 1, 0, 0, 0}, // e
  {0, 0, 0, 0, 0, 1, 0, 0}, // f
  {0, 0, 0, 0, 0, 0, 1, 0}, // g
  {0, 0, 0, 0, 0, 0, 0, 1}  // dp
};

// encoding to determine the state for each coordonates (first number is X, second is Y, third is the segment number)
int segMap[segSize] = {
  114, 213, 312, 417, 125, 226, 321, 230
};

// the segment states
byte segState[segSize] = {
  0, 0, 0, 0, 0, 0, 0, 0
};

void setup() 
{
  for (int i = 0; i < segSize; i++) 
  {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(pinSW, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() 
{
  xValue = analogRead(pinY);
  yValue = analogRead(pinX);
  swReading = digitalRead(pinSW);

  translateMap(coordonateX, coordonateY);
  
  if (state == 2)
  {
    displaySegmentSegState();
    changeSegState();
  }
  
  if (state == 1)
  {
    joyStick(xValue, yValue);
    displaySegmentStateOne(currentSeg);   
    displaySegmentSegState();
  }

  buttonPress();
}

void changeSegState()
{
  if (state == 2)
  {
    if (yValue < minThreshold) 
    {
      segState[currentSeg] = 0;
    }
    if (yValue > maxThreshold) 
    {
      segState[currentSeg] = 1;
    }
  }
}

void joyStick(int xValue, int yValue)
{
  if (xValue < minThreshold && joyMoved == false && coordonateX > 1) 
  {
    if (coordonateY == 3)
    {
      coordonateY--;
    }
    coordonateX--;
    joyMoved = true;
  }
	if (xValue > maxThreshold && joyMoved == false && (coordonateX < 3 || (coordonateY == 1 && coordonateX < 4))) 
  {
    if (coordonateY == 3)
    {
      coordonateY--;
    }
    coordonateX++;
    joyMoved = true;
  }
  if (yValue < minThreshold && joyMoved == false && coordonateY > 1) 
  {
    if (coordonateY == 2 && coordonateX != 2)
    {
      coordonateX = 2;
    }
    else
    {
      coordonateY--;
    }
    joyMoved = true;
  }
	if (yValue > maxThreshold && joyMoved == false && coordonateY < 3 && coordonateX < 4) 
  {
    coordonateY++;
    joyMoved = true;
    if(coordonateY == 2 || coordonateY == 3) 
    {
      coordonateX = 2;
    }
  }
	if (xValue > resetMinThreshold && xValue < resetMaxThreshold && yValue > resetMinThreshold && yValue < resetMaxThreshold) 
  {
    joyMoved = false;
  }
}

// translate the coordonates to get the current position on the display
void translateMap(int coordonateX, int coordonateY)
{
  for (int i = 0; i < noOfDigits; i++)
  {
    if (segMap[i] / 100 == coordonateX && segMap[i] / 10 % 10 == coordonateY)
    {
      currentSeg = segMap[i] % 10;
    }
  }
}

void displaySegmentStateOne(byte seg) 
{
  if(blink == 0)
  {
    blinkTimer = millis();
    blink = 1;
  }

  if (millis() - blinkTimer >= blinkInterval && blink == 1)
  {
    for (int i = 0; i < segSize; i++)
    {
      digitalWrite(segments[i], digitMatrix[seg][i]);
    }
    blinkTimer = millis();
    blink = 2;
  }
}

void displaySegmentSegState() 
{
  if (state == 1)
  {
    if(millis() - blinkTimer >= blinkInterval && blink == 2)
    {
      digitalWrite(segments[currentSeg], LOW);
      blink = 0;
    }
  }  
  for (int i = 0; i < segSize; i++)
  {
    if (i != currentSeg && state == 1)
    {
      digitalWrite(segments[i], segState[i]);
    }
    else if (state == 2)
    {
      digitalWrite(segments[i], segState[i]);
    }
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
    for (int i = 0; i < segSize; i++)
    {
      segState[i] = 0;
    }
    coordonateX = 4;
    coordonateY = 1;
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

