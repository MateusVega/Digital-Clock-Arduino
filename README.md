# Digital Clock with Arduino

This project is a simple yet powerful interface built with an Arduino, designed for a 240x240 LCD screen. It combines a clock, LED/laser toggles, a task manager (to-do list), and light/dark theme switching — all controlled with just 5 buttons.

---

## 📸 Demo Preview

![Demo GIF]()

---

## 🧠 Features

- LED toggle scene
- Laser toggle scene
- To-do list with on-screen keyboard
- Theme switcher (dark/light mode)
- Auto sleep mode after inactivity
- Scene navigation using side buttons
  
---

## 🧰 Components Used

- Arduino UNO
- 240x240 ST7789 LCD display
- 5x Push buttons
- LED and Laser module
- Breadboard & jumper wires

---

## 🎨 Scenes Overview

- Clock Scene – Always visible time with smooth update
- LED Scene – Toggle an LED on/off
- Laser Scene – Toggle a laser module on/off
- Theme Scene – Switch between dark/light modes
- Sleep Scene – Activated after inactivity
- To-Do Scene – View, add, or remove tasks
- Keyboard Scene – Type using on-screen keyboard

---

## 📷 Breadboard View

![Breadboard Diagram]()

---

## 🗺️ Schematic View

![Schematic Diagram]()

---

## 💻 Code

```cpp
#define lcd_DC    9
#define lcd_RST   10 
#define SCR_WD   240
#define SCR_HT   240

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Arduino_ST7789_Fast.h>

Arduino_ST7789 lcd = Arduino_ST7789(lcd_DC, lcd_RST);

#define MAX_TASKS 10
#define TASK_LENGTH 11

char tasks[MAX_TASKS][TASK_LENGTH];
int taskCount = 0;

int task_selection = 0;

const char* keyboard[4][8] = {
  {"A", "B", "C", "D", "E", "F", "G", "H"},
  {"I", "J", "K", "L", "M", "N", "O", "P"},
  {"Q", "R", "S", "T", "U", "V", "W", "X"},
  {"", "Y", "Z", "_", "-", "=", "", ""}
};

String text = "";

int selectionX = 0;
int selectionY = 0;

const int keyWidth = 24;
const int keyHeight = 25;
const int spacing = 5;

const int totalWidth = 230;
const int totalHeight = 110;

const int startX = 8;
const int startY = 115;

enum Scenes {
  CLOCK,  // 0
  LED,  // 1
  LASER,  // 2
  THEME,  // 3
  SLEEP, // 4
  TODO,  // 5
  TODO_KEYBOARD  // 6
};

Scenes current_scene = CLOCK;

bool scene_changed = true;

bool white_theme = false;

int buttonUp = 3;
int buttonDown = 4;
int buttonOk = 5;
int buttonLeft = 6;
int buttonRight = 7;

int ledPin = 8;
int laserPin = 2;

bool ledOn = false;
bool laserOn = false;

bool LastButtonUp = HIGH;
bool LastButtonDown = HIGH;
bool LastButtonOk = HIGH;
bool LastButtonLeft = HIGH; 
bool LastButtonRight = HIGH;

unsigned long lastDebounceUp = 0;
unsigned long lastDebounceDown = 0;
unsigned long lastDebounceOk = 0;
unsigned long lastDebounceLeft = 0;
unsigned long lastDebounceRight = 0;

unsigned long debounceDelay = 200;

unsigned long timingOn = 0;
bool issleeping = false;

void setup() {
  Serial.begin(9600);
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonOk, INPUT_PULLUP);
  pinMode(buttonLeft, INPUT_PULLUP);
  pinMode(buttonRight, INPUT_PULLUP);

  pinMode(ledPin, OUTPUT);
  pinMode(laserPin, OUTPUT);

  lcd.init(SCR_WD, SCR_HT);
}

void loop() {
  if (millis() - timingOn > 40000 && issleeping == false) {
    issleeping = true;
    lcd.sleepDisplay(true);
  }
  update_screen();
  verify_buttons();
}

void verify_buttons() {...}

uint16_t getTextColor() {...}

uint16_t getBgColor() {...}

void change_led_state() {...}

void change_laser_state() {...}

void change_theme_state() {...}

void clock_scene() {...}

void theme_scene() {...}

void todo_scene() {...}

void todo_keyboard_scene() {...}

void update_screen() {...}
```
---

## 📜 License

MIT License – feel free to use, modify, and share. Credit is appreciated!

---

Fully written by **Mateus Vega (15y 🇧🇷)**
