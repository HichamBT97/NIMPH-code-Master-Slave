/*
 * I2C_SLAVE_OLIMEX.c
 *
 * Created: 24/12/2020 23:14:15
 * Author : HP
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdint.h>

//#include "io_config.h"
#include "i2c_status.h"

#define __AVR_ATMEGA128__		1
#define OSCSPEED		16000000  // IN Hz
#define SLAVE_ADDR		0x09 //  SLAVE addressee 

/// DEBUG
/// @brief STOP DEBUG   ==> AVR_DEBUG_TWI 0
/// @brief ENABLE DEBUG ==> AVR_DEBUG_TWI 1
#define AVR_DEBUG_TWI 1

unsigned char CODE = 'N';	//  CODE FOR SEEING WHAT THE MASTER LOOKING FOR
uint8_t DATA[50] = {0} ;	// THE DATA WILL BE IN THIS BYTES NAMED DATA
unsigned char STATE = 'N';	// THIS VARIABLE WILL HAVE THE STATE OF MESURE ; DONNE (D)/NOT YET (N)
int CMPT = 0;				// THIS INTEGER ARE USED TO COUNT HOW MATCH DATA THERE ARE
int j = 0;		// pointer of the data case

void PORT_Init()
{
	PORTA = 0b00000000;		DDRA = 0b01000000;	//Relay set as output (Bit6 = 1)
	PORTB = 0b00000000;		DDRB = 0b00000000;
	PORTC = 0b00000000;		DDRC = 0b11110111;	//LCD connection with PORTC
	PORTD = 0b11000000;		DDRD = 0b00001000;	//TX set as output (Bit3 = 1)
	PORTE = 0b00000000;		DDRE = 0b00110000;	//Buzzer set as output (Bit4 = 1, Bit5 = 1)
	PORTF = 0b00000000;		DDRF = 0b00000000;
	PORTG = 0b00000000;		DDRG = 0b00000000;
}

#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
	void UART_Init(uint32_t Baud)
	{
		unsigned int BaudRate = OSCSPEED / (16 * Baud) - 1;	/* as per pg. 173 of the user manual */

		//set BaudRate to registers UBRR1H and UBRR1L
		UBRR1H = (unsigned char) (BaudRate>>8);
		UBRR1L = (unsigned char) BaudRate;

		UCSR1B = UCSR1B | 0b00011000;	//enable Receiver and Transmitter (Bit3 = 1, Bit4 = 1)

		UCSR1C = UCSR1C | 0b10000110;	//Set frame format: 8 data (Bit1 = 1, Bit2 = 1), 1 stop bit (Bit3 = 0)
	}

	void UART_Transmit(unsigned char data)
	{
		while (!(UCSR1A & 0b00100000));	//waiting until buffer is ready to receive

		UDR1 = data;
	}
#endif


void TWI_SLAVE_INIT()
{
	cli(); // arreter tout intteruption; desable general interrupt bit 'I' of SREG
	TWCR = 0X04;											 // ENABLE TWI
	TWAR = (SLAVE_ADDR << 1);								// address shifted a gauche d'un pas pour qu'il soit sur La bonne position
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE); // TWINT Cleared par la mise a 1
														  // TWEN : enable i2c
														 // TWEA : enable acknowledge
													    // TWIE : enable i2c interruption
}

