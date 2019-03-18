#include <stdio.h>
#include <SPI.h>

#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>

// ピンアサイン
#define sclk (12)
#define mosi (11)
#define cs   (10)
#define dc   ( 9)
#define rst  ( 8)  // you can also connect this to the Arduino reset

int OUT_LED             = 13;

// アナログ入力
const int IN_VOLT             =    0; // 電圧測定ピン（analog）
const int IN_VOLT_RESOLUTION  = 1024; // 電圧測定分解能
const int IN_VOLT_MAX         =  5.0; // 計測電圧の最大値（分圧後）
const float voltWarn          =  1.5; // 警告閾値
int warning                   =    0; // 警告モード

// グラフ
const int   graphWidth    = 128;        // グラフ幅（x軸）
const int   graphHeight   = 118;        // グラフ高（y軸）
const float graphHi       =   3; //15;  // グラフの最大値（y軸）
const float graphLo       =   1; //11;  // グラフの最小値（y軸）

// 間隔
int sleepInterval = 100;      // 描画間隔（ミリ秒）
int graphInterval =   10;     // グラフ粒度（何回のループでグラフを更新するか）

float    *history = NULL;     // グラフ値
uint16_t *history_ts = NULL;  // 時刻
int      current = 0;         // グラフ値（現在の添字）

// 電圧の平均値を求めるための変数
float sum    = 0;   // 合計
int hist_cnt = 0;   // 標本数

// 色
#define COLOR_TEXT    (ST7735_WHITE)  // 標準
#define COLOR_VOLT    (ST7735_RED)    // 電圧
#define COLOR_BG      (ST7735_BLACK)  // 背景
#define COLOR_SCALE   (0x75EE)        // 軸
#define COLOR_UPTIME  (ST7735_CYAN)   // 起動時間

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);

uint16_t startup = 0;         // 起動した時間(仮)

void setup(void) {
  Serial.begin(115200);

  // メモリ確保
  history    = (float    *)calloc(graphWidth, sizeof(*history));
  history_ts = (uint16_t *)calloc(graphWidth, sizeof(*history_ts));

  // 初期化
  startup = millis();         // 起動した時間
  pinMode(IN_VOLT, INPUT);
  pinMode(OUT_LED, OUTPUT);
  tft.initR(INITR_BLACKTAB);
  tft.invertDisplay(0);
  tft.fillScreen(COLOR_BG);
}

void loop() {
  static int loop_cnt = 0;

  digitalWrite(OUT_LED, 1);
  char buf[100];
  
  // 起動時間
  drawUptime((millis() - startup) / 1000);

  // 電圧
  float volt = getVoltage();
  drawVolt(volt);
  sum += volt;

  warning = (volt < voltWarn);

  // グラフの描画
  if(++hist_cnt >= graphInterval){
    drawGraph(0);   // 直前の値を黒塗り
    saveValue();    // 現在の電圧を記録
    drawScale();    // スケールを描画
    drawGraph(1);   // 直近の値を再描画
    drawTimeSpan(); // グラフの時間幅を描画
    hist_cnt = 0;
    sum = 0;
  }

  digitalWrite(OUT_LED, 0);

  if(warning){
    tft.invertDisplay(loop_cnt % 2);
  }
  else{
    tft.invertDisplay(0);
  }
  loop_cnt++;

  delay(sleepInterval);
}


// 電圧の読み取りとデコード
float getVoltage()
{
  return analogRead(IN_VOLT) * IN_VOLT_MAX / (float)IN_VOLT_RESOLUTION;
}


void drawGraph(int sw)
{
  int n = current;
  int x, y;
  
  for(x = 0; x < graphWidth; x++){
    y = calcY(history[n++]);
    if(n >= graphWidth)
      n = 0;

    if(y >= 0)
      tft.drawPixel(x, y, sw ? ST7735_WHITE : ST7735_BLACK);
  }
}

int calcY(float value)
{
  float y;                          // 戻り値

  // 描画範囲外
  if(value > graphHi || value < graphLo){
    return -1;
  }

  value = value - graphLo;
  y =  value * graphHeight / (graphHi - graphLo);

  return 160 - y - 10;
}


char *getTimestamp(uint16_t timestamp)
{
  static char buf[30];
  sprintf(buf, "%3dh %2dm %2ds", timestamp / 3600, timestamp / 60 % 60, timestamp % 60);
  return buf;
}

void drawScale()
{
  int y;

  tft.setTextSize(1);
  tft.setTextColor(COLOR_SCALE);

  // 上限
  y = calcY(graphHi);
  tft.drawFastHLine(0, y, graphWidth, COLOR_SCALE);
  tft.setCursor(0, y + 2);
  tft.print(graphHi); tft.print("V");

  // 中間
  float middle = (graphHi + graphLo) / 2.;
  y = calcY(middle);
  tft.drawFastHLine(0, y, graphWidth, COLOR_SCALE);
  tft.setCursor(0, y + 2);
  tft.print(middle); tft.print("V");

  // 下限
  y = calcY(graphLo);
  tft.drawFastHLine(0, y, graphWidth, COLOR_SCALE);
  tft.setCursor(0, y - 8);
  tft.print(graphLo); tft.print("V");
}


void saveValue()
{
  history_ts[current] = millis();     // 現在時刻
  history[current] = sum / hist_cnt;  // 平均値
  if(++current >= graphWidth)
    current = 0;
}


void drawTimeSpan()
{
  tft.setCursor(0, 19 * 8);
  tft.setTextColor(COLOR_SCALE, COLOR_BG);
  tft.print("<---");
  tft.print(getTimestamp((millis() - history_ts[current])  / 1000));
  tft.print(" --->");
}


void drawUptime(int uptime)
{
  tft.setCursor(0, 0);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_UPTIME, COLOR_BG);
  tft.print(" UPTIME:"); tft.print(getTimestamp(uptime));
}


void drawVolt(float volt)
{
  char buf[30];
  dtostrf(volt, 5, 2, buf); strcat(buf, "V");
  tft.setCursor(14, 8);
  tft.setTextSize(3);
  tft.setTextColor(COLOR_VOLT, COLOR_BG);
  tft.print(buf);
}

