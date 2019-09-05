#include <OneWire.h>
#define QSIZE 100
#define MAXLEN 80
#define CR "\n"
#define LF '\r'

OneWire ds(11); //on pin11
byte ds_addr[8];
uint8_t found;
uint8_t scratchpad[10];
int16_t raw;  //raw data
float celcius;  //temperature
char buf[80]; //print buffer
volatile int msec;
int state;

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

void setup(){
  msec = 0;  state = 0;
  TCCR0A = 0x02;  //OC not use, xx, xx, CTC[1:0]
  TCCR0B = 0x03;  //xx, xx, CTC[2], 1/64
  TIMSK0 = 0x02;  //TC0 Output Compare A Match Interrupt Enable
  OCR0A = 250;    //Compare Match on count 250 = 1msec
  
  UBRR0 = 103; // 9600 bps
  UCSR0B = 0b10011000; //Rx Interrupt Enable / Data Ready Interrupt Disable / Rx Tx Enable 
  UCSR0C = 0b00000110; 
  
  found = ds.search(ds_addr);
  if(found){
    sprintf(buf, "ROM addr: %2x, %2x, %2x, %2x, %2x, %2x, %2x, %2x",
            ds_addr[0], ds_addr[1], ds_addr[2], ds_addr[3],
            ds_addr[4], ds_addr[5], ds_addr[6], ds_addr[7]);
            putstr(buf);
            putstr(CR);
  }  
  sei();
}

ISR(TIMER0_COMPA_vect){
  msec = (msec + 1)%2000;  
}

ISR(USART_UDRE_vect){
  uint8_t ch;
  ch = TxQueue.dequeue();
  if(ch != NULL)  UDR0 = ch;
  else  UCSR0B&=0b11011111; //Tx Interrupt Disable  
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

void loop(){
  if(found){
    switch(state){
      case 0:
        ds.reset();
        ds.select(ds_addr);
        ds.write(0x44, 1);  //Convert T start conversion
        state = 1;
        break;
      case 1:
        ds.reset();
        ds.select(ds_addr);
        ds.write(0xBE); //Read scratchpad
        ds.read_bytes(scratchpad, 9); //8byte scratch pad + CRC
      
        for(int n=0;n<9;n++){
          sprintf(buf, "%hhu ", scratchpad[n]);
          putstr(buf);
        }
        putstr(CR);
        
        raw = ((int16_t)scratchpad[1]<<8)+(int16_t)scratchpad[0];
        celcius = (float)raw/16.0;
        sprintf(buf, "Temperature: %d ", raw);
        putstr(buf);
        dtostrf(celcius, 5, 2, buf);
        putstr(buf);  putstr("\n");
        state = 2;
        break;
     case 2:
        if(msec==0)
          state = 0;
        break;
     default:
        break;       
    }  
  }
}
