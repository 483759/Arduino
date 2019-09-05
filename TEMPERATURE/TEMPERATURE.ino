#include <OneWire.h>

OneWire ds(11); //on pin11
byte ds_addr[8];
uint8_t found;
uint8_t scratchpad[10];
int16_t raw;  //raw data
float celcius;  //temperature
char buf[80]; //print buffer

void setup(){
  Serial.begin(9600);
  found = ds.search(ds_addr);
  if(found){
    sprintf(buf, "ROM addr: %2x, %2x, %2x, %2x, %2x, %2x, %2x, %2x",
            ds_addr[0], ds_addr[1], ds_addr[2], ds_addr[3],
            ds_addr[4], ds_addr[5], ds_addr[6], ds_addr[7]);
            Serial.println(buf);
    }  
}

void loop(){
  if(found){
    ds.reset();
    ds.select(ds_addr);
    ds.write(0x44, 1);  //Convert T start conversion
    delay(1000);

    ds.reset();
    ds.select(ds_addr);
    ds.write(0xBE); //Read scratchpad
    ds.read_bytes(scratchpad, 9); //8byte scratch pad + CRC
  

    for(int n=0;n<9;n++){
      Serial.print(scratchpad[n]);
      Serial.print(" ");  
    }
  
    Serial.println();
  
    raw = ((int16_t)scratchpad[1]<<8)+(int16_t)scratchpad[0];
    celcius = (float)raw/16.0;
    sprintf(buf, "Temperature: %d ", raw);
    Serial.print(buf);
    Serial.println(celcius);
  }  
  delay(1000);
}








#include <OneWire.h>

OneWire ds(11); //on pin11
byte ds_addr[8];
uint8_t found;
uint8_t scratchpad[10];
int16_t raw;  //raw data
float celcius;  //temperature
char buf[80]; //print buffer
volatile int msec, msec2, state;

void setup(){
  ADCSRA = 0x88;  //Enable ADC, ADC Interrupt enable
  ADMUX = 0x40;   //AVcc, Right adjust, ADC0

  TCCR0A = 0x02;  //OC not use, xx, xx, CTC[1:0]
  TCCR0B = 0x04;  //xx, xx, CTC[2], 1/256
  TIMSK0 = 0x02;  //TC0 Output Compare A Match Interrupt Enable
  OCR0A = 250;    //Compare Match on count 250 = 1msec
  
  Serial.begin(9600);
  found = ds.search(ds_addr);
  if(found){
    sprintf(buf, "ROM addr: %2x, %2x, %2x, %2x, %2x, %2x, %2x, %2x",
            ds_addr[0], ds_addr[1], ds_addr[2], ds_addr[3],
            ds_addr[4], ds_addr[5], ds_addr[6], ds_addr[7]);
            Serial.println(buf);
    }  

  sei();
}

ISR(TIMER0_COMPA_vect){
  msec = (msec + 1)%1000;
  if(msec==0){
    if(state == 0){
      ConvertTemp();  
    }
    if(state == 1){
      ReadTemp();
    }
  }
}

void ConvertTemp(){
    state = 1;
    ds.reset();
    ds.select(ds_addr);
    ds.write(0x44, 1);  //Convert T start conversion
}

void ReadTemp(){
    state = 2;
    ds.reset();
    ds.select(ds_addr);
    ds.write(0xBE); //Read scratchpad
    ds.read_bytes(scratchpad, 9); //8byte scratch pad + CRC
  
    for(int n=0;n<9;n++){
      Serial.print(scratchpad[n]);
      Serial.print(" ");  
    }
  
    Serial.println();
  
    raw = ((int16_t)scratchpad[1]<<8)+(int16_t)scratchpad[0];
    celcius = (float)raw/16.0;
    sprintf(buf, "Temperature: %d ", raw);
    Serial.print(buf);
    Serial.println(celcius);
}

void loop(){
    if(found && state == 2){
      state = 0;
    }
}
