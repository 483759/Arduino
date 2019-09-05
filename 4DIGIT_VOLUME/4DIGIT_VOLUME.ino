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

void setup(){
  TouchCount = 0;
  Touched = 0;
  Display.setBrightness(0x04);
  Display.showNumberDec(TouchCount);
  ADCSRA = 0x80;  //1000_0000으로 7번째 비트 1로 설정해 줌으로써 ADC를 허용
  ADMUX = 0x40;   //0100_0000으로 기준전압을 AVcc(01), ADCH:ADCL에 오른쪽 정렬해준다
  DDRD &= ~(1<<TOUCH);  //TOUCH센서를 입력으로 설정
  EICRA |= (3<<2);  //Rising Edge일 때 INT1
  EIMSK |= (1<<1);  //INT1에 인터럽트 인에이블
  sei();
  }

  void loop(){
    if(Touched==1){
      ADCSRA |= 0x40; //0100_0000으로 ADC Start Conversion을 1을 기록해줘서 변환을 시작함
      while((ADCSRA&0x40)==0x40); //변환이 끝날때까지(6번째 비트가 0이 될 때 까지) 기다린다.
      adValue = ADCW;  //AD변환 된 (ADCH<<8)+ADCL의 값을 대입
      degree = (int)((float)adValue * FULL_ANGLE / MAX_ADC);  //볼륨 센서에 맞는 값으로 변환
       Display.showNumberDec(degree);   //4-bit 표시장치에 각도를 표시
      Touched = 0;
      
      }
    }
