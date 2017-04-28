/*　GR-KURUMI Test Program　*/

#include <RLduino78.h>
#define LED_R 22
#define LED_G 23
#define LED_B 24
#define LED_OFF 1
#define LED_ON 0


void setup()
{
    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);
}

void loop()
{
    digitalWrite(LED_R, LED_ON);
    delay(100);
    digitalWrite(LED_R, LED_OFF);
    delay(1000);    
    digitalWrite(LED_G, LED_ON);
    delay(100);
    digitalWrite(LED_G, LED_OFF);
    delay(1000);    
    digitalWrite(LED_B, LED_ON);
    delay(100);
    digitalWrite(LED_B, LED_OFF);
    delay(1000);    
}

