#include <SPI.h>

#define clock_pin 13
#define data_pin 11
#define latch_pin 2
 int row_scan = 0;
int this_ms;
int last_ms=0;
byte scan[8];
byte level[8];
int height[32];
int prev_height[32];
String char_height;
char char_ht[200];
void setup() {
  // put your setup code here, to run once:
  SPI.setBitOrder(MSBFIRST);
SPI.setDataMode(SPI_MODE0);
SPI.setClockDivider(SPI_CLOCK_DIV2);
pinMode(latch_pin, OUTPUT);
pinMode(data_pin, OUTPUT);
pinMode(clock_pin, OUTPUT);
SPI.begin();
Serial.begin(115200);
randomSeed(analogRead(A0));
scan[0] = B00000001;
scan[1] = B00000010;
scan[2] = B00000100;
scan[3] = B00001000;
scan[4] = B00010000;
scan[5] = B00100000;
scan[6] = B01000000;
scan[7] = B10000000;

level[0] = B00000001;
level[1] = B00000011;
level[2] = B00000111;
level[3] = B00001111;
level[4] = B00011111;
level[5] = B00111111;
level[6] = B01111111;
level[7] = B11111111;
for(int i = 0;i < 6 ;i++)
SPI.transfer(0b00000000);
  digitalWrite(latch_pin,HIGH);
  digitalWrite(latch_pin,LOW);
  digitalWrite(latch_pin,HIGH);
  for(int i = 0;i < 32 ;i++)
height[i] = 16;
}
void loop() {

Serial.write('t');
Serial.write('\n');
if(Serial.available()>0)
  while(Serial.read() != ',');
  for(int i= 0;i<32;i++)
 {
  height[i] = Serial.parseInt();
 }
 

for(int col_scan = 0; col_scan < 32; col_scan++){
    if(col_scan < 8)
  {
  SPI.transfer(scan[col_scan]);
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  }
  else if(col_scan < 16)
  {
  SPI.transfer(0b00000000);
  SPI.transfer(scan[col_scan-8]);
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  }
  else if(col_scan < 24)
  {
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  SPI.transfer(scan[col_scan-16]);
  SPI.transfer(0b00000000);
  }
  else
  {
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000); 
  SPI.transfer(scan[col_scan-24]);
  }
  if(height[col_scan] ==0 )
  {
    SPI.transfer(0b00000000);
    SPI.transfer(0b00000000);
  }
else if(height[col_scan] < 8)
  {
  SPI.transfer(level[height[col_scan]-1]);
  SPI.transfer(0b00000000);
  }
  else
  {
  SPI.transfer(0b11111111);
  SPI.transfer(level[height[col_scan]-9]);
  }
    PORTD |= 1 << latch_pin;
  PORTD &= 0 << latch_pin;
  PORTD |= 1 << latch_pin;
  //delayMicroseconds(250);
      if(col_scan < 8)
  {
  SPI.transfer(scan[col_scan]);
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  }
  else if(col_scan < 16)
  {
  SPI.transfer(0b00000000);
  SPI.transfer(scan[col_scan-8]);
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  }
  else if(col_scan < 24)
  {
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  SPI.transfer(scan[col_scan-16]);
  SPI.transfer(0b00000000);
  }
  else
  {
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000); 
  SPI.transfer(scan[col_scan-24]);
  }
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000); 
  PORTD |= 1 << latch_pin;
  PORTD &= 0 << latch_pin;
  PORTD |= 1 << latch_pin;
  }
  for(int i = 0; i < 32 ; i++)
  prev_height[i] = height[i];
//LEDs_Off();
}

void LEDs_Off()
{
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  SPI.transfer(0b00000000);
  PORTD |= 1 << latch_pin;
  PORTD &= 0 << latch_pin;
  PORTD |= 1 << latch_pin;
}