ISR(TWI_vect)			// Vecteur d'interruption de bus i2c
{
	/******************** Slave Receiver Mode *********************/
	
	//  SLA+W received, ACK returned : 0x60
	if((TWSR & 0xF8) == SR_SLA_ACK) {
		
		#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
			for (unsigned char i = 0 ; i <= strlen(MSG1) ; i++)
			{
				UART_Transmit(MSG1[i]);
			}
		#endif
		
		TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN); // clear TWINT and prepare to receive new byte
	}
	
	// Arbitration lost in SLA+RW / SLA+W received, ACK returned : 0x68
	else if ((TWSR & 0xF8) == SR_ARB_LOST_SLA_ACK)
	{	
		#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
			for (unsigned char i = 0 ; i <= strlen(MSG2) ; i++)
			{
				UART_Transmit(MSG2[i]);
			}
		#endif
		
		TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
	// general call received, ACK returned : 0x70
	else if ((TWSR & 0xF8) == SR_GEN_CALL_ACK)
	{
		TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		
		#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
			for (unsigned char i = 0 ; i <= strlen(MSG3) ; i++)
			{
				UART_Transmit(MSG3[i]);
			}
		#endif
	}
	
	// Code received, ACK returned : 0x80 ; Previously addressed with own SLA+W
	else if ((TWSR & 0xF8) == SR_DATA_ACK)
	{
		CODE = TWDR;
		
		#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
			for (unsigned char i = 0 ; i <= strlen(MSG4) ; i++)
			{
				UART_Transmit(MSG4[i]);
			}
			
			UART_Transmit(CODE);
			
		#endif
		
		TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
	// data received, NACK returned : 0x88
	else if ((TWSR & 0xF8) == SR_DATA_NACK)
	{
		CODE = TWDR;
		#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
			for (unsigned char i = 0 ; i <= strlen(MSG5) ; i++)
			{
				UART_Transmit(MSG5[i]);
			}
			UART_Transmit(CODE);
	
		#endif
		
		TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
	// general call data received, ACK returned : 0x90
	else if ((TWSR & 0xF8) == SR_GEN_CALL_DATA_ACK)
	{
		CODE = TWDR;
		#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
			for (unsigned char i = 0 ; i <= strlen(MSG6) ; i++)
			{
				UART_Transmit(MSG6[i]);
			}
			UART_Transmit(CODE);
		
		#endif
		
		TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
	// general call data received, NACK returned : 0x98
	else if ((TWSR & 0xF8) == SR_GEN_CALL_DATA_NACK)
	{
		CODE = TWDR;
		#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
			for (unsigned char i = 0 ; i <= strlen(MSG7) ; i++)
			{
				UART_Transmit(MSG7[i]);
			}
			UART_Transmit(CODE);
		
		#endif
		
		TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
	
	/******************** Slave Transmitter Mode *********************/
	// SLA+R Received and ACK returned : 0xA8
	else if ((TWSR & 0xF8) == ST_SLA_ACK)	// MASTER DEMANDE A DATA, SO WE NEED TO PREPARE IT HERE
	{
		/*if (CODE == 'H')		// ONE CHECK; H FROM HELLO
		{
			TWDR = 'H';			// PREPARE THE RESPONSE ON THE REGISTER TWDR
		}
		
		else if (CODE == 'S')	// GET STATE OF MESURE 
		{
			TWDR = STATE;		// send the state of measure STATE = N(NOT YET) OR D(DONE) 
		}
		
		else if (CODE == 'G')	// GET THE DATA OF MESUREMENT ; PREPARE THE FIRST DATA HERE
		{
			CMPT -= 1;			// decrement CMPT by 1
			TWDR = DATA[j];		// charging the first data in TWDR; j = 0 evry time data is charged
			j += 1;				// increment j by 1 to point on the next case of data
		}
		
		else {
			TWDR = 'B';
		}*/
		
		switch (CODE){
			case 'H' :	TWDR = 'H';	break;
			case 'S' :	TWDR = STATE; break;
			case 'G' :	
				CMPT -= 1;			// decrement CMPT by 1
				TWDR = DATA[j];		// charging the first data in TWDR; j = 0 evry time data is charged
				j += 1;				// increment j by 1 to point on the next case of data
			; break;
			default : TWDR = 'E'; break;
		}
		
		#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
			for (unsigned char i = 0 ; i <= strlen(MSG9) ; i++)
			{
				UART_Transmit(MSG9[i]);
			}
		#endif
		TWCR |= (1<<TWEA) | (1<<TWEN) ;
		//TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
	// Arbitration lost in SLA+R/W ; SLA+W has been received; ACK has been returned : 0xB0
	else if ((TWSR & 0xF8) == ST_ARB_LOST_SLA_ACK)
	{
		#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
			for (unsigned char i = 0 ; i <= strlen(MSG2) ; i++)
			{
				UART_Transmit(MSG2[i]);
			}
		#endif
		TWCR |= (1<<TWEA) | (1<<TWEN) ;
		//TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
	// Data byte in TWDR has been transmitted; ACK has been received : 0xB8
	else if ((TWSR & 0xF8) == ST_DATA_ACK)
	{
		if (CMPT == 0)
		{
			TWDR = 'L';			// charge TWDR by 'L' to tell master that was the last data 
			j = 0;				// initiate the pointer
			CMPT = 0;
			
			#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
				for (unsigned char i = 0 ; i <= strlen(MSG12) ; i++)
				{
					UART_Transmit(MSG12[i]);
				}
			#endif
		}
		else{
			CMPT -= 1;			// decrement CMPT by 1
			TWDR = DATA[j];		// charging the first data in TWDR
			j += 1;				// increment j by 1 to point on the next case of data
		
			#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
				for (unsigned char i = 0 ; i <= strlen(MSG11) ; i++)
				{
					UART_Transmit(MSG11[i]);
				}
			#endif
		}
		
		TWCR |= (1<<TWEA) | (1<<TWEN) ;
		// TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
	// Data byte in TWDR has been transmitted; NOT ACK has been received : 0xC0
	else if ((TWSR & 0xF8) == ST_DATA_NACK)
	{
		
		#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
			for (unsigned char i = 0; i <= strlen(MSG13) ; i++)
			{
				UART_Transmit(MSG13[i]);
			}
		#endif
		TWCR |= (1<<TWEA) | (1<<TWEN) ;
		// TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
	// Data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received : 0xC8
	else if ((TWSR & 0xF8) == ST_DATA_ACK_TWEA0)
	{
		
		#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
			for (unsigned char i = 0; i <= strlen(MSG14) ; i++)
			{
				UART_Transmit(MSG14[i]);
			}
		#endif
		TWCR |= (1<<TWEA) | (1<<TWEN) ;
		// TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
	// A STOP condition or repeated START condition has been received while still addressed as Slave : 0xA0
	else if ((TWSR & 0xF8) == TW_STOP)
	{
		#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
			for (unsigned char i = 0; i <= strlen(MSG15) ; i++)
			{
				UART_Transmit(MSG15[i]);
			}
		#endif
		TWCR |= (1<<TWEA) | (1<<TWEN) ;

		//TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
	// if the code transmitted by the Master is not equal to one of the precedent condition
	else
	{
		#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0
			for (unsigned char i = 0; i <= strlen(MSG16) ; i++)
			{
				UART_Transmit(MSG16[i]);
			}
		#endif
		TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
}

int main(void)
{
	PORT_Init();										// initialisation des ports
	#if defined(AVR_DEBUG_TWI) && AVR_DEBUG_TWI > 0		//if debug defined then initiate UART
		UART_Init(9600);
	#endif
    TWI_SLAVE_INIT();									// initialization for i2c
	sei();												// Enable general interrupt bit
	
    while (1) 
    {
		if (CODE == 'M')
		{
			STATE = 'N';
			CMPT = 0;
			//UART_Transmit(STATE);
			for (j = 0 ; j < 40 ; j++)					// generate the measure
			{
				UART_Transmit('A');
				CMPT += 1;
				DATA[j] = CMPT;
			}
			j = 0;
			CODE = 'V';
			STATE = 'R';
			_delay_ms(5);
		}
    }
}

