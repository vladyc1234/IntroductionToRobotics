#include <EEPROM.h>
#include <LiquidCrystal.h>

#include "LedControl.h"

const int xPin = A0;
const int yPin = A1;
const int pinSW = 0;
byte swReading;
unsigned long PressTimer = 0;
int PressStart = 0;
bool joyMoved = false;
const int minThreshold = 200;
const int maxThreshold = 600;
int resetMinThreshold = 450;
int resetMaxThreshold = 580;

const int dinPin = 12;
const int clockPin = 10;
const int loadPin = 11;
byte matrixBrightness = 5;
LedControl lc = LedControl(dinPin, loadPin, clockPin, 1);

const byte RS = 9;
const byte enable = 8;
const byte d4 = 7;
const byte d5 = A2;
const byte d6 = A3;
const byte d7 = 4;
const byte backLightPin = 6;
const byte contrastPin = 5;
int brightness = 0;
int contrast = 0;
unsigned long startTimer = 0;
char time = '3';
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

int buzzerPin = 3;
int buzzerVolume = 1;
int VolumePin = A4;

byte xPos = 0;
byte yPos = 0;
byte newFoodPosX = 0;
byte newFoodPosY = 0;
byte xLastPos = 0;
byte yLastPos = 0;
int eaten = 0;
const byte moveInterval = 500;
unsigned long long lastMoved = 0;
unsigned long long blink = 0;
int blinked = 0;
int score = 0;
int gameTime = 10000;

int xMenu = 0;
int panel = 0;
int clear = 0;
int deleteChoice = 1;

unsigned long nameTimer = 0;
int nameLetters[3] = {65, 65, 65};
int leterPosition = 0;
char highscoreName[3];

const byte matrixSize = 8;
bool matrixChanged = true;

int menuState = 0;
int settingsState = 0;
int gameState = 0;

byte matrix[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}
};

const int menuSize = 4;
char* menuChoices[] = {"START GAME  ", "HIGHSCORES  ", "DIFFICULTY", "SETTINGS", "RESET HIGH SCORE"};
const int settingsSize = 3;
char* settingsChoices[] = {"LCD-CONTRAST", "LCD-BRIGHTNESS", "MX-BRIGHTNESS", "VOLUME"};

struct highscore {
    char name[4];
    int score;
};

highscore highscores[6];

void setup() {
    lcd.begin(16, 2);

    Serial.begin(9600);

    pinMode(pinSW, INPUT_PULLUP);
    pinMode(backLightPin, OUTPUT);
    pinMode(contrastPin, OUTPUT);

    EEPROM.get(0, brightness);
    EEPROM.get(2, contrast);
    EEPROM.get(4, matrixBrightness);
    EEPROM.get(8, VolumePin);
    EEPROM.get(12, buzzerVolume);

    int j = 0;
    for (int i = 20; i <= 45; i = i + 5) {
        EEPROM.get(i, highscores[j].name);
        EEPROM.get(i + 3, highscores[j].score);
        j++;
    }

    // the zero refers to the MAX7219 number, it is zero for 1 chip
    lc.shutdown(0, false);  // turn off power saving, enables display
    lc.setIntensity(0, matrixBrightness);
    lc.clearDisplay(0);  // clear screen
    matrix[xPos][yPos] = 1;

    analogWrite(backLightPin, brightness);
    analogWrite(contrastPin, 50);
}

