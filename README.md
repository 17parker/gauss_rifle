A simple coilgun (gauss gun) controller program

An LED, and a photoresistor across from it, detect when the barrel is obscured. When the voltage across the photoresistor drops, the coil is activated for a precise amount of time (a little over 2ms). The coil is powered by a 50 volt, 60,000 microfarad capacitor bank, and power to it is managed with 3 power MOSFETS, each rated for a few hundred continuous amps. 

Voltage is read using the built-in analog to digital converter that triggers an interrupt whenever it finishes sampling. If the voltage is below a threshold, it starts a timer (PWM). The timer (PWM) triggers an interrupt when it completes one period. This interrupt disables the timer, so that only one pulse is sent.
