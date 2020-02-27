/*	Author: steven
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "LCD_16x2_H_file.h"
#include "ADC_H.h"
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;  

unsigned long _avr_timer_M = 1; 
unsigned long _avr_timer_cntcurr = 0; 

void TimerOn() {
  TCCR1B = 0x0B;
  OCR1A = 125;
  TIMSK1 = 0x02;
  TCNT1 = 0;
  _avr_timer_cntcurr = _avr_timer_M;
  SREG |= 0x80;
}

void TimerOff() {
  TCCR1B = 0x00;
}

void TimerISR() {        
  TimerFlag = 1;
}  

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--; 
	if (_avr_timer_cntcurr == 0) { 
		TimerISR(); 
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned char _left = 0;

unsigned char _right = 0;

unsigned char _up = 0;

unsigned char _down = 0;

unsigned char led = 0x00;


//#define x ReadADC(0)

//#define y ReadADC(1)

unsigned short x;

unsigned short y;


void js(){

    x = ADC_Read(0);

    y = ADC_Read(1);

    if(x > 900){

        led = 0x08;

        _left = 1; //left

    }

    else if(x < 300){

        led = 0x04; //right

        _right = 1;

    }

    else if(y < 300){

        led = 0x02; //up

        _up = 1;

    }

    else if(y > 900 ){

        led = 0x01; //down

        _down = 1;

    }

    else{

        led = 0x00;

        _left = 0;

        _right = 0;

        _down = 0;

        _up = 0;

    }

}



int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	char buffer[20];
	int ADC_Value;
	TimerSet(1);
	TimerOn();
	
	ADC_Init();							/* ADC initialize function */
	LCD_Init();							/* LCD initialize function */

	while(1){
		ADC_Value = ADC_Read(0);		/* Read the status on X-OUT pin using channel 0 */
		sprintf(buffer, "X=%d   ", ADC_Value);
		LCD_String_xy(1, 0, buffer);
		
		ADC_Value = ADC_Read(1);		/* Read the status on Y-OUT pin using channel 0 */
		sprintf(buffer, "Y=%d   ", ADC_Value);
		LCD_String_xy(1, 8, buffer);

		ADC_Value = ADC_Read(2);		/* Read the status on SWITCH pin using channel 0 */
		/*if(ADC_Value < 600)
			LCD_String_xy(2, 0, "Switch pressed   ");
		else
			LCD_String_xy(2, 0, "Switch open      ");
			*/
		sprintf(buffer, "Z=%d   ", ADC_Value);
		LCD_String_xy(2, 0, buffer);
		js();

		/*switch(direction) { //didnt work
			case '1':
				led = 0x01;
				break;
			case '2':
				led = 0x02;
				break;
			case '3':
				led = 0x04;
				break;
			case '4':
				led = 0x08;
				break;
			case '0':
				led = 0x00;
				break;
			default:
				break;
		} */

		PORTB = led;
		while(!TimerFlag){
			TimerFlag = 0;}
}
}
