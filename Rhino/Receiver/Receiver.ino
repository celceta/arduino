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
//#define LoRa_Frequency (920000000)
#define LoRa_TxPower (14)

#define LED_PIN (13)
int led_status = LOW;

int seq = 0;

void setup(  )
{
  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("Init...");

  pinMode(LED_PIN, OUTPUT);

  LoRaRadio.begin(LoRa_Frequency);

  LoRaRadio.setFrequency(LoRa_Frequency);
  LoRaRadio.setTxPower(LoRa_TxPower);
  LoRaRadio.setBandwidth(LoRaRadio.BW_125);
  LoRaRadio.setSpreadingFactor(LoRaRadio.SF_10);
  LoRaRadio.setCodingRate(LoRaRadio.CR_4_5);
  LoRaRadio.setLnaBoost(true);
  LoRaRadio.receive(0);    // ★タイムアウト？

  Serial.println("Start...");
}

void loop(  )
{
  if(LoRaRadio.parsePacket() == sizeof info_packet){
    LoRaRadio.read((uint8_t *)&info_packet, sizeof info_packet);

    // アイキャッチの検証
    if(memcmp(&info_packet, "@344", 4) != 0){
      Serial.println("Bad header");
      return;
    }
  
    // チェックサムの検証
    uint16_t received_checksum = info_packet.checksum;
    if(received_checksum != calcCheckSum()){
      Serial.println("Bad checksum");
      return;
    }
    info_packet.checksum = received_checksum; // ダンプ用に書き戻す
    
    dump();

  // Lチカ
  digitalWrite(LED_PIN, led_status = led_status == LOW ? HIGH : LOW);
  }

  delay(100);
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
