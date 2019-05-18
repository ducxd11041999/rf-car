/*
* ----------------------------------------------------------------------------
* “THE COFFEEWARE LICENSE” (Revision 1):
* <ihsan@kehribar.me> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a coffee in return.
* -----------------------------------------------------------------------------
*/

#include <avr/io.h>
#include <stdint.h>
#include "nrf24.h"
#include "my_library.h"
#include "function.h"
#include <stdlib.h>

/* ------------------------------------------------------------------------- */
/* Software UART routine. */
/* 9600 bps. Transmit only */
/* Transmit pin is: B2 */
/* ------------------------------------------------------------------------- */
/* Hardware specific section ... */
/* ------------------------------------------------------------------------- */
#define F_CPU 16000000UL
#include <util/delay.h>
#include "USART_RS232_H_file.h"
/* ------------------------------------------------------------------------- */
uint8_t temp;
uint8_t data_array[2];
uint8_t tx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
uint8_t rx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
void binary_print (uint8_t x);
void process_message();
/* ------------------------------------------------------------------------- */
int main()
{
    /* init the software uart */
    USART_Init(9600);
	USART_SendString("=>>start\n");
    /* init hardware pins */
    nrf24_init();
    
    /* Channel #2 , payload length: 4 */
    nrf24_config(2,2);
 
    /* Set the device addresses */
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);
	
	INIT();
	handle(0);
	speed(0,0,0);
    while(1)
    {    
        if(nrf24_dataReady())
        {
			nrf24_getData(data_array);
            process_message();
        }
    }
}
/* ------------------------------------------------------------------------- */
void binary_print (uint8_t x) {
	for (int i=0; i<8 ; i++) {
		if(bit_is_set(x,i)) {
			USART_TxChar('1');
		}
		else USART_TxChar('0');
	}
	USART_TxChar('\n');
}
void process_message() {
	//Debug============================================
	//USART_SendString("==>");
	//binary_print(data_array[0]);
	//USART_TxChar('\n');
	//uint8_t stick=(data_array[1] & (0x0F));
	//uint8_t speed_value = (data_array[1] & 0xF0) >> 4;
	//char ch[5];
	//itoa(stick,ch,10);
	//USART_SendString(ch);
	//USART_TxChar('\n');
	//itoa(speed_value,ch,10);
	//USART_SendString(ch);
	//USART_TxChar('\n');
	//=====================================================*/
	uint8_t button = data_array[0];
	uint8_t stick=(data_array[1] & (0x0F));
	float speed_value = ((float)((data_array[1] & 0xF0) >> 4))/15.0*100.0;
	//==================button=================================
	if(bit_is_set(button,0)) {
		speed(255,255,speed_value);
		//BT0: tien
	}
	if(bit_is_set(button,1)) {
		speed(-255,-255,speed_value);
		//BT1: lui
	}
	if(bit_is_set(button,2)) {
		cbi(PORT_DIR0,DIR00);
		sbi(PORT_DIR0,DIR01);
		OCR2=255;
		//BT2: phanh trai
	}
	if(bit_is_set(button,3)) {
		cbi(PORT_DIR1,DIR10);
		sbi(PORT_DIR1,DIR11);
		OCR1B=40000;
		//BT3: phanh phai
	}
	if(bit_is_set(button,4)) {
		handle(-150);
		speed(-100,255,100);
		//BT4: mode tuy chon, trai
	}
	if(bit_is_set(button,5)) {
		handle(150);
		speed(255,-100,100);
		//BT5: mode tuy chon, phai
	}
	//===========joystick=================
	switch(stick) {
		case 8:
			handle(0);
			break;;
		case 0: //<--
			handle(-100);
			//speed
			break;
		case 1: //-->
			handle(100);
			//speed
			break;
		case 2: //tien trai (goc vua)
			handle(-60);
			//speed
			break;
		case 3: //lui phai (goc nho)
			handle(20);
			//speed
			break;
		case 4: //tien phai (goc vua)
			handle(60);
			//speed
			break;
		case 5: //lui trai (goc nho)
			handle(-20);
			//speed
			break;
		case 6: //tien (max toc)
			handle(0);
			speed(255,255,100);
			break;
		case 7: //lui (tuy chon)
			handle(100);
			speed(-255,100,100);
			break;
		default:
			handle(0);
			speed(0,0,0);
			break;
	}
}