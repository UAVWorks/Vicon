/**
* \file   usart.c
* \author Tim Molloy
*
* $Author$
* $Date$
* $Rev$
* $Id$
*
* Queensland University of Technology
*
* \section DESCRIPTION
* Implementation of the USART functions for the MCU
*/

#include "avrdefines.h"
#include "MCUCommands.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "mode.h"
#include "pulsecapture.h"

volatile uint32_t apLastCommands;
volatile uint8_t newAPCommands;
volatile uint8_t txCommands;
volatile uint8_t txRCCommands;
volatile uint8_t txPeriodic;


//FILE debugOut = FDEV_SETUP_STREAM(USARTtxChar, NULL,_FDEV_SETUP_WRITE);

uint8_t InitialiseUSART()
{
	uint8_t bRet = 1;
	uint16_t baudRateRegister = (uint16_t) (F_CPU/(16.0 * BAUD_RATE) - 1.0 + 0.5);

	// Control and Status Register A
	// U2xn = 1 for double speed
	// UCSR0A = (1 << U2X0);

	// Control and Status Register B
	// Rx Interrupt Enabled
	// Rx and Tx Enabled
	// UCSZ0_210 = 011 for 8 bit data
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (0 << UCSZ02) | (1 << RXCIE0);

	// Control and Status Register C
	// UMSEL0_10 = 00 for Asynchronous
	// UPM0_10 = 00 for No Parity
	// USBS0 = 1 for 2 Stop Bit
	// UCSZ0_210 = 011 for 8 bit data
	UCSR0C = (0 << UPM01) | (0 << UPM00) | (1 << UCSZ01) | (1 << UCSZ00) | (0 << USBS0);

	// Usart Baud Rate Register
	// 8 for 57600 @ 8 MHz
	// 3 for 115.2k @ 8 MHz
	// 1 for 250k @ 8MHz
	UBRR0H = (unsigned char) (baudRateRegister >> 8);
	UBRR0L =  (unsigned char) (baudRateRegister);

	return bRet;
}

void USARTtxChar(char txChar, FILE *outStream)
{
	if (txChar == '\n')
	{
		USARTtxChar('\r', outStream);
	}

	while (!BRS(UCSR0A,UDRE0)); // TODO wait with interrupt
	UDR0 = txChar;

	return;
}

void USARTtxData(volatile unsigned char txChar)
{
	while (!BRS(UCSR0A,UDRE0)); // TODO wait with interrupt
	UDR0 = txChar;

	return;
}

/**
* @brief USART Receive and Parse Data
*/
ISR(USART_RX_vect)
{
	// State of Machine
	static uint8_t rxState = HEADER_SYNC;
	uint8_t tempData = UDR0;
	// Temp Variables for SEND_MCU_COMMANDS
	static enum FlightModes tempMode = MANUAL_DEBUG;
	static uint8_t tempHeader = 255;
	static int8_t tempThrottle = 0, tempRoll = 0, tempPitch = 0, tempYaw = 0;

	switch (rxState)
	{
	case HEADER_SYNC: // wait for header sync char
		tempHeader = 255;
		if (tempData == FRAME_CHAR)
		{
			rxState = HEADER_TYPE;
		}
		break;
	case HEADER_TYPE: // got first char, see if the header is here
		if ((tempData == GET_MCU_COMMANDS) || (tempData == GET_MCU_PERIODIC) || (tempData == SEND_MCU_COMMANDS))
		{
			tempHeader = tempData;
			rxState = HEADER_COMPLETE;
		}
		else
		{
			rxState = HEADER_SYNC;
		}
		break;
	case HEADER_COMPLETE:
		if (tempData == FRAME_CHAR) // header was valid
		{
			if (tempHeader == GET_MCU_COMMANDS)// Request sent for Commands
			{
				txCommands = 1;
				rxState = HEADER_SYNC;
			}
			else if (tempHeader == GET_MCU_PERIODIC) // request for mode and engine data
			{
				txPeriodic = 1;
				rxState = HEADER_SYNC;
			}
			else if (tempHeader == SEND_MCU_COMMANDS) // incoming parameters
			{
				rxState = RX_MODE;
			}
			else if (tempHeader == GET_MCU_RC_COMMANDS) // send RC Commands
			{
				txRCCommands = 1;
				rxState = HEADER_SYNC;
			}
			else // unknown
			{
				rxState = HEADER_SYNC;
			}
		}
		else // header not valid
		{
			rxState = HEADER_SYNC;
		}
		break;
	case RX_MODE:
		tempMode = tempData;
		rxState = RX_THROTTLE;
		break;
	case RX_THROTTLE:
		tempThrottle = tempData;
		rxState = RX_ROLL;
		break;
	case RX_ROLL:
		tempRoll = tempData;
		rxState = RX_PITCH;
		break;
	case RX_PITCH:
		tempPitch = tempData;
		rxState = RX_YAW;
		break;
	case RX_YAW:
		tempYaw = tempData;
		rxState = RX_COMPLETE;
		break;
	case RX_COMPLETE:
		if (tempData == FRAME_CHAR)
		{
			newAPCommands = 1;
			apLastCommands = micro();

			apMode = tempMode;
			apThrottle = tempThrottle;
			apRoll = tempRoll;
			apPitch = tempPitch;
			apYaw = tempYaw;
		}
		rxState = HEADER_SYNC;
	}
}

inline void USARTtxCommands()
{
	USARTtxData(commandedThrottle);
	USARTtxData(commandedRoll);
	USARTtxData(commandedPitch);
	USARTtxData(commandedYaw);
}

inline void USARTtxRCCommands()
{
	USARTtxData(rcThrottle);
	USARTtxData(rcRoll);
	USARTtxData(rcPitch);
	USARTtxData(rcYaw);
}

inline void USARTtxPeriodic()
{
	volatile unsigned char tempEngine = 0;

	USARTtxData(flightMode);
	USARTtxData(rcThrottle + PWMToCounter(zeroThrottle));
	USARTtxData(rcRoll + PWMToCounter(zeroRoll));
	USARTtxData(rcPitch + PWMToCounter(zeroPitch));
	USARTtxData(-rcYaw + PWMToCounter(zeroYaw));

	/* USARTtxData(flightMode);
tempEngine = ESC1_COUNTER;
USARTtxData(tempEngine);

tempEngine = ESC2_COUNTER;
USARTtxData(tempEngine);

tempEngine = ESC3_COUNTER;
USARTtxData(tempEngine);

tempEngine = ESC4_COUNTER;
USARTtxData(tempEngine);
*/
	return;
}