void loop() {
    if (gameState == 0) {
        if (menuState == 0) {
            resetScreen();

            navigateMenu();

            lcd.setCursor(1, 0);
            lcd.write(menuChoices[panel]);
            lcd.setCursor(1, 1);
            lcd.write(menuChoices[panel + 1]);

            lcd.setCursor(0, xMenu % 2);
            lcd.write(">");

        } else if (menuState == 1) {
            const int buzzTone = buzzerVolume * 500;
            const int buzzToneStart = buzzerVolume * 600;
            const int buzzTime = buzzerVolume * 200;
            const int buzzTimeStart = buzzerVolume *  300;
            resetScreen();

            if (startTimer == 0) {
                startTimer = millis();
            }

            lcd.setCursor(0, 0);
            lcd.write("GAME STARTS IN");
            lcd.setCursor(15, 0);
            if (millis() - startTimer >= 0 && time == '3') {
                tone(buzzerPin, buzzTone, buzzTime);
                noTone(VolumePin);
                lcd.write(time);
                time = '2';
            }
            if (millis() - startTimer >= 1000 && time == '2') {
                lcd.write(time);
                tone(buzzerPin, buzzTone, buzzTime);
                noTone(VolumePin);
                time = '1';
            }
            if (millis() - startTimer >= 2000 && time == '1') {
                lcd.write(time);
                tone(buzzerPin, buzzTone, buzzTime);
                noTone(VolumePin);
                time = '0';
            }
            if (millis() - startTimer >= 3000 && time == '0') {
                tone(buzzerPin, buzzToneStart, buzzTimeStart);
                noTone(VolumePin);
                gameState = 1;
                startTimer = 0;
                time = '3';
                lcd.clear();
            }

        } else if (menuState == 2) {
            resetScreen();

            navigateMenu();

            lcd.setCursor(1, 0);
            lcd.write(highscores[panel].name);
            char char_arr[100];
            unsigned long num = highscores[panel].score;
            sprintf(char_arr, "%d", num);
            lcd.setCursor(4, 0);
            lcd.write(": ");
            lcd.setCursor(6, 0);
            lcd.write(char_arr);

            lcd.setCursor(1, 1);
            lcd.write(highscores[panel + 1].name);
            char_arr[100];
            num = highscores[panel + 1].score;
            sprintf(char_arr, "%d", num);
            lcd.setCursor(4, 1);
            lcd.write(": ");
            lcd.setCursor(6, 1);
            lcd.write(char_arr);

            lcd.setCursor(0, xMenu % 2);
            lcd.write(">");

        } else if (menuState == 3) {
        } else if (menuState == 4) {
            if (settingsState == 0) {
                resetScreen();

                navigateMenu();

                lcd.setCursor(1, 0);
                lcd.write(settingsChoices[panel]);
                lcd.setCursor(1, 1);
                lcd.write(settingsChoices[panel + 1]);

                lcd.setCursor(0, xMenu % 2);
                lcd.write(">");

            } else if (settingsState == 1) {
                resetScreen();

                swReading = digitalRead(pinSW);
                lcd.setCursor(1, 0);
                lcd.write(" LED CONTRAST ");
                int contrastBars = map(contrast, 0, 150, 16, -1);
                for (int i = 0; i < contrastBars; i++) {
                    lcd.setCursor(i, 1);
                    lcd.write("O");
                }
                setBrightnessContrast();
                analogWrite(contrastPin, contrast);

            } else if (settingsState == 2) {
                resetScreen();

                swReading = digitalRead(pinSW);
                lcd.setCursor(0, 0);
                lcd.write(" LED BRIGHTNESS ");
                int brightnessBars = map(brightness, 0, 255, 16, -1);
                for (int i = 0; i < brightnessBars; i++) {
                    lcd.setCursor(i, 1);
                    lcd.write("O");
                }
                setBrightnessContrast();
                analogWrite(backLightPin, brightness);

            } else if (settingsState == 3) {
                resetScreen();

                swReading = digitalRead(pinSW);
                lcd.setCursor(0, 0);
                lcd.write(" MX BRIGHTNESS ");
                lightUpMatrix();
                int brightnessBars = map(matrixBrightness, 0, 15, 0, 17);
                for (int i = 0; i < brightnessBars; i++) {
                    lcd.setCursor(i, 1);
                    lcd.write("O");
                }
                setBrightnessContrast();
                lc.setIntensity(0, matrixBrightness);

            } else if (settingsState == 4) {
              resetScreen();

              lcd.setCursor(0, 0);
              if(buzzerVolume == 1) {
                  lcd.write(" MUTE VOLUME? ");
              } else if (buzzerVolume == 0) {
                  lcd.write(" UNMUTE VOLUME");
              }
              
              lcd.setCursor(3, 1);
              lcd.write("Yes");
              lcd.setCursor(10, 1);
              lcd.write("No");

              muteOrUnmuteVolume();
            }
        } else if (menuState == 5) {
            resetScreen();

            lcd.setCursor(0, 0);
            lcd.write(" ARE YOU SURE? ");
            lcd.setCursor(3, 1);
            lcd.write("Yes");
            lcd.setCursor(10, 1);
            lcd.write("No");

            selectChoice();
        }
    } else if (gameState == 1) {
        if (startTimer == 0) {
            startTimer = millis();
        }

        if (millis() - startTimer <= gameTime) {
            displayScoreTime();
        }

        // updateByteMatrix();
        if (millis() - lastMoved > moveInterval) {
            // game logic
            updatePositions();
            lastMoved = millis();
        }
        if (matrixChanged == true) {
            // matrix display logi
            updateMatrix();
            matrixChanged = false;
        }
        generateFood();

        if (millis() - startTimer >= gameTime) {
            lcd.clear();
            lc.clearDisplay(0);
            gameState = 2;
            startTimer = 0;
        }
    } else if (gameState == 2) {
        if (enterName() == 1) {
            gameState = 0;
            menuState = 0;

            for (int i = 0; i <= 2; i++) {
                highscores[5].name[i] = highscoreName[i];
            }
            highscores[5].score = score;

            sortHighscores(6);

            score = 0;
            for (int i = 0; i <= 2; i++) {
                highscoreName[i] = ' ';
            }

            int j = 0;
            for (int i = 20; i <= 45; i = i + 5) {
                EEPROM.put(i, highscores[j].name);
                EEPROM.put(i + 3, highscores[j].score);
                j++;
            }
        }
    }
    
}

