/*
Проект измерения потока вода и температуры
посредством датчика YF-B7
*/

#include <LiquidCrystal.h>
#define B 3950 // B коэффициент NTC
#define RESISTOR 200000 // сопротивление резистора, 200 кОм
#define THERMISTOR 100000 // номинальное сопротивление термистора, 100 кОм
#define NOMINAL 25

unsigned char temp = A1; // пин входа датчика температуры
volatile int flow_freq; // импульсы с датчика
float vol = 0.0,l_minute;
unsigned char holl = 2; // пин входа датчика холла
unsigned long currentTime;
unsigned long cloopTime;


LiquidCrystal lcd(12, 11, 5, 4, 3, 9);
void flow () // функция обработки прерывания
{
   flow_freq++;
}
void setup()
{
   pinMode(temp, INPUT);
   pinMode(holl, INPUT);
   digitalWrite(holl, HIGH); // Optional Internal Pull-Up
   Serial.begin(9600);
   lcd.begin(16, 2);
   attachInterrupt(digitalPinToInterrupt(holl), flow, RISING); // Setup Interrupt
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Water Meter");
   lcd.setCursor(0,1);
   lcd.print("Circuit Digest");
   currentTime = millis();
   cloopTime = currentTime;
}
void loop ()
{
   currentTime = millis();
   int t = analogRead(temp);
   float tr = 1023.0/t-1;
   tr = RESISTOR/tr;
   Serial.print("t=");
   float temp_ntc;
   temp_ntc = tr / THERMISTOR;
   temp_ntc = log(temp_ntc);
   temp_ntc /= B;
   temp_ntc += 1.0 / (NOMINAL + 273.15);
   temp_ntc = 1.0 / temp_ntc;
   temp_ntc -= 273.15;
   Serial.print(tr);
   // скорость потока воды в литрах в минуту
   if(currentTime >= (cloopTime + 1000))
   {
    cloopTime = currentTime; 
    if(flow_freq != 0){
      l_minute = (flow_freq / 7.5);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Rate: ");
      lcd.print(l_minute);
      lcd.print(" L/M");
      l_minute = l_minute/60;
      vol = vol +l_minute;
      lcd.print("Vol:");
      lcd.print(vol);
      lcd.print(" L");      
      lcd.setCursor(0,1);
      lcd.print("T:");
      lcd.print(temp_ntc);
      flow_freq = 0; // сбрасываем счетчик
      Serial.print(l_minute, DEC);
      Serial.println(" L/Sec");
    }
    else {
      Serial.println(" flow rate = 0 ");
      lcd.clear();
      lcd.print("Rate: ");
      lcd.print( flow_freq);
      lcd.print(" L/M");
      lcd.setCursor(0,1);
      lcd.print("Vol:");
      lcd.print(vol);
      lcd.print(" L");      
      lcd.setCursor(0,0);
      lcd.print("T:");
      lcd.print(temp_ntc);
    }
   }
}