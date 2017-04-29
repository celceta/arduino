#define PIN_R (5)
#define PIN_G (9)
#define PIN_B (6)
#define RGB_STEP (8)


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
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

  changecolor();
  delay(50);
}


void changecolor()
{
  static int r =   0, rs = +RGB_STEP * 1;
  static int g = 255, gs = -RGB_STEP * 1;
  static int b = 127, bs = +RGB_STEP * 1;

  analogWrite(PIN_R, r);
  analogWrite(PIN_G, g);
  analogWrite(PIN_B, b);

  calc(&r, &rs);
  calc(&g, &gs);
  calc(&b, &bs);
}

void calc(int *n, int *ns)
{
  *n += *ns;
  if(*n < 1){
    *n = 0, *ns = -*ns;
    return;
  }
  if(*n > 255){
    *n = 255, *ns = -*ns;
    return;
  }
}

