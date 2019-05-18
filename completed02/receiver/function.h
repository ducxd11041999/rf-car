#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DDR_MOTOR_L DDRD
#define DDR_MOTOR_R DDRD
#define DDR_SERVO DDRD

#define PORT_MOTOR_L PORTD
#define PORT_MOTOR_R PORTD
#define PORT_SERVO PORTD
#define MOTOR_L 4
#define MOTOR_R 7
#define SERVO 5

#define DDR_DIR0 DDRC
#define PORT_DIR0 PORTC
#define DDR_DIR1 DDRD
#define PORT_DIR1 PORTD
#define DIR00	4
#define DIR01	5
#define DIR10	3
#define DIR11   6

#define true 1
#define false 0
#define SERVO_CENTER		2550	//Sai số của cần sensor trên xe (am se dich qua trai, duong se dich qua phai)
#define ANGLE_MAX           150
#define STEP				5

//================RATIO + SERVO + MOTOR ================
void speed(float left, float right, float persent) { //max left la 255, max right la 255
	left*=(persent/100);
	right*=(persent/100);
	if (right>=0) {
		sbi(PORT_DIR1,DIR10);
		cbi(PORT_DIR1,DIR11);
		if (right*157>40000) OCR1B=40000;
		else
			OCR1B=(int)right*157;
	}
	else {
		cbi(PORT_DIR1,DIR10);
		sbi(PORT_DIR1,DIR11);
		if (right*157<-(40000)) OCR1B=-40000;
		else
			OCR1B=(int)-right*157;
	}
	if (left>=0) {
		sbi(PORT_DIR0,DIR00);
		cbi(PORT_DIR0,DIR01);
		if(left>255) left=255;
		OCR2=(int)left;
	}
	else {
		cbi(PORT_DIR0,DIR00);
		sbi(PORT_DIR0,DIR01);
		if(left<-255) left=-255;
		OCR2=-(int)left;
	}
}
void handle(float goc)
{
	if (goc>ANGLE_MAX) goc=ANGLE_MAX;
	if (goc<-ANGLE_MAX) goc=-ANGLE_MAX;
	OCR1A=SERVO_CENTER+(int)goc*STEP; //duong phai, am trai
									//45* =1000;
}

void INIT()
{
	sbi(DDR_SERVO,SERVO);
	DDR_DIR1|=(1<<DIR11)|(1<<DIR10);
	DDR_DIR0|=(1<<DIR01)|(1<<DIR00);
	sbi(DDR_MOTOR_L,MOTOR_L);
	sbi(DDR_MOTOR_R,MOTOR_R);
	sbi(PORT_DIR0,DIR00);
	sbi(PORT_DIR1,DIR10);
		
	TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);			// SET OCR1A & OCR1B at BOTTOM, CLEAR at Compare Match (Non-invert), Mode 14 Fast PWM
	TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);				// Prescaler = 8
	ICR1 = 40000;										// Time Period = 20ms
	
	TCCR2=(1<<WGM20)|(1<<WGM21)|(1<<COM21)|(1<<CS22)|(1<<CS21)|(1<<CS20);  //SET OC2 at BOTTOM, CLEAR OC2 on compare match,(non-invert), Mode 3 Fast PWM,  Prescaler = 1024
	OCR2=0;
	sei();
}