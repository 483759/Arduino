#include <TimerOne.h>
#include <Arduino.h>
#define LED 3
#define LIGHT A0
 
volatile int state;
volatile int msec;
int adValue;

void setup() {
  Serial.begin(9600);
  state = 0;

  // initialize ADC
  ADCSRA = 0x88;
  ADMUX = 0x40;
  
  // LED
  DDRD |= (1<<LED);
  PORTD = 0;

  // initialize TC0
  TCCR0A = 0x02; // OC not use, xx, xx, CTC[1:0]
  TCCR0B = 0x03; // xx, xx, CTC[2], 1/256
  TIMSK0 = 0x02; // TC0 Output Compare A Match Interrupt Enable
  OCR0A = 250; // Compare Match on count 250 = 1 msec

  // initialize TC2
  TCCR2A = 0x23; // OC2A not used, OC2B = non-inverting mode, Fast PWM
  TCCR2B = 0x0E; // xx, xx, TOP = OCRA, 1/256 prescale
  OCR2A = 250; // 1 KHz frequency
  OCR2B = state;

  sei(); 
}

ISR(TIMER0_COMPA_vect)
{
  msec = (msec + 1) %1000;
  if (msec == 0) {
    ADCSRA |= 0x40;
  }
}

ISR(ADC_vect)
{
  adValue = ADCW;
  //state = 1 / adValue;
  //OCR2B = state;
}

void loop() {
  // put your main code here, to run repeatedly:

  state = 1000 - adValue;
  OCR2B = state;
  Serial.println(adValue);
  Serial.println(state);
}
