
#include "DFRobot_EC.h"
#include "DFRobot_PH.h"
#include <EEPROM.h>
#include <OneWire.h>
#include <LiquidCrystal_I2C.h>


#define EC_PIN A1
#define PH_PIN A2
int DS18S20_Pin = 3;                              //온도센서를 2번 핀으로 연결
int DS18S20_Pin_4 = 4;

float evoltage,ecValue,etemperature = 25;
float pvoltage,phValue,ptemperature = 25;

DFRobot_EC ec;
DFRobot_PH ph;

OneWire ds(DS18S20_Pin);                          //3번 핀과 연결되 OneWire 객체 생성
OneWire ds2(DS18S20_Pin_4);

LiquidCrystal_I2C lcd(0x27,16,2); // LCD주소: 0x27 또는 0x3F

void setup_lcd(){
  //lcd.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.clear(); 
}

void setup()
{
  Serial.begin(115200);  
  setup_lcd();
  ec.begin();
  ph.begin();
}

void loop()
{
  getEc();
  delay(300);
  getPh();
//  float temperature = getTemp();//온도 측정 후 변수에 저장
//  Serial.print("temp: ");
//  Serial.println(temperature);
//  float temperature2 = getTemp2();//온도 측정 후 변수에 저장
//  Serial.print("temp2: ");
//  Serial.println(temperature2);
    lcd_display();
  delay(100);
}
float getTemp(){                                   //온도 측정 후 반환하는 함수
 byte data[12];
 byte addr[8];
 if ( !ds.search(addr)) {
   ds.reset_search();
   return -1000;
 }
 if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return -1000;
 }
 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return -1000;
 }
 ds.reset();
 ds.select(addr);
 ds.write(0x44,1);                                   
 byte present = ds.reset();
 ds.select(addr);  
 ds.write(0xBE); 
 
 for (int i = 0; i < 9; i++) { 
  data[i] = ds.read();                                                          
 }
 
 ds.reset_search(); 
 byte MSB = data[1];
 byte LSB = data[0];
 float tempRead = ((MSB << 8) | LSB); 
 float TemperatureSum = tempRead / 16; 
 return TemperatureSum;                                                                    
}

float getTemp2(){                                   //온도 측정 후 반환하는 함수
 byte data[12];
 byte addr[8];
 
 if ( !ds2.search(addr)) {
   ds2.reset_search();
   return -1000;
 }
 if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return -1000;
 }
 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return -1000;
 }
 ds2.reset();
 ds2.select(addr);
 ds2.write(0x44,1);                                   
 byte present = ds2.reset();
 ds2.select(addr);  
 ds2.write(0xBE); 
 
 for (int i = 0; i < 9; i++) { 
  data[i] = ds2.read();                                                          
 }
 
 ds2.reset_search(); 
 byte MSB = data[1];
 byte LSB = data[0];
 float tempRead = ((MSB << 8) | LSB); 
 float TemperatureSum = tempRead / 16; 
 return TemperatureSum;                                                                    
}

float getEc(){
  static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U)  //time interval: 1s
    {
      timepoint = millis();
      evoltage = analogRead(EC_PIN)/1024.0*5000;   // read the evoltage
      //temperature = readTemperature();          // read your temperature sensor to execute temperature compensation
      ecValue =  ec.readEC(evoltage,etemperature);  // convert evoltage to EC with temperature compensation
      Serial.print("etemperature:");
      Serial.print(etemperature,1);
      Serial.print("^C  EC:");
      Serial.print(ecValue,2);
      Serial.println("ms/cm");
    }
    ec.calibration(evoltage,etemperature);          // calibration process by Serail CMD
    return ecValue;
}

float getPh(){
      static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U){                  //time interval: 1s
        timepoint = millis();
        //temperature = readTemperature();         // read your temperature sensor to execute temperature compensation
       pvoltage = analogRead(PH_PIN)/1024.0*5000;  // read the voltage
        phValue = ph.readPH(pvoltage,ptemperature);  // convert voltage to pH with temperature compensation
        Serial.print("ptemperature:");
        Serial.print(ptemperature,1);
        Serial.print("^C  pH:");
        Serial.println(phValue,2);
    }
    ph.calibration(pvoltage,ptemperature);           // calibration process by Serail CMD
    return phValue;
}

void lcd_display(){
  
  lcd.setCursor(0,0); // LCD Cursor 원점
  lcd.print("T1:"); // LCD에 "temp" 표시
  lcd.print(getTemp()); // 온도값 LCD로 출력
  lcd.print("T2:"); // LCD에 "temp" 표시
  lcd.print(getTemp2()); // 온도값 LCD로 출력
  
  lcd.setCursor(0,1); // LCD Cursor 원점
  lcd.print("EC:"); // LCD에 "temp" 표시
  lcd.print(getEc()); // 온도값 LCD로 출력
  lcd.print("PH:"); // LCD에 "temp" 표시
  lcd.print(getPh()); // 온도값 LCD로 출력
}

float readTemperature()
{
  //add your code here to get the temperature from your temperature sensor
}
