#include <TM1637.h>
#include <Arduino.h>
#define DIO 11
#define CLK 10
#define TOUCH 3
#define VOLUME A0
#define FULL_ANGLE 280.0
#define MAX_ADC 1023.0

TM1637Display Display(CLK, DIO);
volatile int state;
volatile int adValue;
int degree = 0;

ISR(INT1_vect){
  if(state==0){
    ADCSRA |= 0x40;
    state=1;
    }
  }
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  state = 0;
  Display.setBrightness(0x04);
  Display.showNumberDec(degree);

  DDRD &= ~(1<<TOUCH);
  EICRA |= (3<<2);
  EIMSK |= (1<<1);
  sei();
}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t t1, t2, adc_time, dsp_time;
  if(state==1){
    t1 = micros(); adValue = analogRead(VOLUME); t2 = micros();
    adc_time = t2-t1;
    degree = (int)((float)adValue*FULL_ANGLE/MAX_ADC);
    t1 = micros(); Display.showNumberDec(degree); t2 = micros();
    dsp_time = t2-t1;
    t1 = micros();
    Serial.print("ADC time : ");  Serial.println(adc_time);
    Serial.print("DSP time : ");  Serial.println(dsp_time);
    t2 = micros();
    Serial.print("PRT time : ");  Serial.println(t2-t1);
    state=0;
    }
}
