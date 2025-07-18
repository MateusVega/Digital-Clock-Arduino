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

void verify_buttons() {
  unsigned long now = millis();

  bool currentButtonUp = digitalRead(buttonUp);
  bool currentButtonDown = digitalRead(buttonDown);
  bool currentButtonOk = digitalRead(buttonOk);
  bool currentButtonLeft = digitalRead(buttonLeft);
  bool currentButtonRight = digitalRead(buttonRight);

  if (currentButtonUp == LOW && LastButtonUp == HIGH && now - lastDebounceUp > debounceDelay) {
    if (current_scene == TODO_KEYBOARD && selectionY > 0) {
      selectionY -= 1;
      scene_changed = true;
    } else if (current_scene == TODO && task_selection > 0) {
      task_selection -= 1;
      scene_changed = true;
    }
    timingOn = millis();
    lastDebounceUp = now;
  }

  if (currentButtonDown == LOW && LastButtonDown == HIGH && now - lastDebounceDown > debounceDelay) {
    int max = 0;
    if (selectionX > 0 && selectionX < 6) {max = 2;} else {max = 1;}
    if (current_scene == TODO_KEYBOARD && selectionY <= max) {
      selectionY += 1;
      scene_changed = true;
    } else if (current_scene == TODO && task_selection < taskCount - 1) {
      task_selection += 1;
      scene_changed = true;
    }
    timingOn = millis();
    lastDebounceDown = now;
  }

  if (currentButtonOk == LOW && LastButtonOk == HIGH && now - lastDebounceOk > debounceDelay) {
    if (issleeping) {
      issleeping = false;
      lcd.sleepDisplay(false);
    } else if (current_scene == LED) {
      change_led_state();
      scene_changed = true;
    } else if (current_scene == LASER) {
      change_laser_state();
      scene_changed = true;
    } else if (current_scene == THEME) {
      change_theme_state();
      scene_changed = true;
    } else if (current_scene == SLEEP) {
      issleeping = true;
      lcd.sleepDisplay(true);
    } else if (current_scene == TODO) {
      if (task_selection < 0 || task_selection >= MAX_TASKS) return;

      for (int i = task_selection; i < MAX_TASKS - 1; i++) {
        strcpy(tasks[i], tasks[i + 1]);
      }
      tasks[MAX_TASKS - 1][0] = '\0';
      taskCount--;
      scene_changed = true;
    } else if (current_scene == TODO_KEYBOARD) {
      if (selectionX == 3 && selectionY == 3 && text.length() < 9){
        text += " ";
      } else if (selectionX == 4 && selectionY == 3) {
        text.remove(text.length() - 1);
      } else if (selectionX == 5 && selectionY == 3) {
        if (text != "") {
          if (taskCount < MAX_TASKS) {
            strncpy(tasks[taskCount], text.c_str(), TASK_LENGTH - 1);
            tasks[taskCount][TASK_LENGTH - 1] = '\0';
            taskCount++;
            text = "";
            selectionX = 0;
            selectionY = 0;
            current_scene = TODO;
            scene_changed = true;
          } else {
            Serial.println("Não há mais espaço para tarefas.");
          }
        }
      } else if (text.length() < 9) {
        text += keyboard[selectionY][selectionX];
      }
      scene_changed = true;
    }
    timingOn = millis();
    lastDebounceOk = now;
  }

  if (currentButtonLeft == LOW && LastButtonLeft == HIGH && now - lastDebounceLeft > debounceDelay) {
    if ((current_scene > 0 && current_scene != TODO_KEYBOARD) || (current_scene == TODO_KEYBOARD && selectionX == 0)) {
      current_scene = (Scenes)(current_scene - 1);
      scene_changed = true;
    } else if (current_scene == TODO_KEYBOARD && selectionX > 0) {
      selectionX -= 1;
      scene_changed = true;
    }
    timingOn = millis();
    lastDebounceLeft = now;
  }

  if (currentButtonRight == LOW && LastButtonRight == HIGH && now - lastDebounceRight > debounceDelay) {
    if (current_scene < 6 && current_scene != TODO_KEYBOARD) {
      current_scene = (Scenes)(current_scene + 1);
      scene_changed = true;
    } else if (current_scene == TODO_KEYBOARD && selectionX < 7) {
      selectionX += 1;
      scene_changed = true;
    }
    timingOn = millis();
    lastDebounceRight = now;
  }

  LastButtonUp = currentButtonUp;
  LastButtonDown = currentButtonDown;
  LastButtonOk = currentButtonOk;
  LastButtonLeft = currentButtonLeft;
  LastButtonRight = currentButtonRight;
}

uint16_t getTextColor() {
  return white_theme ? BLACK : WHITE;
}

uint16_t getBgColor() {
  return white_theme ? WHITE : BLACK;
}

void change_led_state() {
  if (ledOn == false) {
    digitalWrite(ledPin, HIGH);
    ledOn = true;
  } else if (ledOn == true) {
    digitalWrite(ledPin, LOW);
    ledOn = false;
  }
}

