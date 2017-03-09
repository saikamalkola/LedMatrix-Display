#include <SPI.h>

#define clock_pin 13
#define data_pin 11
#define latch_pin 2
#define row_offset 0
int row_scan = 0;
int row = 0;
unsigned long int pause_time = 3000000;
unsigned long int scroll_speed = 70000 ;
/************************************
 edit:
 msg1 : this will get scrolled in bottom eight rows of matrix
 msg2 : this will get scrolled in top eight rows of matrix
 */
unsigned long present_ms = 0;
unsigned long last_ms = 0;
unsigned long pause_ms = 0;
char newline = '\n';
byte scroll_msg[128];
int msg_index1 = 0;
int msg_index2 = 0;
int scroll_index = 0;
byte scan[8];
byte led[64];
boolean pause;
char msg2[200] = "                      ";
String msg1 =    "                      ";
const byte char_set[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#$%&'()*+-,.~:;<>?@[]/\\=_";
const byte char_def[] = {
  0,0,0,0,0,0,0,0,                // Space symbol (must be the first character in this array)
  // Upper case letters:
  24,60,102,126,102,102,102,0,    // A
  124,102,102,124,102,102,124,0,  // B
  60,102,96,96,96,102,60,0,       // C
  120,108,102,102,102,108,120,0,  // D
  126,96,96,120,96,96,126,0,      // E
  126,96,96,120,96,96,96,0,       // F
  60,102,96,110,102,102,60,0,     // G
  102,102,102,126,102,102,102,0,  // H
  60,24,24,24,24,24,60,0,         // I
  30,12,12,12,12,108,56,0,        // J
  102,108,120,112,120,108,102,0,  // K
  96,96,96,96,96,96,126,0,        // L
  99,119,127,107,99,99,99,0,      // M
  102,118,126,126,110,102,102,0,  // N
  60,102,102,102,102,102,60,0,    // O
  124,102,102,124,96,96,96,0,     // P
  60,102,102,102,102,60,14,0,     // Q
  124,102,102,124,120,108,102,0,  // R
  60,102,96,60,6,102,60,0,        // S
  126,24,24,24,24,24,24,0,        // T
  102,102,102,102,102,102,60,0,   // U
  102,102,102,102,102,60,24,0,    // V
  99,99,99,107,127,119,99,0,      // W
  102,102,60,24,60,102,102,0,     // X
  102,102,102,60,24,24,24,0,      // Y
  126,6,12,24,48,96,126,0,        // Z
  // Lower case letters:
  0,0,60,6,62,102,62,0,           // a
  0,96,96,124,102,102,124,0,      // b
  0,0,60,96,96,96,60,0,           // c
  0,6,6,62,102,102,62,0,          // d
  0,0,60,102,126,96,60,0,         // e
  0,14,24,62,24,24,24,0,          // f
  0,0,62,102,102,62,6,124,        // g
  0,96,96,124,102,102,102,0,      // h
  0,24,0,56,24,24,60,0,           // i
  0,6,0,6,6,6,6,60,               // j
  0,96,96,108,120,108,102,0,      // k
  0,56,24,24,24,24,60,0,          // l
  0,0,102,127,127,107,99,0,       // m
  0,0,124,102,102,102,102,0,      // n
  0,0,60,102,102,102,60,0,        // o
  0,0,124,102,102,124,96,96,      // p
  0,0,62,102,102,62,6,6,          // q
  0,0,124,102,96,96,96,0,         // r
  0,0,62,96,60,6,124,0,           // s
  0,24,126,24,24,24,14,0,         // t
  0,0,102,102,102,102,62,0,       // u
  0,0,102,102,102,60,24,0,        // v
  0,0,99,107,127,62,54,0,         // w
  0,0,102,60,24,60,102,0,         // x
  0,0,102,102,102,62,12,120,      // y
  0,0,126,12,24,48,126,0,         // z
  // Numbers
  60,102,110,118,102,102,60,0,    // 0
  24,24,56,24,24,24,126,0,        // 1
  60,102,6,12,48,96,126,0,        // 2
  60,102,6,28,6,102,60,0,         // 3
  6,14,30,102,127,6,6,0,          // 4
  126,96,124,6,6,102,60,0,        // 5
  60,102,96,124,102,102,60,0,     // 6
  126,102,12,24,24,24,24,0,       // 7
  60,102,102,60,102,102,60,0,     // 8
  60,102,102,62,6,102,60,0,       // 9
  // Characters and symbols
  24,24,24,24,0,0,24,0,           // !
  102,102,255,102,255,102,102,0,  // #
  24,62,96,60,6,124,24,0,         // $
  98,102,12,24,48,102,70,0,       // %
  60,102,60,56,103,102,63,0,      // &
  6,12,24,0,0,0,0,0,              // '
  12,24,48,48,48,24,12,0,         // (
  48,24,12,12,12,24,48,0,         // )
  0,102,60,255,60,102,0,0,        // *
  0,24,24,126,24,24,0,0,          // +
  0,0,0,126,0,0,0,0,              // -
  0,0,0,0,0,24,24,48,             // ,
  0,0,0,0,0,24,24,0,              // .
  60,66,165,129,165,153,66,60,    // "Smiley face" replaces the tilde (~) character
  0,0,24,0,0,24,0,0,              // :
  0,0,24,0,0,24,24,48,            // ;
  14,24,48,96,48,24,14,0,         // <
  112,24,12,6,12,24,112,0,        // >
  60,102,6,12,24,0,24,0,          // ?
  60,102,110,110,96,98,60,0,      // @
  60,48,48,48,48,48,60,0,         // [
  60,12,12,12,12,12,60,0,         // ]
  0,3,6,12,24,48,96,0,            // /
  0,96,48,24,12,6,3,0,            // \
  0,0,126,0,126,0,0,0,            // =
  0,0,0,0,0,0,0,255               // _
};
void setup() {
  // put your setup code here, to run once:
  SPI.setBitOrder(LSBFIRST);
SPI.setDataMode(SPI_MODE0);
SPI.setClockDivider(SPI_CLOCK_DIV2);
pinMode(latch_pin, OUTPUT);
pinMode(data_pin, OUTPUT);
pinMode(clock_pin, OUTPUT);
SPI.begin();
Serial.begin(115200);
scan[7]=B00000001;
scan[6]=B00000010;
scan[5]=B00000100;
scan[4]=B00001000;
scan[3]=B00010000;
scan[2]=B00100000;
scan[1]=B01000000;
scan[0]=B10000000;

for(int i = 0; i < 6 ; i++)
SPI.transfer(0b00000000);
  PORTD |= 1 << latch_pin;
  PORTD &= 0 << latch_pin;
  PORTD |= 1 << latch_pin;
  initialize();

}

void loop() {

present_ms = micros();
if(present_ms - last_ms > scroll_speed){
  last_ms = present_ms;
  if(pause == true){
    finish_scroll();
    if(present_ms - pause_ms > pause_time)
    initialize();
  }
  else
    scroll_message();
}

for(row_scan = 0;row_scan<16;row_scan++)
{

  for(int i = row ; i < 4+row ; i++)
{
  SPI.transfer(0);
}
  if(row_scan < 8)
  {
  SPI.transfer(scan[row_scan]);
  SPI.transfer(0b00000000);
  }
  else
  {
  SPI.transfer(0b00000000);
  SPI.transfer(scan[row_scan-8]);
  }
  PORTD |= 1 << latch_pin;
  PORTD &= 0 << latch_pin;
  PORTD |= 1 << latch_pin;
  delayMicroseconds(150);
for(int i = row ; i < 4+row ; i++)
{
  SPI.transfer(scroll_msg[i]);
}
  if(row_scan < 8)
  {
  SPI.transfer(scan[row_scan]);
  SPI.transfer(0b00000000);
  }
  else
  {
  SPI.transfer(0b00000000);
  SPI.transfer(scan[row_scan-8]);
  }
  PORTD |= 1 << latch_pin;
  PORTD &= 0 << latch_pin;
  PORTD |= 1 << latch_pin;
  delayMicroseconds(100);
  row = row + 4;
  if(row == 64)
  row = 0;
  }
}

void initialize(){
  for(int i = 0; i < 128; i++)
   scroll_msg[i] = 0;
  msg_index1 = 0;
  msg_index2 = 0;
  frame_decode();
  pause = false;
}
void frame_decode() {
  int char_index_1[4] = {0};
  for(int j = 0; j<4 ; j++){
  if (msg1[msg_index1] == '\0') {
    msg_index1 = 0;
    pause = true;
    pause_ms = present_ms;
  }
  for (int i = 0; i < 89; i++)
    if (msg1[msg_index1] == char_set[i])
      char_index_1[j] = i;
  msg_index1++;
  }

  for (int i = 7; i >= 0 ; i--)
  {
  for(int j = 0; j < 4 ; j++)
  {
    scroll_msg[4*(7-i) + 64 + j] = char_def[char_index_1[j] * 8 + i];
  }
  }
  int char_index_2[4] = {0};
  for(int j = 0; j<4 ; j++){
  if (msg2[msg_index2] == '\0') {
    msg_index2 = 0;
    pause = true;
    pause_ms = present_ms;
  }
  for (int i = 0; i < 89; i++)
    if (msg2[msg_index2] == char_set[i])
      char_index_2[j] = i;
  msg_index2++;
  }

  for (int i = 7; i >= 0 ; i--)
  {
  for(int j = 0; j < 4 ; j++)
  {
    scroll_msg[4*(15 - i) + 64 + j] = char_def[char_index_2[j] * 8 + i];
  }
  }
}
void scroll_message()
{ 
  
 for(int i = 0; i < 16 ; i++)
  { 
 for(int j = 0; j < 3 ; j++)
 {
   scroll_msg[4*i+j] = scroll_msg[4*i+j] << 1;
   bitWrite(scroll_msg[4*i+j],0,bitRead(scroll_msg[4*i+j+1],7));
}
    scroll_msg[4*i+3] = scroll_msg[4*i+3] << 1;
    bitWrite(scroll_msg[4*i+3],0,bitRead(scroll_msg[4*i+64],7));
for(int j = 0; j < 3 ; j++)
 {
   scroll_msg[4*i+j+64] = scroll_msg[4*i+j+64] << 1;
   bitWrite(scroll_msg[4*i+j+64],0,bitRead(scroll_msg[4*i+j+64+1],7));
}
 scroll_msg[4*i+3+64] = scroll_msg[4*i+3+64] << 1;
}
scroll_index++;
if(scroll_index > 31)
{
 scroll_index = 0;
 frame_decode();
}
}
void finish_scroll()
{
 for(int i = 0; i < 16 ; i++)
  { 
 for(int j = 0; j < 3 ; j++)
 {
   scroll_msg[4*i+j] = scroll_msg[4*i+j] << 1;
   bitWrite(scroll_msg[4*i+j],0,bitRead(scroll_msg[4*i+j+1],7));
}
    scroll_msg[4*i+3] = scroll_msg[4*i+3] << 1;

}
}


/*for(int i = 0; i<32 ; i++)
{
  scroll_msg[i] = scroll_msg[i] << 1;
  if(scroll_msg[i+64] & 128)
  scroll_msg[i] = scroll_msg[i] | 1;
  scroll_msg[i+64] = scroll_msg[i+64] << 1;   
}
scroll_index++;
if(scroll_index > 31)
{
 scroll_index = 0;
 frame_decode();
}
}
*/