void displayScoreTime() {
    lcd.setCursor(0, 0);
    lcd.write("SCORE:");
    lcd.setCursor(7, 0);
    char char_arr[100];
    unsigned long num = score;
    sprintf(char_arr, "%d", num);
    lcd.write(char_arr);
    lcd.setCursor(0, 1);
    lcd.write("TIME LEFT:");
    lcd.setCursor(11, 1);
    num = (gameTime - (millis() - startTimer)) / 1000;
    sprintf(char_arr, "%d", num);
    lcd.write(char_arr);
    if (millis() % 800 == 0) {
        lcd.clear();
    }
}

// theoretical example
void generateFood() {
    if (eaten == 0) {
        newFoodPosX = random() % 8;
        newFoodPosY = random() % 8;
        if (newFoodPosX == xPos && newFoodPosY == yPos) {
            newFoodPosX = (newFoodPosX + 4) % 8;
            newFoodPosY = (newFoodPosY - 3) % 8;
        }
        eaten = 1;
    }

    if (millis() - blink > 300 && blinked == 0) {
        matrix[newFoodPosX][newFoodPosY] = 1;
        updateMatrix();
        blink = millis();
        blinked = 1;
    }

    if (millis() - blink > 300 && blinked == 1) {
        matrix[newFoodPosX][newFoodPosY] = 0;
        updateMatrix();
        blink = millis();
        blinked = 0;
    }
}

void updateMatrix() {
    for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
            lc.setLed(0, row, col, matrix[row][col]);
        }
    }
}

void lightUpMatrix() {
    for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
            lc.setLed(0, row, col, 1);
        }
    }
}

void updatePositions() {
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);
    xLastPos = xPos;
    yLastPos = yPos;
    if (xValue < minThreshold) {
        if (xPos < matrixSize - 1) {
            xPos++;
        } else {
            xPos = 0;
        }
    }
    if (xValue > maxThreshold) {
        if (xPos > 0) {
            xPos--;
        } else {
            xPos = matrixSize - 1;
        }
    }
    if (yValue > maxThreshold) {
        if (yPos < matrixSize - 1) {
            yPos++;
        } else {
            yPos = 0;
        }
    }
    if (yValue < minThreshold) {
        if (yPos > 0) {
            yPos--;
        } else {
            yPos = matrixSize - 1;
        }
    }
    if (xPos != xLastPos || yPos != yLastPos) {
        matrixChanged = true;
        matrix[xLastPos][yLastPos] = 0;
        matrix[xPos][yPos] = 1;
        if (xPos == newFoodPosX && yPos == newFoodPosY) {
            eaten = 0;
            score = score + 10;
        }
    }
}

