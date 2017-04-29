#define PHOTO_IN (0) // analog pin

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  int brightness;

  brightness = analogRead(PHOTO_IN);

  Serial.print("brightness : ");
  Serial.println(brightness);
  delay(100);
}

