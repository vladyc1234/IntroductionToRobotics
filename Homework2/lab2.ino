#define buzzerBlinkingIntervalStateThree 600
#define buzzerBlinkingIntervalStateFour 300
#define buzzerTone 500
#define buzzTime 200
#define crosswalkBlinkingInterval 400
#define stateOneTime 8000 
#define stateTwoTime 4000
#define stateThreeTime 8000
#define stateFourTime 4000

const int buttonPin = 2;

const int crossWalkGreenPin = 8;
const int crossWalkRedPin = 9;

const int traficGreenPin = 3;
const int traficYellowPin = 4;
const int traficRedPin = 5;

const int buzzerPin = 6;

byte crosswalkBlinking = LOW;
int blink = 0; //check if the led has to blink

int State = 1;
int started = 0; //checks if state timer has started
unsigned long startMoment; //the moment the state starts

byte reading = LOW;
byte buttonState = LOW;
byte lastReading = HIGH;
int pressed = 0; //check if button was pressed

unsigned int lastDebounceTime = 0;
unsigned int debounceDelay = 50;

void StateOne()
{
  digitalWrite(crossWalkGreenPin, LOW);
  digitalWrite(crossWalkRedPin, HIGH);

  digitalWrite(traficGreenPin, HIGH);
  digitalWrite(traficYellowPin, LOW);
  digitalWrite(traficRedPin, LOW);
}

void StateTwo()
{
  digitalWrite(crossWalkGreenPin, LOW);
  digitalWrite(crossWalkRedPin, HIGH);

  digitalWrite(traficGreenPin, LOW);
  digitalWrite(traficYellowPin, HIGH);
  digitalWrite(traficRedPin, LOW);
}

void StateThree()
{
  digitalWrite(crossWalkGreenPin, HIGH);
  digitalWrite(crossWalkRedPin, LOW);

  digitalWrite(traficGreenPin, LOW);
  digitalWrite(traficYellowPin, LOW);
  digitalWrite(traficRedPin, HIGH);

  if(millis() % buzzerBlinkingIntervalStateThree == 0)
  {
    tone(buzzerPin, buzzerTone, buzzTime);
  }
}

void StateFour()
{
  if(millis() % crosswalkBlinkingInterval == 0)
  {
    blink = 1;
  }

  if(blink == 1)
  {
    crosswalkBlinking = !crosswalkBlinking;
    blink = 0;
  }

  digitalWrite(crossWalkGreenPin, crosswalkBlinking);
  digitalWrite(crossWalkRedPin, LOW);

  digitalWrite(traficGreenPin, LOW);
  digitalWrite(traficYellowPin, LOW);
  digitalWrite(traficRedPin, HIGH);

  if(millis() % buzzerBlinkingIntervalStateFour == 0)
  {
    tone(buzzerPin, buzzerTone, buzzTime);
  }
}

void setup() 
{
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(crossWalkGreenPin, OUTPUT);
  pinMode(crossWalkRedPin, OUTPUT);

  pinMode(traficGreenPin, OUTPUT);
  pinMode(traficYellowPin, OUTPUT);
  pinMode(traficRedPin, OUTPUT);

  Serial.begin(9600);
}

void loop() 
{
  if(State == 1)
  {
    StateOne();

    reading = digitalRead(buttonPin);

    if(reading != lastReading)
    {
      lastDebounceTime = millis();
    }
    
    if(millis() - lastDebounceTime >= debounceDelay)
    {
      if (reading != buttonState) 
      {
        buttonState = reading;

        if (buttonState == LOW) 
        {
          pressed = 1;
          
          if(started == 0)
          {
          startMoment = millis();     
          started = 1; 
          }
        }
      }
    }

    if(pressed == 1)
    {
      if(millis() - startMoment >= stateOneTime)
      {
        State = 2;
        startMoment = millis();
        started = 0;
        pressed = 0;
      }
    }

    lastReading = reading;
    
  }
  else if(State == 2)
  {
    if(started == 0)
    {
      startMoment = millis();     
      started = 1; 
    }
    
    StateTwo();

    if(millis() - startMoment >= stateTwoTime)
    {
      State = 3;
      startMoment = millis();
      started = 0;
    }
  }
  else if(State == 3)
  {
    if(started == 0)
    {
      startMoment = millis();     
      started = 1; 
    }
    
    StateThree();

    if(millis() - startMoment >= stateThreeTime)
    {
      State = 4;
      startMoment = millis();
      started = 0;
    }
  }
  else if(State == 4)
  {
    if(started == 0)
    {
      startMoment = millis();     
      started = 1; 
    }
    
    StateFour();

    if(millis() - startMoment >= stateFourTime)
    {
      State = 1;
      startMoment = millis();
      started = 0;
    }
  }

  

  
}