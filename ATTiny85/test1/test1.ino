void setup() {
  // put your setup code here, to run once:
  pinMode(1, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int current;
  current = analogRead(1);
  digitalWrite(1, HIGH);
  delay(current);
  digitalWrite(1, LOW);
  delay(current );
}
