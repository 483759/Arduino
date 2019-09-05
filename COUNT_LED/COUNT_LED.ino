#define LED 6
volatile int msec;
void setup() {
 // put your setup code here, to run once:
 msec = 0;
 // port PD.6
 DDRD |= (1<<LED); // output
 PORTD &= ~(1<<LED); // clear

 // initialize TC0 
 
 TCCR0A = 0x02; // OC not use, xx, xx, CTC[1:0]
 TCCR0B = 0x03; // xx, xx, CTC[2], 1/64
 TIMSK0 = 0x02; // TC0 Output Compare A Match Interrupt Enable
 OCR0A = 250; // 1msec count: 250KHz/250 = 1KHz
 sei();
}
ISR(TIMER0_COMPA_vect)
{W
 msec += 1;
 if (msec == 500) {
 msec = 0;
 PORTD ^= (1<<LED); // LED toggle
 }
}
void loop() {
 // put your main code here, to run repeatedly:
} 
