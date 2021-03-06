

#include "lcd.h"
#include "timer.h"
#include <inc/tm4c123gh6pm.h>
#include <stdbool.h>
#include "ping.h"
#include <math.h>


volatile unsigned int rising_time; // start time of the return pulse
volatile unsigned int falling_time; // end time of the return pulse

/*
* float ping() Sends a pulse out with the sonar sensor and returns the distance in centimeters.
* @return float value of the distance of the object.
*/
float ping(){
	uint32_t avg_time = 0, dist_cm;
	send_pulse();
	avg_time += ping_read();
	timer_waitMillis(10);
	send_pulse();
	avg_time += ping_read();
	timer_waitMillis(10);
	send_pulse();
	avg_time += ping_read();
	avg_time /= 3;

	dist_cm = time2dist(avg_time);
	return ((float)dist_cm);
}

float ping_raw(){
	uint32_t avg_time = 0, dist_mm;
	send_pulse();
	avg_time += ping_read();
	timer_waitMillis(10);
	send_pulse();
	avg_time += ping_read();
	timer_waitMillis(10);
	send_pulse();
	avg_time += ping_read();
	avg_time /= 3;

	return (float) avg_time;
}

/**
* void TIMER3B_Handler(void) ping sensor interrupt handler related to ISR.
*/
void TIMER3B_Handler(void){
	TIMER3_ICR_R |= TIMER_ICR_CBECINT;
	int event_time = TIMER3_TBR_R;
	if (state == LOW){
		state = HIGH;
		rising_time = event_time;
	} else if(state == HIGH){
		state = DONE;
		falling_time = event_time;
	}
}

/** 
* void send_pulse() Sends out a pulse with the sonar sensor on PB3.
*/
void send_pulse(){
	TIMER3_CTL_R &= ~( TIMER_CTL_TBEN); //disable timerB to allow us to change settings
	GPIO_PORTB_PCTL_R &= ~(0x7000);

	GPIO_PORTB_AFSEL_R &= ~BIT3; // change to GPIO mode
	GPIO_PORTB_DIR_R |= 0x08; // set PB3 as output
	GPIO_PORTB_DATA_R |= 0x08; // set PB3 to high

	timer_waitMicros(5); // wait 5 us

	GPIO_PORTB_DATA_R &= 0xF7; // set PB3 to low

	GPIO_PORTB_DIR_R &= 0xF7; // set PB3 as input
	GPIO_PORTB_AFSEL_R |= BIT3; // change to peripheral input mode

	GPIO_PORTB_PCTL_R |= 0x00007000;
	TIMER3_CTL_R |= TIMER_CTL_TBEN;

}

/**
* unsigned int time2dist(unsigned int time) convert time in clock counts to single-trip distance in mm
* @return int value of single-trip distance in mm
*/
unsigned int time2dist(unsigned int time){
	//return 0.011*time;
	return (float) ((0.0011*time) - 0.9082);


}

/**
* unsigned int ping_read() start and read the ping sensor once, return distance in mm
* @return Integer value of the ping's return distance in mm
*/
unsigned int ping_read(){

	while (state != DONE);
	state = LOW;
	if(rising_time > falling_time)
		return pow(2, 24) - 1 - rising_time + falling_time;
	return falling_time - rising_time;

}

/**
* void pulse_init() Initializes the correct GPIO ports for sending a pulse.
*/
void pulse_init(){
	SYSCTL_RCGCGPIO_R |= BIT1;
	GPIO_PORTB_AFSEL_R &= ~BIT3;
	GPIO_PORTB_DEN_R |= 0x08;
}

/**
* void timer_init() Initializes the correct registers and ports for TIMER3 for sending a pulse, and starting an interrupt service routine with a handler.
*/
void timer_init(){
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3; // Turn on clock to TIMER3

	//Configure the timer for input capture mode
	TIMER3_CTL_R &= ~( TIMER_CTL_TBEN); //disable timerB to allow us to change settings
	TIMER3_CFG_R |= TIMER_CFG_16_BIT; //set to 16 bit timer

	TIMER3_TBMR_R = 0x0017;//TIMER_TBMR_TBMR_PERIOD; //set for periodic mode, down count

	TIMER3_TBPR_R = 0;//TIMER3B_PRESCALER - 1;  // 200 ms clock

	TIMER3_CTL_R |= TIMER_CTL_TBEVENT_BOTH;

	TIMER3_TBILR_R |= 0xFFFF; // set the load value for the 0.2 second clock

	//clear TIMER3B interrupt flags
	TIMER3_ICR_R = (TIMER_ICR_TBTOCINT | TIMER_ICR_CBMCINT | TIMER_ICR_CBECINT); //clears TIMER3 time-out interrupt flags
	TIMER3_IMR_R |= (TIMER_IMR_CBEIM); //enable TIMER3(A&B) time-out interrupts

	//initialize local interrupts
	NVIC_EN1_R |= 0x00000010; //#warning "enable interrupts for TIMER3A and TIMER3B" n = 0, 1, 2, 3, or 3
	//go to page 105 and find the corresponding interrupt numbers for TIMER3 A&B
	//then set those bits in the correct interrupt set EN register (p. 132)

	IntRegister(INT_TIMER3B, TIMER3B_Handler); //register TIMER3B interrupt handler

	IntMasterEnable(); //intialize global interrupts

	// (TIMER_CTL_TBEN); //Enable TIMER3A & TIMER3B
}

