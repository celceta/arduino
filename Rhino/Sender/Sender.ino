#include <MyRhino.h>
#include "LoRaRadio.h"

#include <TinyGPS++.h>

#define GPSSerial (Serial1)

#define EYECATCH ("@344")

#define LoRa_Frequency (LoRa_ARIB_Ch24)
#define LoRa_TxPower (14)

#define LED_PIN (13)
int led_status = LOW;

TinyGPSPlus gps;


int seq = 0x40;

void setup()
{
  Serial.begin(9600);
//  while (!Serial)
//    ;
  Serial.println("Serial:OK");


  GPSSerial.begin(9600);
  while (!GPSSerial)
    ;
  Serial.println("Serial No.3:OK");

  
  pinMode(LED_PIN, OUTPUT);

  LoRaRadio.begin(LoRa_Frequency);
  LoRaRadio.setTxPower(LoRa_TxPower);
  LoRaRadio.setBandwidth(LoRaRadio.BW_125);
  LoRaRadio.setSpreadingFactor(LoRaRadio.SF_10);
  LoRaRadio.setCodingRate(LoRaRadio.CR_4_5);
  LoRaRadio.setLnaBoost(true);
//  LoRaRadio.receive(5000);    // ★タイムアウト？

  Serial.println("Start...");
}

void loop(  )
{
  while(GPSSerial.available() > 0){
    if(!gps.encode(GPSSerial.read()))
      continue;
    if(!gps.location.isUpdated())
      continue;

    memset(&info_packet, sizeof info_packet, 0);  // クリア
    info_packet.eyecatch[0] = '@';                // アイキャッチ
    info_packet.eyecatch[1] = '3';
    info_packet.eyecatch[2] = '4';
    info_packet.eyecatch[3] = '4';
    if(++seq > 0x7f)
      seq = 0x41;
    info_packet.sequence    = seq;                    // シーケンス
    info_packet.satellites  = gps.satellites.value(); // 捕捉数
    info_packet.latitude    = gps.location.lat();     // 経度
    info_packet.longitude   = gps.location.lng();     // 緯度
    info_packet.altitude    = gps.altitude.meters();  // 高度
    info_packet.heading     = gps.course.deg();       // コース
    info_packet.speed       = gps.speed.kmph();       // 速度

    // UTC->JST
    int year    = gps.date.year();
    int month   = gps.date.month();
    int day     = gps.date.day();
    int hour    = gps.time.hour();
    int minute  = gps.time.minute();
    int second  = gps.time.second();
    utc2jst(&year, &month, &day, &hour, &minute, &second);
    
    info_packet.year2000  = year - 2000;  // 年
    info_packet.month     = month;        // 月
    info_packet.day       = day;          // 日
    info_packet.hour      = hour;         // 時
    info_packet.minute    = minute;       // 分
    info_packet.second    = second;       // 秒
    
    info_packet.terminator = '*';                 // 番兵
    info_packet.checksum = calcCheckSum();        // チェックサム

    // 送信前キャリアセンス
    CarrierSense();
   
    while(LoRaRadio.busy())
      ;
  
    unsigned long start = millis();
    LoRaRadio.beginPacket();
    LoRaRadio.write((uint8_t *)&info_packet, sizeof info_packet);
    LoRaRadio.endPacket();
    unsigned long finish = millis();
  
    Serial.print(finish - start);
    Serial.println("ms");

    // Lチカ
    digitalWrite(LED_PIN, led_status = led_status == LOW ? HIGH : LOW);
  
    dump();
    ToString();
  }




//  memset(&info_packet, sizeof info_packet, 0); // クリア
//
//  info_packet.eyecatch[0] = '@';      // アイキャッチ
//  info_packet.eyecatch[1] = '3';
//  info_packet.eyecatch[2] = '4';
//  info_packet.eyecatch[3] = '4';
//
//  if(++seq > 0x7f)
//    seq = 0x41;
//
//  info_packet.sequence = seq;       // シーケンス
//
//  info_packet.terminator = '*';       // 番兵
//
//  info_packet.checksum = calcCheckSum();  // チェックサム
//
//  // 送信前キャリアセンス
//  CarrierSense();
// 
//  while(LoRaRadio.busy())
//    ;
//
//  unsigned long start = millis();
//  LoRaRadio.beginPacket();
//  LoRaRadio.write((uint8_t *)&info_packet, sizeof info_packet);
//  LoRaRadio.endPacket();
//  unsigned long finish = millis();
//
//  Serial.print(finish - start);
//  Serial.println("ms");
//
//  // Lチカ
//  digitalWrite(LED_PIN, led_status = led_status == LOW ? HIGH : LOW);
//
//  dump();
//
//  delay(1000);
}


void utc2jst(int *year, int *month, int *day, int *hour, int *minute, int *second)
{
  *hour += 9; // UTC->JST

  // 繰り上がりの処理
  if(*hour >= 24){
    *hour %= 24;
    *day++;
    int lastday = getLastdayOfMonth(*year, *month);
    if(*day >= lastday + 1){
      *day %= lastday + 1;
      *month++;
      if(*month >= 12){
        *month %= 12;
        *year++;
      }
    }
  }
}


int getLastdayOfMonth(int year, int month)
{
  if(month == 2){
    return (year % 4 == 0 && year % 100 != 0 || year % 400 == 0) ? 29 : 28;
  }
  const int lastdays[] = { 31, -1, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  return lastdays[month - 1];
}
