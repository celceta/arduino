#include <MyRhino.h>

struct INFO info_packet;

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



void ToString()
{
//	char lat1[3 + 1], lat2[10 + 1];
//	char lng1[3 + 1], lng2[10 + 1];
//	char alt1[4 + 1], alt2[ 2 + 1];

	char lat[20 + 1], lng[20 + 1], alt[6 + 1];

	dtostr(info_packet.latitude , sizeof lat, 17, lat);	// 緯度
	dtostr(info_packet.longitude, sizeof lng, 16, lng);	// 経度
	dtostr(info_packet.altitude , sizeof alt,  2, alt);	// 高度

	char buf[200];
	sprintf(buf, "[%2d] %d/%2d/%2d %2d:%02d:%02d lat:%s lng:%s alt:%s",
		info_packet.satellites,
		info_packet.year2000 + 2000,
		info_packet.month,
		info_packet.day,
		info_packet.hour,
		info_packet.minute,
		info_packet.second,
		lat,
		lng,
		alt
	);
	Serial.println(buf);

//	Serial.print(lat);
//	Serial.print(" ");
//	Serial.print(lng);
//	Serial.print(" ");
//	Serial.println(alt);
}




void dtostr(double value, int lenTotal, int lenDecimal, char *buf)
{
	int i;
	int lenInteger = lenTotal  - 1 - lenDecimal - 1;				// 整数部桁数 ← 全体桁数 - 符号(1) - 小数点(1) - 整数部桁数 - NUL(1)
	byte minus = 0;

	if(value < 0){			// 符号処理
		minus = 1;
		value = abs(value);	
	}

	int intInteger = value;					// 整数部を取り出す

	value -= intInteger;					// 整数部を0に
	for(i = 0; i < lenDecimal; i++){
		value *= 10;						// 小数部をシフトして整数化
	}
	long intDecimal = value;				// 小数部を取り出す

//	Serial.print(intInteger);
//	Serial.print(".");
//	Serial.println(intDecimal);

	char fmt[15];
	sprintf(fmt, "%%%dd.%%0%dd", lenInteger, lenDecimal);		// 書式の作成
	snprintf(buf, lenTotal, fmt, intInteger, intDecimal);		// 変換

	if(minus){													// マイナし符号
		for(i = lenInteger - 1; i > 0; i--){
			if(buf[i] == ' '){
				break;
			}
		}
		buf[i] = '-';
	}
}