void navigateMenu() {
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);
    const int buzzToneScroll = buzzerVolume *  300;
    const int buzzToneSelect = buzzerVolume *  400;
    const int buzzTime = buzzerVolume * 100;
    if (xValue > maxThreshold && joyMoved == false && xMenu >= 1) {
        xMenu--;
        tone(buzzerPin, buzzToneScroll, buzzTime);
        noTone(VolumePin);
        if (xMenu % 2 == 1) {
            panel = panel - 2;
        }
        joyMoved = true;
        lcd.clear();
    }
    if (xValue < minThreshold && joyMoved == false && xMenu <= settingsSize - 1 && menuState == 4) {
        xMenu++;
        tone(buzzerPin, buzzToneScroll, buzzTime);
        noTone(VolumePin);
        if (xMenu % 2 == 0) {
            panel = panel + 2;
        }
        joyMoved = true;
        lcd.clear();
        
    }
    if (xValue < minThreshold && joyMoved == false && xMenu <= menuSize - 1 && menuState == 0) {
        xMenu++;
        tone(buzzerPin, buzzToneScroll, buzzTime);
        noTone(VolumePin);
        if (xMenu % 2 == 0) {
            panel = panel + 2;
        }
        joyMoved = true;
        lcd.clear();
    }
    if (xValue < minThreshold && joyMoved == false && xMenu <= 4 && menuState == 2) {
        xMenu++;
        tone(buzzerPin, buzzToneScroll, buzzTime);
        noTone(VolumePin);
        if (xMenu % 2 == 0) {
            panel = panel + 2;
        }
        joyMoved = true;
        lcd.clear();
    }
    if (yValue > maxThreshold && joyMoved == false) {
        tone(buzzerPin, buzzToneSelect, buzzTime);
        noTone(VolumePin);
        if (menuState == 4) {
            settingsState = xMenu + 1;
        } else if (menuState == 0) {
            menuState = xMenu + 1;
        }
        joyMoved = true;
        resetMenu();
    }
    if (yValue < minThreshold && joyMoved == false) {
        tone(buzzerPin, buzzToneSelect, buzzTime);
        noTone(VolumePin);
        if (menuState == 4 && settingsState == 0) {
            menuState = 0;
        } else if (settingsState != 0) {
            settingsState = 0;
        } else if (menuState != 0) {
            menuState = 0;
        }
        joyMoved = true;
        resetMenu();
    }
    

    if (xValue > resetMinThreshold && xValue < resetMaxThreshold && yValue > resetMinThreshold && yValue < resetMaxThreshold) {
        joyMoved = false;
    }
}

void resetMenu() {
    xMenu = 0;
    panel = 0;
    clear = 0;
}

void resetScreen() {
    if (clear == 0) {
        lcd.clear();
        clear = 1;
    }
}

void setBrightnessContrast() {
    int yValue = analogRead(yPin);
    const int buzzToneScroll = buzzerVolume *  300;
    const int buzzToneSelect = buzzerVolume *  400;
    const int buzzTime = buzzerVolume * 100;
    if (yValue > maxThreshold && joyMoved == false) {
        if (settingsState == 2 && brightness >= 30) {
            tone(buzzerPin, buzzToneScroll, buzzTime);
            noTone(VolumePin);
            brightness = brightness - 30;
            EEPROM.update(0, brightness);
        } else if (settingsState == 1 && contrast >= 15) {
            tone(buzzerPin, buzzToneScroll, buzzTime);
            noTone(VolumePin);
            contrast = contrast - 15;
            EEPROM.update(2, contrast);
        } else if (settingsState == 3 && matrixBrightness <= 14) {
            tone(buzzerPin, buzzToneScroll, buzzTime);
            noTone(VolumePin);
            matrixBrightness = matrixBrightness + 1;
            EEPROM.update(4, matrixBrightness);
        }
        joyMoved = true;
        lcd.clear();
    }
    if (yValue < minThreshold && joyMoved == false) {
        if (settingsState == 2 && brightness <= 233) {
            tone(buzzerPin, buzzToneScroll, buzzTime);
            noTone(VolumePin);
            brightness = brightness + 30;
            EEPROM.update(0, brightness);
        } else if (settingsState == 1 && contrast <= 135) {
            tone(buzzerPin, buzzToneScroll, buzzTime);
            noTone(VolumePin);
            contrast = contrast + 15;
            EEPROM.update(2, contrast);
        } else if (settingsState == 3 && matrixBrightness >= 1) {
            tone(buzzerPin, buzzToneScroll, buzzTime);
            noTone(VolumePin);
            matrixBrightness = matrixBrightness - 1;
            EEPROM.update(4, matrixBrightness);
        }
        joyMoved = true;
        lcd.clear();
    }
    if (yValue > resetMinThreshold && yValue < resetMaxThreshold) {
        joyMoved = false;
    }
    if (swReading == LOW && PressStart == 0) {
        PressTimer = millis();
        PressStart = 1;
    }
    if (millis() - PressTimer >= 100 && PressStart == 1) {
        tone(buzzerPin, buzzToneSelect, buzzTime);
        noTone(VolumePin);
        settingsState = 0;
        PressStart = 0;
        PressTimer = millis();
        resetMenu();
        lc.clearDisplay(0);
    }
    if (swReading != LOW) {
        PressTimer = millis();
        PressStart = 0;
    }
}

