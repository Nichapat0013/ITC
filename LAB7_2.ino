#include "DHT.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define DHT_TYPE DHT11
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define BUZZER_PIN 3
#define TEXT_SIZE 2
#define DHT_PIN 9
DHT dht(DHT_PIN, DHT_TYPE);

#define HOUR_ADDRESS 0
#define MINUTE_ADDRESS 1
#define HOUR_ALARM_ADDRESS 2
#define MINUTE_ALARM_ADDRESS 3 
#define PRESS 1
#define OFF 0
#define ON 1
#define ACTIVATE 1
#define NO_ACTIVATE 0
#define GO_TO_COUNT_TIME 2
#define PAUSE 0 
#define PLAY 1

int button_increase_hour = 8;
int button_increase_min = 7;
int button_alarm = 6;
int button_exit = 5;
int button_save_time = 4;
int timer_start_hour = 0;
int timer_start_minute = 0;
int timer_start_second = 0;
int timer_alarm_hour = 0;
int timer_alarm_minute = 0;
int timer_count_hour = 0;
int timer_count_minute = 0;
int timer_count_second = 0;
int alarm_activate = 0;
int off_alarm = 0;
int play_count_time = 0;
int x;
int y;
int z;

long bounce_time_delay = 150;
unsigned long last_button_increase_hour = 0;
unsigned long last_button_increase_min = 0;
unsigned long last_button_alarm = 0;
unsigned long last_button_exit = 0;
unsigned long last_button_increase_hour_alarm = 0;
unsigned long last_button_increase_min_alarm = 0;
unsigned long last_button_save_time = 0;
unsigned long last_time = 0;
unsigned long last_count_time = 0;

