/* 
 *  This sketch run on ESP32 with Arduino core for ESP32,
 *  demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pins depending on the request,
 *  to control the brightness of RGB LED connected to:
 *    27 : BLUE
 *    26 : GREEN
 *    25 : RED
 *    
 *    http://server_ip/rgb/rrggbb/
 *    where rr is the value set RED
 *    where gg is the value set GREEN
 *    where bb is the value set BLUE
 *    then terminate with '/'
 *  server_ip is the IP address of the ESP32, will be 
 *  printed to Serial when the module is connected.
*/
#include <WiFi.h>
#include <Wire.h>
#include <BH1750.h>
#include <LiquidCrystal_I2C.h>

BH1750 lightMeter;

LiquidCrystal_I2C lcd(0x27,16,2); // LCD주소: 0x27 또는 0x3F

const char* ssid = "ironfarm";
const char* password = "password3!";
//const char* ssid = "jc_home";
//const char* password = "cjk!@#0501";

WiFiServer server(80);

#define MAX_LED_VALUE 99
// use first 3 channels of 16 channels (started from zero)
#define LEDC_CHANNEL_0_R  0
#define LEDC_CHANNEL_1_G  1
#define LEDC_CHANNEL_2_B  2

// use 13 bit precission for LEDC timer
#define LEDC_TIMER_13_BIT  13

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     5000

// LED PINs
#define LED_PIN_R   25
#define LED_PIN_G   27
#define LED_PIN_B   26

int ptRMeter = 32;
int ptGMeter = 33;
int ptBMeter = 34;
int ptRValue = 0;
int ptGValue = 0;
int ptBValue = 0;
int ctRValue = 0;
int ctGValue = 0;
int ctBValue = 0;
float lux = 0.0;

uint32_t min(uint32_t num1, uint32_t num2){
  if(num1 < num2){
    return num1;
  }else{
    return num2;
  }
}

// Arduino like analogWrite
// value has to be between 0 and valueMax
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t rvalue, uint32_t valueMax = MAX_LED_VALUE) {
  // calculate duty
  Serial.print("rduty : ");
  Serial.print(rvalue);
  Serial.print(" : ");
  Serial.println(map(rvalue, 0, 4095, 0, LEDC_BASE_FREQ));
  uint32_t rduty = map(rvalue, 0, 4095, 0, LEDC_BASE_FREQ);
  uint32_t duty = (LEDC_BASE_FREQ / valueMax) * min(value, valueMax);
  Serial.print("duty: ");
  Serial.println(duty);
  // write duty to LEDC
  ledcWrite(channel, duty); 
}

void anlcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = MAX_LED_VALUE) {
  // calculate duty
  uint32_t duty = map(value, 0, 4095, 0, LEDC_BASE_FREQ);
  Serial.print("rduty : ");
  Serial.print(value);
  Serial.print(" : ");
  Serial.println(duty);  
  ledcWrite(channel, duty);
}

void setup_ligthMeter(){    
  lightMeter.begin();
  //lightMeter.begin(BH1750_ONE_TIME_HIGH_RES_MODE_2);//분해능이 0.5 lux
  Serial.println(F("BH1750 Test begin"));
}

void setup_lcd(){
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.clear(); 
}

void getlux(){
  
  Wire.beginTransmission(0x23); // 슬레이브주소 1번 전송시작
  Wire.write(0);
  Wire.endTransmission();
  lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");  
  //lux = lightMeter.readLightLevel();  
}


