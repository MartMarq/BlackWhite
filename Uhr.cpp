#include "Uhr.h"


RTC_DATA_ATTR int Welche = 3;
RTC_DATA_ATTR uint16_t s7 = 7000;
RTC_DATA_ATTR uint16_t s6 = 6000;
RTC_DATA_ATTR uint16_t s5 = 5000;
RTC_DATA_ATTR uint16_t s4 = 4000;
RTC_DATA_ATTR uint16_t s3 = 3000;
RTC_DATA_ATTR uint16_t s2 = 2000;
RTC_DATA_ATTR uint16_t s1 = 1000;

#define DARKMODE false
#define BLACK 0xFFFF

float PIx = 3.1415;
const uint8_t* aKKu[] = { xb0, xb1, xb2, xb3, xb4 };
int8_t batLev = 0;

int Uhr::LeseBild(int16_t x, int16_t y, const uint8_t* bitmap) {
    int16_t BildBreit = pgm_read_byte(bitmap);
    int16_t BreitB = (BildBreit + 7) / 8;
    return (pgm_read_byte(bitmap + 2 + y * BreitB + x / 8) & (128 >> (x & 7)));
}

void Uhr::drawWatchFace() { 


    display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
    display.setTextColor(DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);

  // reset step counter at midnight
    if (currentTime.Hour == 00 && currentTime.Minute == 00) {

      s7=s6;
      s6=s5;
      s5=s4;
      s4=s3;
      s3=s2;
      s2=s1;

        //turn off radios
        WiFi.mode(WIFI_OFF);
        btStop();
    }
    if (currentTime.Hour == 00 && currentTime.Minute == 01) {

         sensor.resetStepCounter();
        
    }

    //time adjustment start
    //if you don't need a time adjustment, just delete all lines between time adjustment start and end
    //my watchy runs too fast (15 seconds and 500 ms a day), this code set back time each day by 15 seconds and 500 ms

    if (currentTime.Hour == 00 && currentTime.Minute == 30) {

        RTC.read(currentTime);
        int8_t sekunde = currentTime.Second;
        int8_t minute = currentTime.Minute;
        int8_t hour = currentTime.Hour;
        int8_t day = currentTime.Day;
        int8_t month = currentTime.Month;
        int8_t year = tmYearToY2k(currentTime.Year);

        delay(15500);

        tmElements_t tm;
        tm.Month = month;
        tm.Day = day;
        tm.Year = y2kYearToTm(year);
        tm.Hour = hour;
        tm.Minute = minute;
        tm.Second = sekunde;

        RTC.set(tm);
    }
    // time adjustment end

    s1 = sensor.getCounter();
    float VBAT = getBatteryVoltage();
    uint32_t  Durch;
    Durch = (s7 + s6 + s5 + s4 + s3 + s2 + s1) / 7;
    if (VBAT > 4) {
        batLev = 4;
    }
    else if (VBAT > 3.80 && VBAT <= 4) {
        batLev = 3;
    }
    else if (VBAT > 3.60 && VBAT <= 3.80) {
        batLev = 2;
    }
    else if (VBAT > 3.40 && VBAT <= 3.60) {
        batLev = 1;
    }
    else if (VBAT <= 3.40) {
        batLev = 0;
    }
    
    if (Welche == 1) {
        display.setFont(&DSEG7_Classic_Bold_53);
        display.setCursor(5, 58);
        uint8_t displayHour;
        displayHour = currentTime.Hour;
        if (displayHour < 10) {
            display.print("0");
        }
        display.print(displayHour);
        display.print(":");
        if (currentTime.Minute < 10) {
            display.print("0");
        }
        display.println(currentTime.Minute);


        display.setFont(&FreeSansBold15pt7b);

        int16_t  x1, y1;
        uint16_t w, h;

        String TagA = dayShortStr(currentTime.Wday);
        int b = currentTime.Day;
        String TagC = monthShortStr(currentTime.Month);
        String Datum = TagA + " " + "  " + " " + TagC;
        display.getTextBounds(Datum, 0, 0, &x1, &y1, &w, &h);
        display.setCursor(100 - (w / 2), 90);
        display.println(TagA + " " + b + " " + TagC);


        uint8_t Protz = min<int>(100 - ((4.20 - VBAT) * 100), 100);
        Protz = max<int>(Protz, 0);
        display.drawBitmap(40, 100, akku, 30, 20, GxEPD_BLACK);
        display.setCursor(80, 120);
        display.print(Protz);
        display.print(" %");
        display.drawBitmap(40, 130, steps, 19, 23, GxEPD_BLACK);
        display.setCursor(80, 150);
        display.println(s1);
        display.drawBitmap(40, 160, durch, 30, 32, GxEPD_BLACK);
        display.setCursor(80, 185);
        display.println(Durch);
    }
    else
    if (Welche == 2){
    {
        display.setFont(&FreeSansBold15pt7b);
        
        display.setCursor(0, 25);
        display.print("today ");
        display.println(s1);
        display.setCursor(20, 49);
        display.print("t - 1  ");
        display.println(s2);
        display.setCursor(20, 73);
        display.print("t - 2  ");
        display.println(s3);
        display.setCursor(20, 97);
        display.print("t - 3  ");
        display.println(s4);
        display.setCursor(20, 121);
        display.print("t - 4  "); 
        display.println(s5);
        display.setCursor(20, 145);
        display.print("t - 5  "); 
        display.println(s6);
        display.setCursor(20, 169);
        display.print("t - 6  ");
        display.println(s7);
        display.writeFastHLine(0, 175, 200, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
        
        display.drawBitmap(30, 177, xdurch, 20, 21, GxEPD_BLACK);
        display.setCursor(90, 199);
        display.println(Durch);

    }
    }
    else
    if (Welche == 3){

    int8_t stundeA =  ((currentTime.Hour + 11) % 12) + 1;
    int minuteA =  currentTime.Minute;

    

    display.fillCircle(100, 100, 95, GxEPD_WHITE);

                
        int Winkel = ((stundeA  * 60 + minuteA) * 360  / 720) + 90 ;
        
        DrehBild( 100, 100, xmin, (minuteA+15)*6, GxEPD_BLACK);
        display.fillCircle(100, 100, 60, GxEPD_WHITE);
        DrehBild(100, 100, xstd, Winkel, GxEPD_BLACK);

        
        display.drawCircle(100, 100, 61, GxEPD_BLACK );
        display.drawCircle(100, 100, 60, GxEPD_BLACK );
        display.drawCircle(100, 100, 96, GxEPD_BLACK );
        display.drawCircle(100, 100, 95, GxEPD_BLACK );
        
        display.fillCircle(100, 100, 24, GxEPD_WHITE );
        display.drawCircle(100, 100, 25, GxEPD_BLACK );
        display.drawBitmap(75, 75, aKKu[batLev], 50, 50, GxEPD_BLACK);


    }
  
}

void Uhr::MaleBild(int16_t x, int16_t y, uint16_t Farb) {

    switch (Farb) {
    case BLACK:
        if (y & 1) {
            if (x & 1) {
                display.drawPixel(x, y, GxEPD_BLACK);
            }
            else {
                display.drawPixel(x, y, GxEPD_WHITE);
            }
        }
        else {
            if (x & 1) {
                display.drawPixel(x, y, GxEPD_WHITE);
            }
            else {
                display.drawPixel(x, y, GxEPD_BLACK);
            }
        }
        break;
    default:
        display.drawPixel(x, y, Farb);
        break;
    }
}
 
    void Uhr::DrehBild(int x2, int y2, const uint8_t * bitmap, unsigned int DrehWink, uint16_t Farbe = GxEPD_BLACK) {

        int Breit = pgm_read_byte(bitmap);
        int Hoch = pgm_read_byte(bitmap + 1);
        int Xa = Breit / 2;
        int Ya = Hoch;
        float Winkels = DrehWink * PIx / 180.0;

        int AnfaX = -Xa;
        int SchluX = AnfaX + Breit;
        int AnfaY = -Ya;
        int SchluY = AnfaY + Hoch;

        assert(Xa <= Ya);
        int startX = x2 - Ya;
        int startY = y2 - Ya;
        int endX = 200 - startX;
        int endY = 200 - startY;
        for (int x = 0; x < 200; x++) {
            for (int y = 0; y < 200; y++) {
                int jetztx = (x - x2) * cos(-Winkels) - (y - y2) * sin(-Winkels);
                int jetzty = (x - x2) * sin(-Winkels) + (y - y2) * cos(-Winkels);

                if (jetztx >= AnfaX && jetztx < SchluX && jetzty >= AnfaY && jetzty < SchluY) {
                    if (!LeseBild(jetztx + Xa, jetzty + Ya, bitmap)) {
                        MaleBild(x, y, Farbe);
                    }
                }
            }
        }
    }



void Uhr::handleButtonPress() {
  uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
  if (IS_DOUBLE_TAP) {
    while (!sensor.getINT()) {
      }
   }

if (IS_BTN_RIGHT_UP) {
    vibrate();
    Welche = Welche + 1;
    if (Welche > 3) {
      Welche = 1;
    }
    RTC.read(currentTime);
    drawWatchFace();
    showWatchFace(true);
  } else if (IS_BTN_RIGHT_DOWN) {
    vibrate();
    Welche = Welche - 1;
    if (Welche < 1) {
      Welche = 3;
    }
    RTC.read(currentTime);
    drawWatchFace();
    showWatchFace(true);
  } else {
    Watchy::handleButtonPress();
  }
}

void Uhr::vibrate() {
  
  sensor.enableFeature(BMA423_WAKEUP, false);
  pinMode(VIB_MOTOR_PIN, OUTPUT);
  
    delay(50);
    digitalWrite(VIB_MOTOR_PIN, true);
    delay(50);
    digitalWrite(VIB_MOTOR_PIN, false);
  
  sensor.enableFeature(BMA423_WAKEUP, true);
}
