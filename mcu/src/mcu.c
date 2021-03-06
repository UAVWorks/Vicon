/**
 * \file   mcu.c
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
 * Main file for the ATMEGA328P Mode Control Unit
 */

#include "avrdefines.h"
#include "MCUCommands.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "pwm.h"
#include "pulsecapture.h"
#include "usart.h"
#include "modeindicator.h"
#include "mode.h"

/**
 * @brief Pack of all Initialise Routines
 */
void init();

/**
 * @brief Combine High Level Commands
 *
 * Depending on the selected flight mode:
 * 
 * - Pass the RC Commands without modification
 * - Augment the RC commands with AP commands
 * - Pass the AP Commands without modification 
 */
void CombineCommands();


int main (void)
{
  uint8_t inputsInitialised = 0;
  StopPWM();
  init();
  sei();

  StartPWM();
  // Initialise and detect the Inputs
  uint8_t loopCount = 0;
  do 
  {
    loopCount++;
    ToggleRed(TOGGLE);
    _delay_ms(250);
    // dual rates need to be off
    if ((inputChannel[inputsInitialised].measuredPulseWidth < (1300)) && (inputChannel[inputsInitialised].measuredPulseWidth != 0))
    {
      inputsInitialised++;
    }
    if (loopCount > 6)
    {
      loopCount = 0;
      inputsInitialised = 0;
    }
  } while (inputsInitialised < NUM_CHANNELS);
  #ifdef DEBUG
  stdout = &debugOut;
  printf("System Initialised\n");
  #endif

  // Calibrate Neutral Pulse Widths
  _delay_ms(1000);
  
  // Start Output
  StartPWM();

// Minimum Outputs
#ifdef _GYRO_
        ESC1_COUNTER = PWMToCounter(zeroThrottle);
  	ESC2_COUNTER = PWMToCounter(zeroRoll);
  	ESC3_COUNTER = PWMToCounter(zeroPitch);
  	ESC4_COUNTER = PWMToCounter(zeroYaw);
        _delay_ms(750);
#else
        _delay_ms(750);
  	ESC1_COUNTER = PWMToCounter(1000);
        _delay_ms(750);
  	ESC2_COUNTER = PWMToCounter(1000);
        _delay_ms(750);
  	ESC3_COUNTER = PWMToCounter(1000);
        _delay_ms(750);
  	ESC4_COUNTER = PWMToCounter(1000);
#endif
  
  for ( ; ; )
  {
    // RC Pulse Capture
    if(newRC) // new RC values to process
    {
      newRC = 0;
      UpdateRC();
      if(failSafe) // Never Recover from Failsafe 
      {
        static uint8_t toggleEnabled = 0;
        
        // Minimum Commands
#ifdef _GYRO_
        ESC1_COUNTER = PWMToCounter(zeroThrottle);
  	ESC2_COUNTER = PWMToCounter(zeroRoll);
  	ESC3_COUNTER = PWMToCounter(zeroPitch);
  	ESC4_COUNTER = PWMToCounter(zeroYaw);
#else
  	ESC1_COUNTER = PWMToCounter(1000);
  	ESC2_COUNTER = PWMToCounter(1000);
  	ESC3_COUNTER = PWMToCounter(1000);
  	ESC4_COUNTER = PWMToCounter(1000);
#endif
  
        if (!toggleEnabled) // prepare to go into failsafe
        {
	  _delay_ms(100);
          toggleEnabled = 1;
          ToggleRed(OFF);
          ToggleGreen(OFF);
          ToggleBlue(OFF);
	  _delay_ms(100);
	  flightMode = FAIL_SAFE;
	}
        else  // Toggle Yellow Error at 10Hz
        {
	  _delay_ms(100);
          ToggleGreen(TOGGLE);
          ToggleRed(TOGGLE);
        }
      }
      else
      {	      
        // Combine RC and AP inputs
        // Then Output them
        newAPCommands = 0;
        CombineCommands();
      }
    }
    else if (newAPCommands) // only new autopilot commands
    {
      newAPCommands = 0;
      CombineCommands();
    }

    // Communicate with Flight Computer
    if (txCommands)
    {
      txCommands = 0;
      USARTtxCommands();
    }

    if (txPeriodic)
    {
      txPeriodic = 0;
      USARTtxPeriodic();
    }

    if (txRCCommands)
    {
      txRCCommands = 0;
      USARTtxRCCommands();
    }
  }
  return 0;
}


inline void init()
{
  // Setup Clock - prescalar of 2 to 8MHz
  CLKPR = (1<<CLKPCE);
  CLKPR = (1<<CLKPS0);
  
  InitialiseModeIndicator();
  InitialiseTimer0();
  InitialiseTimer1();

  InitialiseUSART();

  InitialiseTimer2();
  InitialisePC();

  return;
}