void lcd_display(){
  Wire.beginTransmission(0x27); // 슬레이브주소 1번 전송시작
  Wire.write(0);
  Wire.endTransmission();
  
  lcd.setCursor(0,0); // LCD Cursor 원점
  lcd.print("Light:"); // LCD에 "temp" 표시
  lcd.print(lux); // 온도값 LCD로 출력
  lcd.print(" lx"); // 온도 단위 표시
  lcd.println();
//  lcd.print("TEMP:"); // LCD에 "temp" 표시
//  lcd.print(celsiusTemp); // 온도값 LCD로 출력
//  lcd.print(" *C"); // 온도 단위 표시
//  lcd.setCursor(0,1); //LCD 커서 줄바꿈
//  lcd.print("HUMIDITY:"); //LCD 2번째 줄에 "humidity:" 출력
//  lcd.print(humidityTemp); //습도값 LCD에 출력
//  lcd.print("%"); //습도 단위 출력
//  lcd.println();
}

void setup() {
  Serial.begin(115200);
//  Wire.begin();
//  setup_ligthMeter();
//  setup_lcd();
//
//  pinMode(ptRMeter,INPUT);
//  pinMode(ptBMeter,INPUT);
//  analogReadResolution(12);
  
  // Setup timer and attach timer to a led pins
  ledcSetup(LEDC_CHANNEL_0_R, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN_R, LEDC_CHANNEL_0_R);
  ledcSetup(LEDC_CHANNEL_1_G, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN_G, LEDC_CHANNEL_1_G);
  ledcSetup(LEDC_CHANNEL_2_B, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN_B, LEDC_CHANNEL_2_B);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
  delay(1000);
}

void loop() {
  ptRValue = analogRead(ptRMeter);
  ptGValue = analogRead(ptGMeter);
  ptBValue = analogRead(ptBMeter);

  if(ctRValue <= 0 ){
    anlcAnalogWrite(LEDC_CHANNEL_0_R, ptRValue);
  }
  if(ctGValue <= 0 ){
    anlcAnalogWrite(LEDC_CHANNEL_1_G, ptGValue);
  }
  if(ctBValue <= 0 ){
    anlcAnalogWrite(LEDC_CHANNEL_2_B, ptBValue);
  }
//  
//  if(ptRValue != analogRead(ptRMeter)){
//      
//  }
//  if(ptGValue != analogRead(ptGMeter)){
//      
//  }
//  if(ptBValue != analogRead(ptBMeter)){
//      
//  }  
//getlux();
//delay(100);
//lcd_display();
delay(1000);
//  Serial.print("RED: ");
//  Serial.print(ptRValue);
//  Serial.print("GREEN: ");
//  Serial.print(ptGValue);
//  Serial.print("BLUE: ");
//  Serial.println(ptBValue);
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int valR, valG, valB;
  String subStringR, subStringG, subStringB;
  int index = req.indexOf("/rgb/");
  if(index != -1){
    if(req.charAt(index+11)=='/'){
      subStringR = req.substring(index+5, index+7);
      subStringG = req.substring(index+7, index+9);
      subStringB = req.substring(index+9, index+11);
      Serial.println("R: " + subStringR);
      Serial.println("G: " + subStringG);
      Serial.println("B: " + subStringB);
  
      valR = subStringR.toInt();
      valG = subStringG.toInt();
      valB = subStringB.toInt();
      
      ctRValue = valR;
      ctGValue = valG;
      ctBValue = valB;

      Serial.println("valR: " + String(valR));
      Serial.println("valG: " + String(valG));
      Serial.println("valB: " + String(valB));
   
    }
    else{
      Serial.println("Not terminated with /");
      client.stop();
      return;
    }
  }
  else {
    Serial.println("No /rgb/ found");
    client.stop();
    return;
  }

  // Set GPIOs according to the request
  // No check valid of the requested setting
  ledcAnalogWrite(LEDC_CHANNEL_0_R, valR, ptRValue);
  ledcAnalogWrite(LEDC_CHANNEL_1_G, valG, ptGValue);
  ledcAnalogWrite(LEDC_CHANNEL_2_B, valB, ptBValue);
  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIOs of RGB is now ";
  s += String(valR) +":" + String(valG) + ":" + String(valB);
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  Serial.println("Client disonnected");
  delay(500);
  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}
