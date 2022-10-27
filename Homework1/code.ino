//default values
#define MAX_POT_VALUE 1023
#define MIN_POT_VALUE 0
#define MAX_RGB_VALUE 255
#define MIN_RGB_VALUE 0

//led pins
const int redPin = 11;
const int greenPin = 10;
const int bluePin = 9;

//potentiometer pins
const int redPotPin = A0;
const int greenPotPin = A1;
const int bluePotPin = A2;

//potentiometer analog values
int redPotValue; 
int greenPotValue; 
int bluePotValue;

void setup() 
{
  pinMode(redPotPin, INPUT);
  pinMode(greenPotPin, INPUT);
  pinMode(bluePotPin, INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  Serial.begin(9600);
}


//set led color function
void setColor(int redPotValue, int greenPotValue , int bluePotValue)
{
  analogWrite(redPin, map(redPotValue, MIN_POT_VALUE, MAX_POT_VALUE, MIN_RGB_VALUE, MAX_RGB_VALUE));
  analogWrite(greenPin, map(greenPotValue, MIN_POT_VALUE, MAX_POT_VALUE, MIN_RGB_VALUE, MAX_RGB_VALUE));
  analogWrite(bluePin, map(bluePotValue, MIN_POT_VALUE, MAX_POT_VALUE, MIN_RGB_VALUE, MAX_RGB_VALUE));
}

void loop() 
{
  redPotValue = analogRead(redPotPin);
  greenPotValue = analogRead(greenPotPin);
  bluePotValue = analogRead(bluePotPin);

  setColor(redPotValue, greenPotValue, bluePotValue);
}
