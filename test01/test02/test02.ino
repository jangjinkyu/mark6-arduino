void setup() {
  Serial.begin(115200);
}

void loop() {
  if(Serial.available()){
    String msg = "";
    while(Serial.available()){
      msg +=char(Serial.read());
      delay(50);
    }
    Serial.print(msg);
  }
}
