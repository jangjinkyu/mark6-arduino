#include <WiFi.h>
#include "DHT.h"
#include <Wire.h>

#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

LiquidCrystal_I2C lcd(0x27,16,2); // LCD주소: 0x27 또는 0x3F
 
// Uncomment one of the lines below for whatever DHT sensor type you're using!
 
// Replace with your network credentials
const char* ssid = "ironfarm";
const char* password = "password3!";
//const char* ssid = "jc_home";
//const char* password = "password1!";
const char* mqtt_server = "192.168.0.7";
WiFiServer server(80);


int BUILTIN_LED = 2; 
// DHT Sensor
#define DHTPIN_1 26 //DHT11을 D4번핀에 연결한다.
#define DHTPIN_2 25 //DHT11을 D4번핀에 연결한다.
#define DHTTYPE DHT21 // DHT 11
// Initialize DHT sensor.
DHT dht1(DHTPIN_1, DHTTYPE);
DHT dht2(DHTPIN_2, DHTTYPE);

// Temporary variables
static char celsiusTemp1[7];
static char fahrenheitTemp1[7];
static char humidityTemp1[7];

static char celsiusTemp2[7];
static char fahrenheitTemp2[7];
static char humidityTemp2[7];
 
// Client variables
char linebuf[80];
int charcount=0;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
String packet;
float Humi1,Temp1;
float Humi2,Temp2;

void setup() {
  Serial.begin(115200);    
  setup_dht();
  setup_lcd();
  //setup_wifi();  
//  client.setServer(mqtt_server, 1883);
//  client.setCallback(callback);
}

void loop() {  
  Temp1 = getTemp1(); //온도를 받아서 변수에 입력
  Humi1 = getHumi1(); //습도를 받아서 변수에 입력
  delay(100);
  Temp2 = getTemp2(); //온도를 받아서 변수에 입력
  Humi2 = getHumi2(); //습도를 받아서 변수에 입력  
  delay(100);
  //mqtt_publish(Temp1, Humi1);// 온습도의 값을 함수에 넣어서 해당 값을 통신을 통해서 전송한다.
  //makeJson(Temp1, Humi1); //변환 함수 사용
  lcd_display();
  delay(100);
  //serverRes();
}

void setup_dht(){
  dht1.begin();
  dht2.begin();
  Serial.println(F("DHT Test begin"));
}


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void setup_lcd(){
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.clear(); 
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

float getHumi1() { //DHT11 습도를 받아오는 함수
  float h = dht1.readHumidity();  
  dtostrf(h, 6, 2, humidityTemp1);
  Serial.print("Humidity: ");
  Serial.print(humidityTemp1);
  Serial.println(" %\t");
  return(h);
}

float getTemp1() {//DHT11 온도를 받아오는 함수
  
  float h = dht1.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht1.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht1.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f))
  {
  Serial.println("Failed to read from DHT sensor!");
  }
    
  float hic = dht1.computeHeatIndex(t, h, false);
  dtostrf(hic, 6, 2, celsiusTemp1);
  float hif = dht1.computeHeatIndex(f, h);
  dtostrf(hif, 6, 2, fahrenheitTemp1);
  Serial.print("Temperature: ");
  Serial.print(celsiusTemp1);
  Serial.print(" *C ");

  return(hic);
}

float getHumi2() { //DHT11 습도를 받아오는 함수
  float h = dht2.readHumidity();  
  dtostrf(h, 6, 2, humidityTemp2);
  Serial.print("Humidity: ");
  Serial.print(humidityTemp2);
  Serial.println(" %\t");
  return(h);
}

float getTemp2() {//DHT11 온도를 받아오는 함수
  
  float h = dht2.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht2.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht2.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f))
  {
  Serial.println("Failed to read from DHT sensor!");
  }
    
  float hic = dht2.computeHeatIndex(t, h, false);
  dtostrf(hic, 6, 2, celsiusTemp2);
  float hif = dht2.computeHeatIndex(f, h);
  dtostrf(hif, 6, 2, fahrenheitTemp2);
  Serial.print("Temperature: ");
  Serial.print(celsiusTemp2);
  Serial.print(" *C ");

  return(hic);
}

void mqtt_publish(float Temp, float Humi){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

//    packet = "Temperature : " + String(Temp) + "*C " + "Humidity : " + String(Humi) + "%" ; 
//    //문자열과 숫자를 합친다.
//    packet.toCharArray(msg, 50); 
//    //mqtt publishing이 char형으로만 보낼 수 있기때문에 toCharArray로 변환한다.
//    Serial.print("Publish message: ");
//    Serial.println(msg);
    client.publish("sensor/Temp_Humi", msg);
  }
  delay(2000); //5초 단위로 Publishing (조정가능)
}

void lcd_display(){
  lcd.setCursor(0,0); // LCD Cursor 원점
  lcd.print("1"); // LCD에 "temp" 표시
  lcd.print(celsiusTemp1); // 온도값 LCD로 출력
  lcd.print("C"); // 온도 단위 표시
  lcd.print(":"); //LCD 2번째 줄에 "humidity:" 출력
  lcd.print(humidityTemp1); //습도값 LCD에 출력
  lcd.print("%"); //습도 단위 출력
  lcd.setCursor(0,1); //LCD 커서 줄바꿈
  lcd.print("2"); // LCD에 "temp" 표시
  lcd.print(celsiusTemp2); // 온도값 LCD로 출력
  lcd.print("C"); // 온도 단위 표시
  lcd.print(":"); //LCD 2번째 줄에 "humidity:" 출력
  lcd.print(humidityTemp2); //습도값 LCD에 출력
  lcd.print("%"); //습도 단위 출력
  lcd.println();

}

float makeJson(float temp, float humi) { //함수로 받아온 데이터값을 Json으로 변환 시킵니다.
  const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(3);
  DynamicJsonDocument doc(capacity);
  
  doc["Temp"] = celsiusTemp1;
  doc["Humi"] = humidityTemp2;
//  JsonArray data = doc.createNestedArray("data");
//  data.add(48.75608);
//  data.add(2.302038);

  Serial.print("Json data : ");
  serializeJson(doc, Serial);
  Serial.println();
  serializeJson(doc,msg,50);
}
 
void serverRes()
{
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client)
  {
    Serial.println("New client");
    memset(linebuf,0,sizeof(linebuf));
    charcount=0;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        //read char by char HTTP request
        linebuf[charcount]=c;
        if (charcount<sizeof(linebuf)-1) charcount++;
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {    
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close"); // the connection will be closed after completion of the response
          client.println();
          client.println("<!DOCTYPE HTML><html><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
          client.println("<meta http-equiv=\"refresh\" content=\"2\"></head>");
          client.println("<body><h1><p><span style=\"color: #169179;\">아이언팜 시스템 정보</span><p></h1>");
          client.print("<p><span style=\"color: #ba372a;\">온도: ");
          client.println(celsiusTemp1);
          client.println("*C</span></p>");
          //client.println(fahrenheitTemp);
          //client.println("*F</p><p>");
          client.print("<p><span style=\"color: #236fa1;\">습도: ");
          client.println(humidityTemp1);
          client.println("%</span></p>");
          client.println("</body></html>");
          break;
        }
        if (c == '\n')
        {
        // you're starting a new line
        currentLineIsBlank = true;
        memset(linebuf,0,sizeof(linebuf));
        charcount=0;
        }
        else if (c != '\r')
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
