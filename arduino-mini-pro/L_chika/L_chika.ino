int led = 13; // blink 'digital' pin 1 - AKA the built in green LED
 
// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
 
}
 
// the loop routine runs over and over again forever:
void loop() {
    digitalWrite(led, HIGH); 
    delay(100);
    digitalWrite(led, LOW);
    delay(100);
}

