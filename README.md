# Introduction to Robotics (2022 - 2023)

Introduction to Robotics laboratory homeworks, taken in the 3rd year at the Faculty of Mathematics and Computer Science, University of Bucharest. 
Each homework includes requirements, implementation details, code and image files.

## Homework 0

Create a GitHub repository with a suitable description.

## Homework 1

Use a separat potentiometer in controlling each of the color of the RGB led (Red, Green and Blue). The control must be done with digital electronics(aka you must read the value of the potentiometer with Arduino, and write a mapped value to each of the pins connected to the led). In order to complete this task I used 3 potentiometers, 1 RGB led, 1 bread board, 3 resistors (220 Ohms), 1 Arduino UNO board and connectivity wires. I implemented the code used to control the led in Arduino IDE 2.

The code, image and video showcasing the RGB led being controlled are provided in the Homework1 file (in case you don,t want to download the video i have provided a youtube link: https://youtu.be/ouFXPSiqGA0).

![image](https://user-images.githubusercontent.com/73032808/198399772-9b0410f8-1966-418c-9673-1a74f16d3940.jpg)

## Homework 2

Building the traffic lights for a crosswalk. You will use 2 LEDs to represent the traffic lights for people (red and green), 3 LEDs to represent the traffic lights for cars (red, yellow and green) and 1 buzzer for visually impaired people.

The system has the following states:

1. **State 1**(default, reinstated after state 4 ends): green light for cars, red light for people, no sounds. Duration:  indefinite, changed bypressing the button.

2. **State 2**(initiated by counting down 8 seconds after a button press): the light should be yellow for cars, red for people and no sounds. Duration: 3 seconds.

3. **State 3**(initiated after state 2 ends): red for cars, green for peopleand a beeping sound from the buzzer at a constant interval. Duration: 8 seconds.

4. **State 4**(initiated after state 3 ends): red for cars,blinking greenfor people and a beeping sound from the buzzer, at a constant in-terval, faster than the beeping in state 3. This state should last 4seconds.

Pressing the button in any state other than state 1 should NOT yield any actions.

The code, image and video showcasing the traffic light simulation are provided in the Homework2 file (in case you don,t want to download the video i have provided a youtube link: https://youtube.com/shorts/NqkNPd55kYg).

![e0e0e3af-a857-4441-a6c8-04ea7edfbb6a](https://user-images.githubusercontent.com/73032808/199785659-5a2c6bf2-a29e-4671-a4db-343906ebf0fb.jpg)

