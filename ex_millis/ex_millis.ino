int iDta;
int *iPt1;
int **iPt2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  iDta = 123;
  TrcVal();
  *iPt1 = 234;
  TrcVal();
  **iPt2 = 345;
  TrcVal();
}

void TrcVal(){
  int val, adr, val1, val2;
  val = iDta;
  adr = &iDta;
  String str = "iDta : lvalue = "+String(adr)+", rvalue = "+String(val);
  Serial.println(str);
  iPt1 = &iDta;
  val = iPt1;
  adr = &iPt1;
  val1 = *iPt1;
  str = "iPt1 : lvalue = "+String(adr)+", rvalue = "+String(val) + "=> "+String(val1);
  Serial.println(str);
  iPt2 = &iPt1;
  val = iPt2;
  adr = &iPt2;
  val1 = *iPt2;
  val2 = **iPt2;
  str = "iPt2 : lvalue = "+String(adr)+", rvalue = "+String(val) + "=> "+String(val1)+ "=> "+String(val2);
  Serial.println(str);
  Serial.println("");
}
void loop() {
 
}                