void change_laser_state() {
  if (laserOn == false) {
    digitalWrite(laserPin, HIGH);
    laserOn = true;
  } else if (laserOn == true) {
    digitalWrite(laserPin, LOW);
    laserOn = false;
  }
}

void change_theme_state() {
  if (white_theme == false) {
    white_theme = true;
  } else if (white_theme == true) {
    white_theme = false;
  }
}

void clock_scene() {
  lcd.fillScreen(getBgColor());

  lcd.setCursor(10, 80);
  lcd.setTextColor(getTextColor());
  lcd.setTextSize(6);
  lcd.println("10:05");

  lcd.setCursor(25, 135);
  lcd.setTextColor(getTextColor());
  lcd.setTextSize(3);
  lcd.println("07/17/2025");
}

void led_scene() {
  lcd.fillScreen(getBgColor());

  lcd.setCursor(10, 80);
  lcd.setTextColor(getTextColor());
  lcd.setTextSize(6);
  lcd.println("LED");

  lcd.setCursor(25, 135);
  lcd.setTextSize(3);
  if (ledOn == false) {
    lcd.setTextColor(RED); 
    lcd.println("OFF");
  } else if (ledOn == true) {
    lcd.setTextColor(GREEN);
    lcd.println("ON");
  }
}

void laser_scene() {
  lcd.fillScreen(getBgColor());

  lcd.setCursor(10, 80);
  lcd.setTextColor(getTextColor());
  lcd.setTextSize(6);
  lcd.println("LASER");

  lcd.setCursor(25, 135);
  lcd.setTextSize(3);
  if (laserOn == false) {
    lcd.setTextColor(RED); 
    lcd.println("OFF");
  } else if (laserOn == true) {
    lcd.setTextColor(GREEN);
    lcd.println("ON");
  }
}

void theme_scene() {
  lcd.fillScreen(getBgColor());

  lcd.setCursor(10, 80);
  lcd.setTextColor(getTextColor());
  lcd.setTextSize(6);
  lcd.println("THEME");

  lcd.setCursor(25, 135);
  lcd.setTextSize(3);
  if (white_theme == false) {
    lcd.setTextColor(WHITE); 
    lcd.println("BLACK");
  } else if (white_theme == true) {
    lcd.setTextColor(BLACK);
    lcd.println("WHITE");
  }
}

void sleep_scene() {
  lcd.fillScreen(getBgColor());

  lcd.setCursor(10, 100);
  lcd.setTextColor(getTextColor());
  lcd.setTextSize(6);
  lcd.println("SLEEP!");

}

void todo_scene() {
  lcd.fillScreen(getBgColor());

  lcd.setCursor(10, 15);
  lcd.setTextColor(getTextColor());
  lcd.setTextSize(4);
  lcd.println("TO DO");

  if (taskCount > 0) {
    for (int i = 0; i < taskCount; i++) {
      lcd.setCursor(10, 75+30*i);
      if (i == task_selection) {
        lcd.setTextColor(RED);
      } else {
        lcd.setTextColor(getTextColor());
      }
      lcd.setTextSize(2);
      lcd.println(String(i+1) + ". " + String(tasks[i]));
    }
  } else {
      lcd.setCursor(10, 75);
      lcd.setTextColor(getTextColor());
      lcd.setTextSize(3);
      lcd.println("No Tasks");
  }
}

void todo_keyboard_scene() {
  lcd.fillScreen(getBgColor());

  lcd.setCursor(10, 15);
  lcd.setTextColor(getTextColor());
  lcd.setTextSize(4);
  lcd.println("TO DO:");
  
  lcd.setCursor(10, 70);
  lcd.setTextColor(getTextColor());
  lcd.setTextSize(3);
  lcd.println(text);

  lcd.setTextSize(2);
  lcd.setTextColor(getTextColor());

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 8; col++) {
      const char* label = keyboard[row][col];
      if (label[0] != '\0') {
        int x = startX + col * (keyWidth + spacing);
        int y = startY + row * (keyHeight + spacing);
        
        if (row == selectionY && col == selectionX) {
          lcd.drawRect(x, y, keyWidth, keyHeight, RED);
        } else {
          lcd.drawRect(x, y, keyWidth, keyHeight, getTextColor());
        }

        int textX = x + (keyWidth / 2) - (strlen(label) * 3);
        int textY = y + (keyHeight / 2) - 4;
        lcd.setCursor(textX, textY);
        lcd.print(label);
      }
    }
  }
}

void update_screen() {
  if (scene_changed == true) {
    lcd.clearScreen();
    if (current_scene == CLOCK) {
      clock_scene();
    } else if (current_scene == LED) {
      led_scene();
    } else if (current_scene == LASER) {
      laser_scene();
    } else if (current_scene == THEME) {
      theme_scene();
    } else if (current_scene == SLEEP) {
      sleep_scene();
    } else if (current_scene == TODO) {
      todo_scene();
    } else if (current_scene == TODO_KEYBOARD) {
      todo_keyboard_scene();
    }
    scene_changed = false;
  }
}
