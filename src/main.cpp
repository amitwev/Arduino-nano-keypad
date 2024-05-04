#include <Arduino.h> // Include Arduino.h for Arduino-specific functions and types
#include <SPI.h>
#include "ST7567_FB.h"
#include "c64enh_font.h"
#include <DS1302RTC.h>
#include <TimeLib.h>


#include <Keyboard.h>
// #include <Mouse.h>

#define LCD_BACKLIGHT 6
#define LCD_CS 10
#define LCD_DC 8
#define LCD_RST 9
#define RTC_RST A4
#define RTC_DAT A3
#define RTC_CLK A2
#define KEYBOARD_PLAY 3
// #define KEY_MEDIA_PLAY_PAUSE 0xCD // Example key code for play/pause

DS1302RTC RTC(RTC_CLK, RTC_DAT, RTC_RST); // CLK, DAT, RST

ST7567_FB lcd(LCD_DC, LCD_RST, LCD_CS);

void AutoSetBuildTime() {
  char input[] = __TIME__ " " __DATE__;  //Input
  String H, M, S, D, N, Y;               // Output
  const char del[] = " :,";              //Delimiters
  char *token;
  int i = 0;
  char *array[6];  // Number of variables to save
  int Hi, Mi, Si, Di, Ni, Yi;

  token = strtok(input, del);
  while (token != NULL)  //splitted
  {
    array[i++] = token;
    token = strtok(NULL, del);
  }
  H = array[0];
  Hi = H.toInt();
  M = array[1];
  Mi = M.toInt();
  S = array[2];
  Si = S.toInt();
  D = array[4];
  Di = D.toInt();
  N = array[3];  // month to number
  if (N == "Jan")
    Ni = 1;
  else if (N == "Feb")
    Ni = 2;
  else if (N == "Mar")
    Ni = 3;
  else if (N == "Apr")
    Ni = 4;
  else if (N == "May")
    Ni = 5;
  else if (N == "Jun")
    Ni = 6;
  else if (N == "Jul")
    Ni = 7;
  else if (N == "Aug")
    Ni = 8;
  else if (N == "Sep")
    Ni = 9;
  else if (N == "Oct")
    Ni = 10;
  else if (N == "Nov")
    Ni = 11;
  else if (N = "Dec")
    Ni = 12;
  Y = array[5];
  Yi = Y.toInt();
  setTime(Hi, Mi, Si, Di, Ni, Yi);
  RTC.set(now());
}

void setup() {
  Serial.begin(9600);
  //keyboard
  pinMode(KEYBOARD_PLAY, INPUT_PULLUP);
  Keyboard.begin();
  //
  pinMode(LCD_BACKLIGHT, OUTPUT);
  digitalWrite(LCD_BACKLIGHT, LOW);

  lcd.init();
  lcd.cls();

  lcd.setFont(c64enh);

  // Reset RTC module
  pinMode(RTC_RST, OUTPUT);
  digitalWrite(RTC_RST, LOW);
  delay(100);
  digitalWrite(RTC_RST, HIGH);
  delay(100);

  bool rtchaltRTC = RTC.haltRTC();
  Serial.print("rtc.haltRTC: ");
  Serial.println(rtchaltRTC);

  // Initialize RTC
  if (!rtchaltRTC) {
    lcd.printStr(ALIGN_CENTER, 10, "Clock stopped!");
    AutoSetBuildTime();
    RTC.haltRTC(false);
  } else {
    lcd.printStr(ALIGN_CENTER, 10, "Clock working!");
  }

  delay(500);

  if (RTC.writeEN()){
    lcd.printStr(ALIGN_CENTER, 10, "Write allowed.");
  }
  else{
    lcd.printStr(ALIGN_CENTER, 10, "Write protected.");
  }

  delay(2000);

  lcd.cls();
  lcd.printStr(ALIGN_CENTER, 10, "RTC sync!");
  setSyncProvider(RTC.get);

  if (timeStatus() == timeSet){
    lcd.printStr(ALIGN_CENTER, 10, " Ok!");
    Serial.print("time status is OK");
  }
  else{
    lcd.printStr(ALIGN_CENTER, 10, " FAIL!");
    Serial.print("Time status is not OK"); 
  }
  
  delay(2000);

  // lcd.drawRectD(0, 0, 128, 64, 1);
  // lcd.display();
}

void loop() {
  static int sday = 0;  // Saved day number for change check

  // DateTime now = rtc.now();

  // Serial.print("Hour: ");
  // Serial.println(hour());
  // Serial.print("Minute: ");
  // Serial.println(minute());
  // Serial.print("Second: ");
  // Serial.println(second());
  
  char timeStr[16];
  sprintf(timeStr, "%02d:%02d:%02d", hour(), minute(), second());

  char dateStr[16];
  sprintf(dateStr, "%02d/%02d/%04d", day(), month(), year());

  lcd.cls();
  lcd.printStr(ALIGN_CENTER, 10, timeStr);
  lcd.printStr(ALIGN_CENTER, 20, dateStr);


  //
  // Serial.print("day: ");
  // Serial.println(day());
  // Serial.print("Month: ");
  // Serial.println(month());
  // Serial.print("year: ");
  // Serial.println(year());

  //
  int playState = digitalRead(KEYBOARD_PLAY);
  if(playState == LOW){
    Serial.println("Button pressed!");
    lcd.printStr(ALIGN_CENTER, 30, "Key PLAY YES press!");
    // Keyboard.write('z');
    // Keyboard.press(KEY_MEDIA_PLAY_PAUSE);
    delay(50); // Debounce the button
    // Keyboard.releaseAll(); // Release all keys
  } else{
    lcd.printStr(ALIGN_CENTER, 30, "key PLAY NOT press!");
  }

  lcd.display();
  delay(50);
  // // Print current time to LCD
  // char timeStr[16];
  // sprintf(timeStr, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  // Serial.print("Time String: ");
  // Serial.println(timeStr);

  // lcd.cls();
  // lcd.printStr(ALIGN_CENTER, 10, timeStr);
  // lcd.display();

  // delay(1000); // Update once per second
}
