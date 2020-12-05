#include <LoRaRadio.h>

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
};

extern struct INFO info_packet;


void dump();
uint16_t calcCheckSum();
void CarrierSense();
void ToString();
void dtostr(double value, int lenAll, int lenDecimal, char *buf);
void dtostr2(double value, char *bufInteger, int lenInteger, char *bufDecimal, int lenDecimal);
