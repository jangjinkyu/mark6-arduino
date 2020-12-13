#define GREEN 6
#define BLUE 3
#define RED 5

void red_on();
void green_on();
void blue_on();

int ptRMeter = A0;
int ptGMeter = A1;
int ptBMeter = A2;
int ptRValue = 0;
int ptGValue = 0;
int ptBValue = 0;

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(ptRMeter, INPUT);
  pinMode(ptGMeter, INPUT);
  pinMode(ptBMeter, INPUT);
  Serial.begin(9600);
  Serial.println("==Serial START==");
}

void loop() {
  ptRValue = analogRead(ptRMeter);
  ptGValue = analogRead(ptGMeter);
  ptBValue = analogRead(ptBMeter);
  analogWrite(RED,map(ptRValue, 0, 1023, 0, 255));
  analogWrite(GREEN,map(ptGValue, 0, 1023, 0, 255));
  analogWrite(BLUE,map(ptBValue, 0, 1023, 0, 255));
  delay(500);
}

void red_on(){
  
  analogWrite(RED,map(ptRValue, 0, 1023, 0, 255));
}
void red_off(){
  analogWrite(RED,0);
}
void green_on(){
  analogWrite(GREEN,map(ptGValue, 0, 1023, 0, 255));
}
void green_off(){
  analogWrite(GREEN,0);
}
void blue_on(){
  analogWrite(BLUE,map(ptBValue, 0, 1023, 0, 255));
}
void blue_off(){
  analogWrite(BLUE,0);
}
void all_on(){
  red_on();
  green_on();
  blue_on();
}
void all_off(){
  red_off();
  green_off();
  blue_off();
}
