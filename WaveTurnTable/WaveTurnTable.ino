#define BRIGHTNESS_IN   (1)           // analog pin
#define SPEEDVR_IN      (0)           // analog pin
#define REED_IN         (8)           // digital pin

#define LED_OUT         (13)          // digital pin
#define MOTOR_OUT       (9)           // pwm pin

#define THRESHOLD       (50)          // for brightness
#define LOOP_INTERVAL   (200)         // for busy wait(ms)
#define WAIT_FOR_BRIGHT (10 * 1000)   // exit dark environment time(ms)
#define WAIT_FOR_DARK   ( 2 * 1000)   // exit bright environment time(ms)

enum WAITING_STATUS { ROOM_BRIGHT, ROOM_DARK };
enum LED_STATUS { OFF, ON, FAST, SLOW };

int motor_speed = 0;


void setup() {
  Serial.begin(9600);
  pinMode(REED_IN, INPUT);
  pinMode(LED_OUT, OUTPUT);

  Status(OFF);
}

void loop() {
    TurnOn();                         // 回転開始
    Wait(ROOM_DARK);                  // 部屋が暗くなるのを待つ
    WaitInitialPositionAndTurnOff();  // 回転停止待ち
    Wait(ROOM_BRIGHT);                // 部屋が明るくなるのを待つ
}


void TurnOn()
{
  Serial.println("Turn On");
  //digitalWrite(STATUS_OUT, HIGH);
  motor_speed = GetMotorSpeed();;
  Status(ON);
}


void TurnOff()
{
  Serial.println("Turn Off");
  //digitalWrite(STATUS_OUT, LOW);
  motor_speed = 0;
  UpdateMotor();
}



void Wait(enum WAITING_STATUS wait)
{
  int brightness;
  int cnt = 0;

  for (;;) {
    brightness = analogRead(BRIGHTNESS_IN);
    Serial.println(brightness);

    switch(wait){
      case ROOM_DARK:
        if (brightness < THRESHOLD) {
          Status(FAST);
          cnt++;
          if(cnt >= (WAIT_FOR_DARK / LOOP_INTERVAL)){
            Serial.println("into the dark");
            return;
          }
        }
        motor_speed = GetMotorSpeed();
        UpdateMotor();
        break;
      case ROOM_BRIGHT:
        if (brightness >= THRESHOLD) {
          Status(FAST);
          cnt++;
          if(cnt >= (WAIT_FOR_BRIGHT / LOOP_INTERVAL)){
            Serial.println("into the bright");
            return;
          }
        }
    }
    delay(LOOP_INTERVAL);
  }
}


void WaitInitialPositionAndTurnOff()
{
  int reedsw = HIGH;

  for (;;) {
    reedsw = digitalRead(REED_IN);
    Serial.print("reed : ");
    Serial.println(reedsw);
    if (reedsw == HIGH) {
      TurnOff();
      return;
    }

    motor_speed = GetMotorSpeed();
    UpdateMotor();

    Status(SLOW);

    if(analogRead(BRIGHTNESS_IN) >= THRESHOLD){
      return;
    }
    
    delay(LOOP_INTERVAL);
  }
}


void UpdateMotor()
{
  analogWrite(MOTOR_OUT, motor_speed);
}



int GetMotorSpeed()
{
  return analogRead(SPEEDVR_IN) / 4;
}



void Status(enum LED_STATUS status)
{
  static int cnt = 0;
  static boolean sw = LOW;

  switch(status){
    case OFF:
      digitalWrite(LED_OUT, LOW);
      break;
    case ON:
      digitalWrite(LED_OUT, HIGH);
      break;
    case FAST:
      sw = !sw;
      digitalWrite(LED_OUT, sw);
      break;
    case SLOW:
      if(++cnt >= 3){
        sw = !sw;
        cnt = 0;
        digitalWrite(LED_OUT, sw);
      }
      break;
  }
}


