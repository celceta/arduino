const int BRIGHTNESS_IN = 0;
const int THRESHOLD = 30;

const int STATUS_OUT = 8;

struct PINPAIR {
  int MAGSW_IN;   /* リードスイッチが接続されたピン */
  int RELAY_OUT;  /* リレーが接続されたピン */
};
struct PINPAIR pins[] = {
  { 3, 2 },
  { 5, 4 },
  { 7, 6 },
};
const int NUM_OF_PINS = 1;

enum WAITING_STATUS { BRIGHTNESS, DARKNESS };

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_OF_PINS; i++) {
    pinMode(pins[i].MAGSW_IN , INPUT);
    pinMode(pins[i].RELAY_OUT, OUTPUT);
  }
  pinMode(STATUS_OUT, OUTPUT);
}

void loop()
{
  for (;;) {
    int i;
    for (i = 1; i <= NUM_OF_PINS; i++)
      TurnOn(i);
    Wait(DARKNESS);
    for (i = 1; i <= NUM_OF_PINS; i++)
      WaitInitialPositionAndTurnOff(i);
    Wait(BRIGHTNESS);
  }
}


void TurnOn(int num)
{
  Serial.print("Turn On : ");
  Serial.println(num);
  digitalWrite(STATUS_OUT, HIGH);
  digitalWrite(pins[num - 1].RELAY_OUT, HIGH);
}


void TurnOff(int num)
{
  Serial.print("Turn Off : ");
  Serial.println(num);
  digitalWrite(STATUS_OUT, LOW);
  digitalWrite(pins[num - 1].RELAY_OUT, LOW);
}


void Wait(enum WAITING_STATUS wait)
{
  int brightness;

  for (;;) {
    brightness = analogRead(BRIGHTNESS_IN);
    Serial.println(brightness);

    switch(wait){
      case DARKNESS:
        if (brightness < THRESHOLD) {
          Serial.println("into the dark");
          return;
        }
        break;
      case BRIGHTNESS:
        if (brightness >= THRESHOLD) {
          Serial.println("into the bright");
          return;
        }
    }
    delay(250);
  }
}


void WaitInitialPositionAndTurnOff(int num)
{
  int leadsw;
  int sw = HIGH;

  for (;;) {
    leadsw = digitalRead(pins[num - 1].MAGSW_IN);
    Serial.println(leadsw);
    if (leadsw == HIGH) {
      TurnOff(num);
      return;
    }
    digitalWrite(STATUS_OUT, sw);
    sw = (sw == HIGH ? LOW : HIGH);
    delay(500);
  }
}