int enterName() {
    const int buzzToneScroll = buzzerVolume *  300;
    const int buzzTonescrollLetter = buzzerVolume *  350;
    const int buzzToneSelect = buzzerVolume *  400;
    const int buzzTime = buzzerVolume * 100;

    if (nameTimer == 0) {
        nameTimer = millis();
    }

    lcd.setCursor(0, 0);
    lcd.write("   ENTER NAME   ");
    lcd.setCursor(5, 1);
    lcd.write("<");
    lcd.setCursor(9, 1);
    lcd.write(">");

    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);
    swReading = digitalRead(pinSW);
    if (xValue > maxThreshold && joyMoved == false && nameLetters[leterPosition] <= 89) {
        tone(buzzerPin, buzzTonescrollLetter, buzzTime);
        noTone(VolumePin);
        nameLetters[leterPosition]++;
        joyMoved = true;
    }
    if (xValue < minThreshold && joyMoved == false && nameLetters[leterPosition] >= 66) {
        tone(buzzerPin, buzzTonescrollLetter, buzzTime);
        noTone(VolumePin);
        nameLetters[leterPosition]--;
        joyMoved = true;
    }
    if (yValue > maxThreshold && joyMoved == false && leterPosition <= 1) {
        tone(buzzerPin, buzzToneScroll, buzzTime);
        noTone(VolumePin);
        leterPosition++;
        joyMoved = true;
    }
    if (yValue < minThreshold && joyMoved == false && leterPosition >= 1) {
        tone(buzzerPin, buzzToneScroll, buzzTime);
        noTone(VolumePin);
        leterPosition--;
        joyMoved = true;
    }

    if (xValue > resetMinThreshold && xValue < resetMaxThreshold && yValue > resetMinThreshold && yValue < resetMaxThreshold) {
        joyMoved = false;
    }

    lcd.setCursor(6 + leterPosition, 1);
    if (millis() - nameTimer >= 300) {
        lcd.write(" ");
    }

    if (millis() - nameTimer >= 600) {
        lcd.setCursor(6, 1);
        lcd.write((char)(nameLetters[0]));
        lcd.setCursor(7, 1);
        lcd.write((char)(nameLetters[1]));
        lcd.setCursor(8, 1);
        lcd.write((char)(nameLetters[2]));
        nameTimer = millis();
    }

    if (swReading == LOW && PressStart == 0) {
        PressTimer = millis();
        PressStart = 1;
    }
    if (millis() - PressTimer >= 100 && PressStart == 1) {
        tone(buzzerPin, buzzToneSelect, buzzTime);
        noTone(VolumePin);
        lcd.clear();
        PressTimer = millis();
        PressStart = 0;
        highscoreName[0] = (char)(nameLetters[0]);
        highscoreName[1] = (char)(nameLetters[1]);
        highscoreName[2] = (char)(nameLetters[2]);
        return 1;
    }
    if (swReading != LOW) {
        PressTimer = millis();
        PressStart = 0;
    }

    return 0;
}

void sortHighscores(int len) {
    highscore temp;

    for (int i = 0; i < len - 1; i++) {
        for (int j = i + 1; j < len; j++) {
            if (highscores[i].score < highscores[j].score) {
                temp = highscores[i];
                highscores[i] = highscores[j];
                highscores[j] = temp;
            }
        }
    }
}

