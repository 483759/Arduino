#include <Arduino.h>
#include <TM1637Display.h>
#define DIO 11
#define CLK 10 
#define TOUCH 3
#define VOLUME A0
#define FULL_ANGLE 280.0
#define MAX_ADC 1023.0
TM1637Display Display(CLK, DIO);
volatile int Touched;
uint16_t TouchCount;
int adValue;
int degree;

ISR(INT1_vect){
Touched = 1;
}

ISR(ANALOG_COMP_vect){
  
  }

void setup() {
 TouchCount = 0; 
 Touched = 0;
 // TM1637Display
 Display.setBrightness(0x04);
 Display.showNumberDec(TouchCount);
 // intialize ADC
 ADCSRA = 0x80; // Enable ADC
 ADMUX = 0x40; // AVcc, Right adjust, ADC0

 // Touch sensor
 DDRD &= ~(1<<TOUCH); // input
 EICRA |= (3<<2); // rising edge of int1
 EIMSK |= (1<<1); // Interrupt mask register, interrupt enable
 sei();
}
void loop() {
 // put your main code here, to run repeatedly:
 if (Touched == 1) {
 //adValue = analogRead(VOLUME);
 ADCSRA |= 0x40; // start ADC
 while ( (ADCSRA & 0x40) == 0x40 ); // wait for finish
 adValue = ADCW; // (ADCH << 8) + ADCL;
 degree = (int)((float)adValue * FULL_ANGLE / MAX_ADC);
 Display.showNumberDec(degree);
 Touched = 0;
 }
} 