void setup() {
  Serial.begin(9600);
  OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  dht.begin();
  pinMode(button_increase_hour, INPUT);
  pinMode(button_increase_min, INPUT);
  pinMode(button_alarm, INPUT);
  pinMode(button_exit, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  timer_start_hour = EEPROM.read(HOUR_ADDRESS);
  timer_start_minute = EEPROM.read(MINUTE_ADDRESS);
  timer_alarm_hour = EEPROM.read(HOUR_ALARM_ADDRESS);
  timer_alarm_minute = EEPROM.read(MINUTE_ALARM_ADDRESS);
  
}

void loop() {
  Serial.begin(9600);
  x = analogRead(A0);  
  y = analogRead(A1);  
  z = analogRead(A2); 
  OLED.clearDisplay();
  write_time();
  write_temparature();
  write_humidity();
  write_alarm();
  write_count_time();
  second_time();
  run_count_time();
  check_time();
  check_count_time();
  clock_alert();
  reset_alarm();
  button_increase_number_hour_and_reset_count_time();
  button_increase_number_min();
  button_go_to_set_mode();
  button_exit_mode_and_stop_alarm();
  button_to_save_time_and_play_or_plause();
  if ((x > 340 && x < 420) && (y > 200 && y < 360) && (z > 320 && z < 410)) {
    OLED.setRotation(2);
  } else {
    OLED.setRotation(0);
  }
  OLED.display();
  delay(100);
}

void second_time() {
  if(millis() - last_time >= 1000){
    timer_start_second++;
    last_time = millis();
  }
}

void second_count_time() {
  if(millis() - last_count_time >= 1000){
    timer_count_second++;
    last_count_time = millis();
  }
}

void write_time() {
  if(alarm_activate == NO_ACTIVATE){
    OLED.setTextColor(WHITE);
    OLED.setCursor(30, 6);
    OLED.setTextSize(3);
    OLED.println(":");
    OLED.setCursor(72, 6);
    OLED.println(":");
      if(timer_start_hour < 10) {
        OLED.setCursor(5, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println("0");
        OLED.setCursor(18, 10);
        OLED.println(timer_start_hour);
        }
      if(timer_start_hour >= 10 && timer_start_hour < 24){
        OLED.setCursor(5, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println(timer_start_hour);
        }
      if(timer_start_minute < 10) {
        OLED.setCursor(47, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println("0");
        OLED.setCursor(60, 10);
        OLED.println(timer_start_minute);
        }
       if(timer_start_minute >= 10 && timer_start_minute < 60) {
        OLED.setCursor(47, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println(timer_start_minute);
        }
       if(timer_start_second < 10) {
        OLED.setCursor(90, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println("0");
        OLED.setCursor(103, 10);
        OLED.println(timer_start_second);
        }
       if(timer_start_second >= 10 && timer_start_second < 60) {
        OLED.setCursor(90, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println(timer_start_second);
        }
       if(timer_start_hour >= 10 && timer_start_hour < 24) {
        OLED.setCursor(5, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println(timer_start_hour); 
    }
  }
}

void write_alarm() {
  if(alarm_activate == ACTIVATE) {
    OLED.setCursor(0, 0);
    OLED.setTextSize(1);
    OLED.println("SET ALARM");
      if(timer_alarm_hour < 10) {
        OLED.setCursor(0, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println("0");
        OLED.setCursor(13, 10);
        OLED.println(timer_alarm_hour);
        OLED.setCursor(30, 10);
        OLED.print("hr");
      }
      if(timer_alarm_hour >= 10 && timer_alarm_hour < 24) {
        OLED.setCursor(0, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println(timer_alarm_hour);
        OLED.setCursor(30, 10);
        OLED.print("hr");
      }
      if(timer_alarm_minute < 10) {
        OLED.setCursor(60, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println("0");
        OLED.setCursor(73, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println(timer_alarm_minute);
        OLED.setCursor(90, 10);
        OLED.print("min");
      }
      if(timer_alarm_minute >= 10 && timer_alarm_minute < 60) {
        OLED.setCursor(60, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println(timer_alarm_minute);
        OLED.setCursor(90, 10);
        OLED.print("min"); 
    }
  }
}

void write_count_time() {
  if(alarm_activate == GO_TO_COUNT_TIME){
    OLED.setCursor(0, 0);
    OLED.setTextSize(1);
    OLED.println("COUNT TIME");
    OLED.setTextColor(WHITE);
    OLED.setCursor(30, 6);
    OLED.setTextSize(3);
    OLED.println(":");
    OLED.setCursor(72, 6);
    OLED.println(":");
      if(timer_count_hour < 10) {
        OLED.setCursor(5, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println("0");
        OLED.setCursor(18, 10);
        OLED.println(timer_count_hour);
        }
      if(timer_count_hour >= 10 && timer_count_hour < 24){
        OLED.setCursor(5, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println(timer_count_hour);
        }
      if(timer_count_minute < 10) {
        OLED.setCursor(47, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println("0");
        OLED.setCursor(60, 10);
        OLED.println(timer_count_minute);
        }
       if(timer_count_minute >= 10 && timer_count_minute < 60) {
        OLED.setCursor(47, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println(timer_count_minute);
        }
       if(timer_count_second < 10) {
        OLED.setCursor(90, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println("0");
        OLED.setCursor(103, 10);
        OLED.println(timer_count_second);
        }
       if(timer_count_second >= 10 && timer_count_second < 60) {
        OLED.setCursor(90, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println(timer_count_second);
        }
       if(timer_count_hour >= 10 && timer_count_hour < 24) {
        OLED.setCursor(5, 10);
        OLED.setTextSize(TEXT_SIZE);
        OLED.println(timer_count_hour); 
    }
  }
}

void write_temparature() {
  int T = dht.readTemperature();
  if(alarm_activate == NO_ACTIVATE){
    OLED.setTextColor(WHITE);
    OLED.setCursor(65, 0);
    OLED.setTextSize(1);
    OLED.print("TEMP: ");
    OLED.print(T);
    OLED.print(" C");
    OLED.fillCircle(115, 1, 1, WHITE);
  }
  if(alarm_activate == ACTIVATE) {
    OLED.setTextColor(WHITE);
    OLED.setCursor(65, 0);
    OLED.setTextSize(1);
    OLED.print("TEMP: ");
    OLED.print(T);
    OLED.print(" C");
    OLED.fillCircle(115, 1, 1, WHITE);
  }
  if(alarm_activate == GO_TO_COUNT_TIME) {
    OLED.setTextColor(WHITE);
    OLED.setCursor(65, 0);
    OLED.setTextSize(1);
    OLED.print("TEMP: ");
    OLED.print(T);
    OLED.print(" C");
    OLED.fillCircle(115, 1, 1, WHITE);
  }
}

void write_humidity() {
  int H = dht.readHumidity();
  if(alarm_activate == NO_ACTIVATE){
    OLED.setTextColor(WHITE);
    OLED.setCursor(0, 0);
    OLED.setTextSize(1);
    OLED.print("HUMI: ");
    OLED.print(H);
    OLED.print("%");
  }
}

void check_time() {
  if(timer_start_second == 60) {
    timer_start_minute++;
    timer_start_second = 0;
  }
  if(timer_start_minute == 60) {
    timer_start_hour++;
    timer_start_minute = 0;
  }
  if(timer_alarm_minute == 60) {
    timer_alarm_hour++;
    timer_alarm_minute = 0;
  }
  if(timer_start_hour == 24) {
    timer_start_hour = 0;
  } 
  if(timer_alarm_hour == 24) {
    timer_alarm_hour = 0;
  }
}

void check_count_time() {
  if(timer_count_second == 60) {
    timer_count_minute++;
    timer_count_second = 0;
  }
  if(timer_count_minute == 60) {
    timer_count_hour++;
    timer_count_minute = 0;
  }
  if(timer_count_hour == 24) {
    timer_count_hour = 0;
  } 
  if(timer_count_hour == 24) {
    timer_count_hour = 0;
  }
}

void run_count_time() {
  if(play_count_time == PLAY) {
    second_count_time();
  }
}

void reset_count_time() {
  timer_count_hour = 0;
  timer_count_minute = 0;
  timer_count_second = 0;
}

void button_increase_number_hour_and_reset_count_time() {
  int state = digitalRead(button_increase_hour);
  
  if (state == PRESS && alarm_activate == NO_ACTIVATE) {
    if(millis() - last_button_increase_hour > bounce_time_delay){
       timer_start_hour++;
       last_button_increase_hour = millis();
    }
  }
  if (state == PRESS && alarm_activate == ACTIVATE) {
    if(millis() - last_button_increase_hour_alarm > bounce_time_delay){
       timer_alarm_hour++;
       last_button_increase_hour_alarm = millis();
    }
  }
  if (state == PRESS && alarm_activate == GO_TO_COUNT_TIME) {
    reset_count_time();
    }
}

void button_increase_number_min() {
  int state = digitalRead(button_increase_min);
  
  if (state == PRESS && alarm_activate == NO_ACTIVATE) {
    if(millis() - last_button_increase_min > bounce_time_delay){
       timer_start_minute++;
       last_button_increase_min = millis();
    }
  }
  if (state == PRESS && alarm_activate == ACTIVATE) {
    if(millis() - last_button_increase_min_alarm > bounce_time_delay){
       timer_alarm_minute++;
       last_button_increase_min_alarm = millis();
    }
  }
}

void button_go_to_set_mode() {
  int state = digitalRead(button_alarm);

  if (state == PRESS && alarm_activate == NO_ACTIVATE) {
    if(millis() - last_button_alarm > bounce_time_delay){
       alarm_activate = ACTIVATE;
       last_button_alarm = millis();
    }
  } 
  if (state == PRESS && alarm_activate == ACTIVATE) {
    if(millis() - last_button_alarm > bounce_time_delay){
       alarm_activate = GO_TO_COUNT_TIME;
       last_button_alarm = millis();
    }
  }
  if (state == PRESS && alarm_activate == GO_TO_COUNT_TIME) {
    if(millis() - last_button_alarm > bounce_time_delay){
       alarm_activate = NO_ACTIVATE;
       last_button_alarm = millis();
    }
  }
}

void button_exit_mode_and_stop_alarm() {
  int state = digitalRead(button_exit);
  
  if (state == PRESS && alarm_activate == ACTIVATE) {
    if(millis() - last_button_exit > bounce_time_delay){
       alarm_activate = NO_ACTIVATE;
       last_button_exit = millis();
    }
  }
  if (state == PRESS && alarm_activate == GO_TO_COUNT_TIME) {
    if(millis() - last_button_exit > bounce_time_delay){
       alarm_activate = NO_ACTIVATE;
       last_button_exit = millis();
    }
  }
  
  if (state == PRESS && ((alarm_activate == ACTIVATE) || (alarm_activate == NO_ACTIVATE) || (alarm_activate == GO_TO_COUNT_TIME))) {
      off_alarm = ON;
  }
}

void button_to_save_time_and_play_or_plause() {
  int state = digitalRead(button_save_time);

  if (state == PRESS && alarm_activate == NO_ACTIVATE) {
    if(millis() - last_button_save_time > bounce_time_delay){
      save_time();
      last_button_save_time = millis();
    }
  }

  if (state == PRESS && alarm_activate == ACTIVATE) {
    if(millis() - last_button_save_time > bounce_time_delay){
      save_alarm_time();
      last_button_save_time = millis();
    }
  }

  if (state == PRESS && alarm_activate == GO_TO_COUNT_TIME && play_count_time == PAUSE) {
    if(millis() - last_button_save_time > bounce_time_delay){
      play_count_time = PLAY;
      second_count_time();
      last_button_save_time = millis();
    }
  }
  if (state == PRESS && alarm_activate == GO_TO_COUNT_TIME && play_count_time == PLAY) {
    if(millis() - last_button_save_time > bounce_time_delay){
      play_count_time = PAUSE;
      second_count_time();
      last_button_save_time = millis();
    }
  }  
}

void speaker () {
  tone(BUZZER_PIN, 261);
  delay(100);
  noTone(BUZZER_PIN);
}

void clock_alert() {
  if(timer_start_hour == timer_alarm_hour && timer_start_minute == timer_alarm_minute ){ 
    if(off_alarm == OFF){
      speaker();
    }
  }
}

void reset_alarm() {
  if (timer_start_hour != timer_alarm_hour || timer_start_minute != timer_alarm_minute) {
    off_alarm = OFF;
  }
}

void save_time(){
  EEPROM.put(HOUR_ADDRESS, timer_start_hour);
  EEPROM.put(MINUTE_ADDRESS, timer_start_minute);
}

void save_alarm_time () {
  EEPROM.put(HOUR_ALARM_ADDRESS, timer_alarm_hour);
  EEPROM.put(MINUTE_ALARM_ADDRESS, timer_alarm_minute);
}
