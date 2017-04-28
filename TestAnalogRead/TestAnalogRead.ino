void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val;
  val = analogRead(0);
  analogWrite(3, val);
  Serial.println(val);
  delay(200);
}
