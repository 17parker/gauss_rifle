#include "ATMEGA328p.h"
#include "ATMEGA328p_TIMERS.h"
//I need to rebuild my coil gun code because I'm not smart enough to back it up

//I had a timer/PWM that controlled the brightness of the LED

/*
I had an analog read set to free run mode (non-stop)
When it was done converting, it would check if the value was under a specific threshold
There was a flag that would set when it went under, and it prevented it from re-triggering
The flag was cleared when it went above (threshold + [some buffer])

If it went under the threshold, it would start the clock on a timer/PWM
(I remember the interrupt was always enabled, it was the Timer's clock that was stopped & started)
The Timer/PWM output would serve as the GATE controller
The Timer/PWM had an interrupt that would disable the PWM
	-This would make the PWM only send out one pulse
*/

/*THE TIMER / PWM INTERRUPT IS COMPARE MATCH INTERRUPT:
The analog read starts the timer/pwm clock (if under threshold)
The compare/match interrupt stops the timer/pwm clock (in the clock select register)
(I didn't need to reset the clock manually, it seemed to do that on its own)
*/

/*
WHAT I NEED:
1. ADC in freerun mode with conversion complete interrupt
	-On interrupt trigger:
		-Compare the analog read value with threshold value
		-If below threshold, start PWM clock (PWM interrupt is already enabled in setup)
		-This sets the "PWM started" flag
	-This interrupt would start/stop the Timer/PWM's clock (in the clock select section)

2. Timer/PWM COMPARE MATCH INTERRUPT configured, with interrupt enabled but clock disabled
	-When PWM is started, the GATE is set HIGH ("automatically") and the gauss rifle fires
	-It was inverted so the PWM started HIGH and on match go LOW and trigger the compare match interrupt
	-On COMPARE MATCH, the PWM clock would be stopped
	-(I did not need to reset the timer's counter, it seemed to do that by itself when the clock was stoppped)
*/

/*OTHER NOTES I WANT TO TYPE OUT:
-The "gauss rifle fired" flag needs to be set when under a threshold, and cleared when OVER the threshold:
		THRESHOLD + BUFFER
-The analog read is noisy when it is going fast so I need a buffer so it does not re-trigger
-The ADC is in free running mode so the interrupt is going off like wildfire
-The Timer/PWM match/compare interrupt is always enabled, but the clock is diabled a majority of the time
-The ADC starts the Timer/PWM clock when under the threshold
-The compare/match interrupt stops the timer/pwm clock so the PWM only lasts for one pulse
-Then the flag would be reset
*/

//Finding out setup() and loop() were just part of the Arduino library made me uncomfortable...
//An empty sketch is only like 150 bytes what the hell

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