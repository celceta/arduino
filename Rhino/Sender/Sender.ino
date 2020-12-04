#include "LoRaRadio.h"

#define LoRa_ARIB_Ch24 (920600000)
#define LoRa_ARIB_Ch25 (920800000)
#define LoRa_ARIB_Ch26 (921000000)
#define LoRa_ARIB_Ch27 (921200000)
#define LoRa_ARIB_Ch28 (921400000)
#define LoRa_ARIB_Ch29 (921600000)
#define LoRa_ARIB_Ch30 (921800000)
#define LoRa_ARIB_Ch31 (922000000)
#define LoRa_ARIB_Ch32 (922200000)
#define LoRa_ARIB_Ch33 (922400000)
#define LoRa_ARIB_Ch34 (922600000)
#define LoRa_ARIB_Ch35 (922800000)
#define LoRa_ARIB_Ch36 (923000000)
#define LoRa_ARIB_Ch37 (923200000)
#define LoRa_ARIB_Ch38 (923400000)

#define EYECATCH ("@344")

struct INFO {
  byte eyecatch[4];
  uint16_t sequence;
  byte satellites;
  double latitude, longitude;
  double altitude;  // meters
  byte heading;     // 0-359
  double speed;     // km/h
  byte year2000, month, day;
  byte hour, minute, second;
  uint16_t checksum;
  char terminator;
} info_packet;

#define LoRa_Frequency (LoRa_ARIB_Ch24)
#define LoRa_TxPower (14)

#define LED_PIN (13)
int led_status = LOW;

int seq = 0x40;

void setup(  )
{
  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("Init...");

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
  memset(&info_packet, sizeof info_packet, 0); // クリア

  info_packet.eyecatch[0] = '@';      // アイキャッチ
  info_packet.eyecatch[1] = '3';
  info_packet.eyecatch[2] = '4';
  info_packet.eyecatch[3] = '4';

  if(++seq > 0x7f)
    seq = 0x41;

  info_packet.sequence = seq;       // シーケンス

  info_packet.terminator = '*';       // 番兵

  info_packet.checksum = calcCheckSum();  // チェックサム

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

  delay(1000);
}

void dump()
{
  char ch;
  for (int i = 0; i < sizeof info_packet; i++) {
    ch = ((char *)&info_packet)[i];
    if (ch == 0x00) {
      ch = '_';
    } else if (!isprint(ch)) {
      ch = '.';
    }
    Serial.print(ch);
  }
  Serial.print(" : ");
  Serial.print(sizeof info_packet);
  Serial.println("bytes");
}

uint16_t calcCheckSum()
{
  uint16_t checksum = 0;
  info_packet.checksum = 0;

  byte *p = (byte *)&info_packet;
  for(int i = 0; i < sizeof info_packet; i++){
    checksum += *p++ + (i * 7);
  }
  return checksum;
}

void CarrierSense()
{
  unsigned long limit;

  for(;;){
    // 時限の算出
    limit = millis() + 5;
    if(limit < millis())
      continue;           // ulongの上限を超えた時は振り出しへ
  
    // Channel Activity Detection
    while(millis() < limit){
      if(LoRaRadio.cadDetected())
        continue;         // 混信中なら振り出しへ
    }
    break;
  }
}
