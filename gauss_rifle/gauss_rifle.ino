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

/*
WHAT I NEED:
1. ADC in freerun mode with conversion complete interrupt
	-On interrupt trigger:
		-Compare the analog read value with threshold value
		-If below threshold, start PWM clock (PWM interrupt is already enabled in setup)
		-This sets the "PWM started" flag
	-This interrupt would start/stop the Timer/PWM's clock (in the clock select section)
2. Timer/PWM configured, with interrupt enabled but clock disabled
	-When PWM is started, the GATE is set HIGH ("automatically") and the gauss rifle fires
	-
	-(I did not need to reset the timer's counter, it seemed to do that by itself when the clock was stoppped)

*/


int main() {
	//Enter setup code here


	while (1) {}
}