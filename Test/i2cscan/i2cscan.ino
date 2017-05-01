#include <Wire.h> //I2C Arduino Library

int i,j;
void setup(){
  //Initialize Serial and I2C communications
  Serial.begin(9600);
  Wire.begin();
  j = 0;
  Serial.println("init");
}

void loop(){
  Wire.beginTransmission(j);
  Wire.write(0x00);
  i = Wire.endTransmission();
  if(i == 0){
    Serial.print(j,HEX);
    Serial.print(",");
    Serial.println(i);
  }
  j++;
  if(j > 0xFF){
    Serial.println("Over");
    while(1){
     delay(1000);
    } 
  }
  delay(25);
}
