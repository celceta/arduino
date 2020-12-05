#include <MyRhino.h>
#include "LoRaRadio.h"

#define EYECATCH ("@344")

#define LoRa_Frequency (LoRa_ARIB_Ch24)
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
    ToString();

    // Lチカ
    digitalWrite(LED_PIN, led_status = led_status == LOW ? HIGH : LOW);
  }

  delay(100);
}
