라이브러리 구동 코드

int TOUCH = 11;  //센서별 포트 번호 지정
int LED = 9;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT); //LED센서를 출력 형식으로 설정
  pinMode(TOUCH, INPUT);  //터치센서를 입력 형식으로 설정
}

void loop() {
  int TouchValue = digitalRead(TOUCH);  //터치센서의 입력 값을 변수에 저장

  if(TouchValue == HIGH){ //터치했을 떄(변수에 HIGH가 저장됨)
    digitalWrite(LED, HIGH);  //LED센서를 켠다
  } else{
    digitalWrite(LED, LOW); //LED센서를 끈다
  }
}


GPIO 구동 코드

int TOUCH = B00100000;  //TOUCH 5번 포트
int LED = B00001000;  //LED 3번 포트


void setup() {
  Serial.begin(9600);
  DDRD = LED;
}

void loop() {
  Serial.println((PIND&TOUCH)==TOUCH);
  unsigned char touch;
  if(((touch=PIND)&(1<<5))==0x00) //TOUCH가 눌렸으면
  PORTD|=(1<<3);  //00001000
  else
  PORTD&=~(1<<3); //11110111
}