void deleteHighscores() {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 3; j++) {
            highscores[i].name[j] = 'A';
        }
        highscores[i].score = 0;
    }
    int j = 0;
    char cuv[3] = "AAA";
    for (int i = 20; i <= 45; i = i + 5) {
        EEPROM.put(i, cuv);
        EEPROM.put(i + 3, 0);
        j++;
    }
}

void selectChoice() {
    int yValue = analogRead(yPin);
    swReading = digitalRead(pinSW);
    const int buzzToneScroll = buzzerVolume *  300;
    const int buzzToneSelect = buzzerVolume *  400;
    const int buzzTime = buzzerVolume * 100;

    if (yValue > maxThreshold && joyMoved == false) {
        tone(buzzerPin, buzzToneScroll, buzzTime);
        noTone(VolumePin);
        deleteChoice = 0;
        lcd.clear();
        lcd.setCursor(9, 1);
        lcd.write(">");
        lcd.setCursor(12, 1);
        lcd.write("<");
        joyMoved = true;
    }
    if (yValue < minThreshold && joyMoved == false) {
        tone(buzzerPin, buzzToneScroll, buzzTime);
        noTone(VolumePin);
        deleteChoice = 1;
        lcd.clear();
        lcd.setCursor(2, 1);
        lcd.write(">");
        lcd.setCursor(6, 1);
        lcd.write("<");
        joyMoved = true;
    }

    if (yValue > resetMinThreshold && yValue < resetMaxThreshold) {
        joyMoved = false;
    }

    if (swReading == LOW && PressStart == 0) {
        PressTimer = millis();
        PressStart = 1;
    }
    if (millis() - PressTimer >= 100 && PressStart == 1) {
        tone(buzzerPin, buzzToneSelect, buzzTime);
        noTone(VolumePin);
        menuState = 0;
        PressTimer = millis();
        PressStart = 0;
        resetMenu();
        if (deleteChoice == 1) {
            deleteHighscores();
        }
    }
    if (swReading != LOW) {
        PressTimer = millis();
        PressStart = 0;
    }
}

void muteOrUnmuteVolume() {
    int yValue = analogRead(yPin);
    swReading = digitalRead(pinSW);
    const int buzzToneScroll = buzzerVolume *  300;
    const int buzzToneSelect = buzzerVolume *  400;
    const int buzzTime = buzzerVolume * 100;

    if (yValue > maxThreshold && joyMoved == false) {
        tone(buzzerPin, buzzToneScroll, buzzTime);
        noTone(VolumePin);
        deleteChoice = 0;
        lcd.clear();
        lcd.setCursor(9, 1);
        lcd.write(">");
        lcd.setCursor(12, 1);
        lcd.write("<");
        joyMoved = true;
    }
    if (yValue < minThreshold && joyMoved == false) {
        tone(buzzerPin, buzzToneScroll, buzzTime);
        noTone(VolumePin);
        deleteChoice = 1;
        lcd.clear();
        lcd.setCursor(2, 1);
        lcd.write(">");
        lcd.setCursor(6, 1);
        lcd.write("<");
        joyMoved = true;
    }

    if (yValue > resetMinThreshold && yValue < resetMaxThreshold) {
        joyMoved = false;
    }

    if (swReading == LOW && PressStart == 0) {
        PressTimer = millis();
        PressStart = 1;
    }
    if (millis() - PressTimer >= 100 && PressStart == 1) {
        tone(buzzerPin, buzzToneSelect, buzzTime);
        noTone(VolumePin);
        settingsState = 0;
        menuState = 4;
        PressTimer = millis();
        PressStart = 0;
        resetMenu();

        if(deleteChoice == 1 && buzzerVolume == 1) {
          VolumePin = 3;
          buzzerVolume = 0;
          EEPROM.update(8, VolumePin);
          EEPROM.update(12, buzzerVolume);
        } else if (deleteChoice == 1 && buzzerVolume == 0) {
          VolumePin = A4;
          buzzerVolume = 1;
          EEPROM.update(8, VolumePin);
          EEPROM.update(12, buzzerVolume);
        }
    }
    if (swReading != LOW) {
        PressTimer = millis();
        PressStart = 0;
    }
}