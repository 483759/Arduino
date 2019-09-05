#include <TM1637Display.h>

#define DIO 11
#define CLK 10
#define TOUCH 3

TM1637Display Display(CLK, DIO);
volatile int Touched;
volatile int Ticks, Elapsed;
int LastTime, Dots;

void setup(){
  Ticks = 0;
  Elapsed = 0;
  LastTime = 0;
  Dots = 0;

  Display.setBrightness(0x04);
  Display.showNumberDecEx(Elapsed, Dots, 1);

  DDRD &= ~(1<<TOUCH);
  EICRA |= (3<<2);
  EIMSK |= (1<<1);

  TCCR0A = 0x02;
  TCCR0B = 0x03;
  TIMSK0 = 0x02;
  OCR0A = 250;
  sei();
  }

ISR(INT1_vect){
  Touched ^= 1;
  if(Touched == 1){
      Ticks = 0;
    }
  }

ISR(TIMER0_COMPA_vect){
  if(Touched){
    Ticks += 1;
    Elapsed = Ticks/10;
    }
  }

void loop(){
  if(Elapsed != LastTime){
    Dots = ((Elapsed%100)<50)?0:0x40;
    Display.showNumberDecEx(Elapsed, Dots, 1);
    LastTime = Elapsed;
    }
  }
