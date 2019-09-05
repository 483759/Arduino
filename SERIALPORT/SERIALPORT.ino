#define TxBufferEmpty ((UCSR0A & 0x20)==0x20) //TxReady(Transmit Buffer can receive new data)
#define RxComplete ((UCSR0A & 0x80)==0x80) //RxReady(Receive Complete)

void setup(){
  UBRR0 = 103;  //9600bps
  UCSR0B = 0b00011000; //Set 3, 4 bit -> Rx, Tx Enable
  UCSR0C = 0b00000110; //Async, No parity, 1 stop bit, 8 data bits
}


void putch(uint8_t ch){
  while(!TxBufferEmpty);  //wait
  UDR0 = ch;  //Transmit ch
}

uint8_t getch(void){
  while (!RxComplete);  //wait
  return UDR0;  //Get new data UDR0
}

void loop(){
  uint8_t ch;
  ch = getch();
  putch(ch);  
}








#define TxBufferEmpty ((UCSR0A & 0x20)==0x20) //TxReady(Transmit Buffer can receive new data)
#define RxComplete ((UCSR0A & 0x80)==0x80) //RxReady(Receive Complete)
#define MAXLEN 80
#define CR '\n'
#define LF '\r'

void setup(){
  UBRR0 = 103;  //9600bps
  UCSR0B = 0b00011000; //Set 3, 4 bit -> Rx, Tx Enable
  UCSR0C = 0b00000110; //Async, No parity, 1 stop bit, 8 data bits

  putstr("Welcome!\n");
}

void putch(uint8_t ch){
  while(!TxBufferEmpty);  //wait
  UDR0 = ch;  //Transmit ch
}

void putstr(uint8_t *str){
  while(*str!=NULL){
    putch(*str++);  
  }  
}

uint8_t getch(void){
  while (!RxComplete);  //wait
  return UDR0;  //Get new data UDR0
}

int16_t getstr(uint8_t str[]){
  int16_t n = 0;
  do{
    str[n++] = getch();
  }while((str[n-1]!=CR)&&(n<MAXLEN));
  str[n] = NULL;
  return n;
}

void loop(){
  uint8_t str[81];
  getstr(str);
  putstr(str); 
}











#define QSIZE 100
#define MAXLEN 80
#define CR '\n'
#define LF '\r'
class CQueue {
  uint8_t queue[QSIZE];
  uint8_t head;
  uint8_t tail;
public:
  CQueue() { head = tail = 0; }
  bool enqueue(uint8_t ch) {
    uint8_t h;
    bool flag;
    h = (head + 1) % QSIZE;
    if (h == tail) 
      flag = false; //Queue_Full -> do nothing
    else {
      queue[h] = ch;
      head = h;
      flag = true; // success
    }
    return flag;
  }
  uint8_t dequeue(void) {
    uint8_t ch;
    if (head == tail) // QUEUE_EMPTY
      ch = NULL;
    else {
      ch = queue[tail];
      tail = (tail + 1) % QSIZE;
    }
      return ch;
  }
};
struct CQueue RxQueue;
struct CQueue TxQueue;
void setup() {
  // setup USART
  UBRR0 = 103; // 9600 bps
  UCSR0B = 0b10011000; //Rx Interrupt Enable / Data Ready Interrupt Disable / Rx Tx Enable 
  UCSR0C = 0b00000110; 
  sei();
}
ISR(USART_RX_vect){ //Rx Complete Interrupt
  RxQueue.enqueue(UDR0);  //Receive Data put Rx Queue from Rx Register
}
ISR(USART_UDRE_vect){ //Tx Interrupt Enable
  uint8_t ch;
  ch = TxQueue.dequeue();
  if (ch != NULL)
    UDR0 = ch;
  else
    UCSR0B &= 0b11011111; // Tx Interrupt Disable
}
uint8_t getch(void){
  cli(); // critical section
  uint8_t ch = RxQueue.dequeue();
  sei();
  return ch;
}
void putch(uint8_t ch){
  cli(); // critical section
  TxQueue.enqueue(ch);
  if ((UCSR0B & (1 << UDRIE0)) == 0) // Tx Int. disable
    UCSR0B |= (1 << UDRIE0); //Tx Interrupt Enable
  sei();
}
int16_t getstr(uint8_t str[]){
  int16_t n = 0;
  uint8_t ch;
  do {
    ch = getch();
    if (ch != NULL) {
    str[n++] = ch;
    }
  } while ((ch != CR) && (n<MAXLEN));
  str[n] = NULL;
  return n;
}
void putstr(uint8_t *str){
  while (*str != NULL) {
    putch(*str++);
  }
}
void loop() {
  /*****Synchronous I/O*****/
  uint8_t str[MAXLEN+1];
  getstr(str); // 문자열 입력
  putstr(str); // 문자열 출력
}
