uint8_t ac, old_ac;
void setup() {
  // put your setup code here, to run once:
old_ac=(1<<ACO);
DDRB |= (1<<3);
ADCSRB &= ~(1<<ACME);
PINB &= ~(1<<3);
ACSR |= (1<<ACBG);
}

void loop() {
  // put your main code here, to run repeatedly:
ac=ACSR&(1<<ACO);
if(ac!=old_ac){
  PORTB^=(1<<3);
  old_ac = ac;
  }
}


인터럽트 구동코드

uint8_t ac, old_ac;
void setup() {
  // put your setup code here, to run once:
old_ac=(1<<ACO);
DDRB |= (1<<3);
ADCSRB &= ~(1<<ACME);
ACSR = 0x00;
ACSR |= (1<<ACBG);
ACSR |= (1<<ACIE);
sei();
}

ISR(ANALOG_COMP_vect){
  ac = ACSR & (1<<ACO);
  }

void loop() {
  // put your main code here, to run repeatedly:
if(ac!=old_ac){
  PORTB^=(1<<3);
  old_ac = ac;
  }
}
