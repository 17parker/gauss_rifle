#include "ATMEGA328p.h"
#include "ATMEGA328p_TIMERS.h"

#define GATE_PIN PIN_9  //GATE PIN - PB1 - PWM OC1A
#define LED1 PIN_6		//LED1 PIN - PD6 - PWM OC0A
#define SENSOR1 PIN_A0  //PC0

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
	//toggle_pinB(PIN_13);
}

ISR(ADC_vect) {
	//adc_read_8_bit();
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
	//Serial.begin(115200);
	cli();
	init_ports();
	//built in led
	//disable_pullupB(PIN_13);
	//set_outputB(PIN_13);

	disable_pullupB(GATE_PIN);
	disable_pullupD(LED1);
	disable_pullupC(SENSOR1);
	set_outputB(GATE_PIN);
	set_outputD(LED1);
	//Setup the sensor's LED
	set_timer0(WGM_FAST_TOP, CLK_1, 0b10, LED1_DUTY);

	//Setup the sensor's ADC
	adc_select_pin(ADC0);
	adc_disable_interrupt();
	adc_enable_adc();
	adc_set_prescaler(ADC_PRESCALER_8);
	adc_set_free_run_trigger_source();
	adc_enable_auto_trigger();
	adc_init();

	//Setup the gate's timer
	set_timer1(FAST_8_BIT, CLK_OFF, 0b10, GATE_COMP_VAL);

	timer1_interrupt_masks(0b00000010);
	adc_enable_interrupt();
	sei();
	while (1) {
		//Serial.println(adc_read_value);
	}
}