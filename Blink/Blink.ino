#include <TM1637Display.h>
#include <OneWire.h>
#define QSIZE 100
#define MAXLEN 80

#define LED 3
#define TOUCH 5
#define LATCH 9
#define BUZZER 13
#define VOLUME A0
#define DIO 11
#define CLK 10

TM1637Display Display(CLK, DIO);
OneWire ds(8);
byte ds_addr[8];  //온도 저장 배열
uint8_t duty_cycle, dots, found, scratchpad[10];
uint16_t elpased;
volatile int msec, start, t;
volatile uint16_t second, minute;
int timer_state, temp_state;
int adValue;
float celcius;  //temperature
char buf[80]; //print buffer

class CQueue {  //직렬통신을 위한 큐 정의
  uint8_t queue[QSIZE];
  uint8_t head;
  uint8_t tail;
public:
  CQueue() { head = tail = 0; }
  bool enqueue(uint8_t ch) {
    uint8_t h;
    bool flag;
    h = (head + 1) % QSIZE;
    if (h == tail)  flag = false; //Queue_Full -> do nothing
    else {
      queue[h] = ch;
      head = h;
      flag = true; // success
    }
    return flag;
  }
  uint8_t dequeue(void) {
    uint8_t ch;
    if (head == tail) ch = NULL;// QUEUE_EMPTY
    else {
      ch = queue[tail];
      tail = (tail + 1) % QSIZE;
    }
      return ch;
  }
};

struct CQueue TxQueue;

void setup() {
  duty_cycle = 0; timer_state = 0;  temp_state = 0; led_state = 0;
  start = 0; dots=0x40; msec = 0; msec2 = 0;
  
  DDRD |= (1 << LED);  //LED 출력 설정
  DDRB |= (1 << 5);  //BUZZER 출력 설정
  DDRD &= ~(1 << TOUCH); //Touch 입력 설정
  DDRB &= ~(1<< LATCH); //Latch 입력 설정
  PORTD &= ~(1 << LED); 
  PORTB &= ~(1 << 5);  //LED와 BUZZER 출력 초기화(꺼짐)

  //TM1637Display
  Display.setBrightness(0x04);
  Display.showNumberDec(msec);

  PCICR |= (1<<0);  //PCMSK0 레지스터 인터럽트 인에이블
  PCMSK0 |= (1<<1); //PCINT1 인터럽트 허용
  PCICR |= (1<<2);  //PCMSK2 레지스터 인터럽트 인에이블
  PCMSK2 |= (1<<5); //PCINT21 인터럽트 허용

  //TC0
  TCCR0A = 0x02;  //OC not use, xx, xx, CTC[1:0]
  TCCR0B = 0x03;  //xx, xx, CTC[2], 1/64
  OCR0A = 250;    //Compare Match on count 250 = 1msec
  TIMSK0 = 0x02;  //TC0 Output Compare A Match Interrupt Enable
  
  //TC2
  TCCR2A = 0x23;  //Fast PWM/ Clear OCA on CM & Set OCA at Bottom
  TCCR2B = 0x0E;  //Prescalier 1/256
  OCR2A = 250; //Top Count
  //OCR2B = duty_cycle; //0% duty

  ADCSRA = 0x88;  //Enable ADC && ADC interrupt
  ADMUX = 0x40;   //AVcc, Right adjust, ADC0

  UBRR0 = 103; // 9600 bps
  UCSR0B = 0b10011000; //Rx Interrupt Enable / Data Ready Interrupt Disable / Rx Tx Enable 
  UCSR0C = 0b00000110; 

  found = ds.search(ds_addr);
  sei();
}

ISR(TIMER0_COMPA_vect){ //TC0 타이머 인터럽트
  msec = (msec + 1)%30000;  //프로그램 실행 시간을 측정하는 변수 msec
  t = msec - start; //터치(래치,일반)센서가 눌렸을 때의 시간차를 측정하는 변수
    
  if((msec%1000)==0){
    second += 1;
    temp_state = 0;
  } else if((msec%1000)==500){
    temp_state = 1;
  }
  
  if(second==60){
    minute += 1;
    second = 0;  
  }  
  elpased = minute*100 + second;
        
  switch(timer_state){ 
    case 1:  //터치 센서가 눌렸을 때
        if(t==500) LEDON();
        else if(t==1000) LEDOFF();
        else if(t==1500) LEDON();
        else if(t==2000) {
          LEDOFF();
          timer_state = 3;
        } //2번 깜빡임
      break;
    case 2:   //래치형 터치 센서가 눌렸을 때
        if(t==500) LEDON();
        else if(t==1000) LEDOFF();
        else if(t==1500) LEDON();
        else if(t==2000) LEDOFF();
        else if(t==2500) LEDON();
        else if(t==3000){
          LEDOFF();
          timer_state = 3;
        }   //3번 깜빡임
      break;   
    default:
      break;
  }
}

void putch(uint8_t ch){
  cli();
  TxQueue.enqueue(ch);
  if((UCSR0B & (1 << UDRIE0)) == 0)   //Tx Interrupt Disable
    UCSR0B |= (1 << UDRIE0);  //Tx Interrupt Enable
  sei();
}

void putstr(uint8_t *str){
  while (*str != NULL)
    putch(*str++);
}

void LEDON(){
  OCR2B = adValue;
}

void LEDOFF(){
  duty_cycle = 0;
  OCR2B = duty_cycle;
}

ISR(PCINT2_vect){ //터치 센서를 눌렀을 때
  start = msec;
  timer_state = 1;
  putstr("Touched\n");
  ADCSRA |= 0x40;
}

ISR(PCINT0_vect){ //래치형 터치 센서를 눌렀을 때
  start = msec;
  timer_state = 2;
  putstr("Latch Touched\n");
  ADCSRA |= 0x40;
}

ISR(ADC_vect) {
  adValue = ADCW;  
  adValue = (int)((float)adValue*280.0/1023.0);
}

ISR(USART_UDRE_vect){
  uint8_t ch;
  ch = TxQueue.dequeue();
  if(ch != NULL)  UDR0 = ch;  //데이터 버퍼에 문자 저장
  else  UCSR0B&=0b11011111; //Tx Interrupt Disable  
}

void loop() {    
  if(celcius>=27.0) PORTB |= (1 << 5);  //LED와 BUZZER 출력 초기화(꺼짐)
  else PORTB &= ~(1 << 5);  //LED와 BUZZER 출력 초기화(꺼짐)

  if(found){
    switch(temp_state){
      case 0:
        ds.reset();
        ds.select(ds_addr);
        ds.write(0x44, 1);  //Convert T start conversion
        temp_state = 2;
        break;
      case 1:
        ds.reset();
        ds.select(ds_addr);
        ds.write(0xBE); //Read scratchpad
        ds.read_bytes(scratchpad, 9); //8byte scratch pad + CRC
              
        celcius = (float)(((int16_t)scratchpad[1]<<8)+(int16_t)scratchpad[0])/16.0;
        putstr("Temperature: ");        dtostrf(celcius, 5, 2, buf);        putstr(buf);  
        sprintf(buf, "\tTime : %dmin %dsec\tVolume Value : ", minute, second, adValue);  putstr(buf);  putstr("\n");
        //1초에 한 번씩 상태 메시지 시리얼 모니터에 출력
        temp_state = 2;
        break;
     default:
        break;       
    }  
  }
  Display.showNumberDecEx(elpased, dots, true); //4-digit 표시장치에 시간 출력
}
