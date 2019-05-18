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
#include <stdlib.h>
#include "nrf24.h"
#include "function.h"

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
/*--------------------------------------------------------------------------*/
uint8_t temp;
uint8_t data_array[2];
uint8_t tx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
uint8_t rx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
void prepair_data ();
//void binary_print (uint8_t x) {
	//for (int i=0; i<8 ; i++) {
		//if(bit_is_set(x,i)) {
			//USART_TxChar('1');
		//}
		//else USART_TxChar('0');
	//}
	//USART_TxChar('\n');
//}
/* ------------------------------------------------------------------------- */
int main()
{
    /* init the software uart */
    USART_Init(9600);
    /* init hardware pins */
    nrf24_init();
    
    /* Channel #2 , payload length: 10 */
    nrf24_config(2,2);

    /* Set the device addresses */
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);    
	INIT();
    while(1)
    {              
		prepair_data();                                   
        /* Automatically goes to TX mode */
        nrf24_send(data_array);        
        
        /* Wait for transmission to end */
        while(nrf24_isSending());
        /* Make analysis on last tranmission attempt */
        temp=nrf24_lastMessageStatus();
		if(temp==NRF24_TRANSMISSON_OK) {
			USART_SendString("send ok\n");
		}
		//power down tx
		//nrf24_powerDown();  
	}
}
/* ------------------------------------------------------------------------- */
void prepair_data (){
	data_array[0]=0;
	data_array[1]=8; //8 o mode thuong
	//BT0 tien
	if(get_button(BTN0)) data_array[0]|=(1<<0);
	//BT1 lui
	if(get_button(BTN1)) data_array[0]|=(1<<1);
	//BT2 phanh trai gap
	if(get_button(BTN2)) data_array[0]|=(1<<2);
	//BT3 phanh phai gap
	if(get_button(BTN3)) data_array[0]|=(1<<3);
	//BT4 tuy chon
	if(get_button(BTN4)) data_array[0]|=(1<<4);
	//BT5 tuy chon
	if(get_button(BTN5)) data_array[0]|=(1<<5);
	uint16_t ADC0=adc_read(0);
	uint16_t ADC1=adc_read(1);
	// <--
	if((ADC0<50) && (450<ADC1) && (ADC1<550)) data_array[1]=0;
	// -->
	if((ADC0>1000) && (450<ADC1) && (ADC1<550)) data_array[1]=1;
	// tien trai
	if((ADC0<50) && (ADC1>1000)) data_array[1]=2;
	// lui phai
	if((ADC0>1000) && (ADC1<50)) data_array[1]=3;
	// tien phai
	if((ADC0>1000) && (ADC1>1000)) data_array[1]=4;
	// lui trai
	if((ADC0<50) && (ADC1<50)) data_array[1]=5;
	// tien
	if((450<ADC0) && (ADC0<550) && (ADC1>1000)) data_array[1]=6;
	//lui
	if((450<ADC0) && (ADC0<550) && (ADC1<50)) data_array[1]=7;
	//none
	if((490<ADC0) && (ADC0<530) && (480<ADC1) && (ADC1<530)) data_array[1]=8;
	//speed
	uint16_t ADC2=adc_read(2);
	data_array[1]|= (((int)((float)ADC2/1000.0*15.0)) << 4);
	//char ch[5];
	//itoa((data_array[1] & (0x0F)),ch,10);
	//USART_SendString(ch);
	//USART_TxChar('\n');
	return;
}