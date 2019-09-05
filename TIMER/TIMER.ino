#include <TimerOne.h>
#define LED 11

void setup(){
  pinMode(LED, OUTPUT);
  Timer1.initialize(500000);
  Timer1.attachInterrupt(timer1_500);
  }

void timer1_500(){
  digitalWrite(11, digitalRead(LED)^1);
  }

void loop(){
  }


1 2 3 4
5 10

#include <TM1637Display.h>
#include <SimpleTimer.h>

#define TOUCH 3
#define DIO 11
#define CLK 10
#define TICK_INTERVAL 25

SimpleTimer timer;
TM1637Display Display(CLK, DIO);

int id_toggle;
int id_inc_time;
volatile uint8_t touched;
volatile uint16_t seconds, ticks, old_ticks;
uint8_t dots;
uint16_t elpased;

void setup(){
  seconds = ticks = old_ticks = 0;
  dots = 0; //변수 초기화

  DDRD &= ~(1<<TOUCH);  //TOUCH센서를 입력으로
  EICRA = 0x0C; //Rising Edge일 때 인터럽트 발생
  EIMSK = 0x02; //INT1을 인터럽트 인에이블
  sei();

  Display.setBrightness(0x04);
  Display.showNumberDecEx(0, dots, true);
  }

ISR(INT1_vect){
  touched ^= 1; //touched변수 토글
  if(touched==1){
    id_inc_time = timer.setInterval(TICK_INTERVAL, ft_inc_tick);  //TICK_INTERVAL마다 ft_inc_tick함수 호출
    id_toggle = timer.setInterval(500, ft_dot_toggle);  //500msec마다 dot 토글
    elpased = micros();
    }
  else{ //touched가 0일 때(이미 타이머가 돌아가고 있을 때)
    timer.deleteTimer(id_toggle);
    timer.deleteTimer(id_inc_time); //setInterval함수를 저장해놓은 변수의 타이머를 제거함으로써 타이머 정지
    seconds = ticks = 0;
    }
  }

void ft_inc_tick(){
  ticks += TICK_INTERVAL; //10msec만큼 초 증가
  if(ticks == 1000) { //1초가 되면
    seconds += 1;
    ticks = 0;
    } //1초 증가
  elpased = seconds * 100 + ticks/10; //?
  }

void ft_dot_toggle(){
  dots = (dots==0) ? 0x40 : 0;  //dots 토글
  }

void loop(){
  timer.run();
  if((touched==1)&&(old_ticks!=ticks)){
    Display.showNumberDecEx(elpased, dots, true); //타이머를 4-digit에 표시
    old_ticks = ticks;
    }
  }
