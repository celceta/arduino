void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int in;
  int out;
  in = analogRead(0);
  out = in / 4;
  analogWrite(3, out);
  Serial.print(in);
  Serial.print(" -> ");
  Serial.println(out);
  delay(200);
}
