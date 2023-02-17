#include <ATMEGA328p.h>
#include <ATMEGA328p_TIMERS.h>

#define GATE_PIN PIN_9  	//GATE PIN - PB1 - PWM OC1A
#define LED1 PIN_6		//LED1 PIN - PD6 - PWM OC0A
#define SENSOR1 PIN_A0 		//PC0

#define LED1_DUTY 120 //0-255, duty cycle of LED PWM

#define TRIGGER_VAL 76
#define RESET_VAL 80
volatile uint8_t fired = 1;

#define GATE_COMP_VAL 120
#define GATE_PRESCALER CLK_256

void stop_timer1() { TCCR1B &= 0b11111000; }
void start_timer1() { TCCR1B |= GATE_PRESCALER; }

ISR(TIMER1_COMPA_vect) {
	stop_timer1();
	fired = 1;
}

ISR(ADC_vect) {
	if (ADCH <= TRIGGER_VAL) {
		if (fired)
			return;
		start_timer1();
	}
	else if (ADCH >= RESET_VAL) {
		fired = 0;
	}
}

int main() {
	cli();	
	init_ports();
	disable_pullupB(GATE_PIN);
	disable_pullupD(LED1);
	disable_pullupC(SENSOR1);
	set_outputB(GATE_PIN);
	set_outputD(LED1);

	//Set up the sensor LED
	set_timer0(WGM_FAST_TOP, CLK_1, 0b10, LED1_DUTY);
	
	//Setup the gate's timer
	set_timer1(FAST_8_BIT, CLK_OFF, 0b10, GATE_COMP_VAL);
	TIMSK1 = 0b00000010;
	
	//There needs to be a capacitor from AREF to Vcc if the reference value is Vcc
	ADMUX = (1 << 5) | (1 << 6); //Reference voltage is VCC with capacitor at AREF pin, ADC output is left adjusted

	ADCSRB &= 0b11111000;		//Set ADC free run mode
	//Enable ADC, start conversion, Auto trigger enable, Interrupt enable, ADC Prescaler Div. factor = 8
	ADCSRA = (1 << 7) | (1 << 6) | (1 << 5) | (1 << 3) | (1 << 1) | 1;

	sei(); 

	while(1){}
}