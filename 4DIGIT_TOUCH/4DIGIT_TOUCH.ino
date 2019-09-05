#include <Arduino.h>
#include <TM1637Display.h>
#define DIO 10
#define CLK 11
#define TOUCH 3 //INT1로 사용
#define SUBTOUCH 9  //PCINT1로 사용
TM1637Display disp(CLK, DIO);
volatile int TOUCHED;
volatile int SUBTOUCHED;
uint16_t Count;
void setup() {
 TOUCHED = 0;
 SUBTOUCHED = 0;
 Count = 0;
 // TM1637Display
 disp.setBrightness(0x04);
 disp.showNumberDec(Count);

 // Touch sensor
 DDRD &= ~((1<<TOUCH)+(1<<SUBTOUCH)); // toggle type touch sensor input
 EICRA = 3 << 2;
 EIMSK = 1 << 1;
 PCMSK0 |= 1 << 1;
 PCICR |= 1;
 sei();
}
ISR(PCINT0_vect)
{
 SUBTOUCHED = 1;
}
ISR(INT1_vect){
  TOUCHED = 1;
  }
void loop() {
if (TOUCHED == 1) {
 Count += 1;
 disp.showNumberDec(Count);
  TOUCHED = 0;
 }
 if(SUBTOUCHED == 1){
  Count -= 1;
   disp.showNumberDec(Count);
  SUBTOUCHED = 0;
  }
} 
