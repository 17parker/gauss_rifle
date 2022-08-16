#include "ATMEGA328p.h"
#include "ATMEGA328p_TIMERS.h"

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
	/*
	I'm using a timer (basically PWM)
	The PWM starts HIGH, when it goes LOW the interrupt is triggered
	*/
	stop_timer1();		//Stops the timer so there is only 1 pulse
	fired = 1;		//Flag, otherwise it'll keep re-firing
}

ISR(ADC_vect) {
	//When the ADC finishes reading the pin, this interrupt triggers
	//ADC is in free-running mode, so it'll immediately start reading again
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

	//Setup the sensor's LED
	set_timer0(WGM_FAST_TOP, CLK_1, 0b10, LED1_DUTY);	//A lot happens in this function

	//Enable the ADC input on ADC0
	ADMUX &= 0b11110000;	
	ADMUX |= ADC0;

	ADCSRA &= ~(1 << 3);	
	ADCSRA |= 0b10000000;	
	adc_set_prescaler(ADC_PRESCALER_8);
	ADCSRB &= 0b11111000;	//Set ADC free run mode (trigger new analog read right after it finishes)
	ADCSRA |= (1 << 5);	//Enable ADC auto trigger (new ADC conversion starts right after one finishes)
	
	//This sets the reference voltage for the ADC to Vcc, selected ADC pin to ADC0
	//There needs to be a capacitor from AREF to Vcc
	//The read value is left-adjusted (ADLAR = 1)
	ADMUX = (1 << 5) | (1 << 6);
	ADCSRA |= (1 << 6);		//Starts the reading, first read takes 25 cycles vs 13

	//Setup the gate's timer
	set_timer1(FAST_8_BIT, CLK_OFF, 0b10, GATE_COMP_VAL);	//A lot happens here

	TIMSK1 = 0b00000010;	//Un-mask interrupt for ADC0
	//Enable the ADC interrupt
	ADCSRA = (1 << 7) | (1 << 6) | (1 << 5) | (1 << 3) | (1 << 2) | (1 << 1) | 1;
	
	sei(); 	//Enable global interrupts

	while (1) {
		//It's all in interrupts
	}
}
