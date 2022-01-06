/****************************************************************************
**	INCLUDE
****************************************************************************/

//Boilerplate. Inhibit implementation when calling
#define USER_INIBITH_IMPLEMENTATION


//type definition using the bit width and sign
#include <stdint.h>
//define the ISR routune, ISR vector, and the sei() cli() function
#include <avr/interrupt.h>
//name all the register and bit
#include <avr/io.h>
//General purpose macros
#include "at_utils.h"
//AT4809 PORT macros definitions
#include "at4809_port.h"
//Common definitions
#include "global.h"
//hard coded delay
#include <util/delay.h>

#include "uart.h"

#include "servo.h"

/****************************************************************************
**GLOBAL VARS
****************************************************************************/

	///----------------------------------------------------------------------
	///	UART DRIVER
	///----------------------------------------------------------------------

//Instance of driver for the UART0. It provides initialization, circular buffers and ISR handling
extern User::Uart gcl_uart0;

	///----------------------------------------------------------------------
	///	SERVO DRIVER
	///----------------------------------------------------------------------

//Instance of the servo controller class
extern OrangeBot::Servo gc_servo;

/****************************************************************************
** INTERRUPT SERVICE ROUTINE
*****************************************************************************
**	In the AT4809 ISR flags have to be cleared manually
****************************************************************************/

/***************************************************************************/
//! @brief ISR
//! \n RTC_PIT_vect
/***************************************************************************/
//! @return void
//! @details
//! \n Periodic interrupt generated by the RTC from it's independent clock source
/***************************************************************************/

ISR( RTC_PIT_vect )
{	
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------
	//Local prescaler
	
	//if: tick is not issued
	if (g_isr_flags.fast_tick == false)
	{
		//Issue a tick
		g_isr_flags.fast_tick = true;
	}
	//if: slow tick is already issued
	else
	{
		//ERR OVERRUN
		//The main application could not keep up with the previous slow tick. Raise the alarm
		ISSUE_ERROR(ge_error_code, Error_code::ERR_OVERRUN_FAST_TICK);
	}
	
	//----------------------------------------------------------------
	//	SLOW TICK
	//----------------------------------------------------------------
	//	Slow tick meant to handle slow services like LEDs
	
	//Prescaler
	static uint16_t pre_slow = 0;	
	//If: Tick
	if (pre_slow == 0)
	{
		//if: tick is not issued
		if (g_isr_flags.slow_tick == false)
		{
			//Issue a tick
			g_isr_flags.slow_tick = true;	
		}
		//if: tick is already issued
		else
		{
			//ERR OVERRUN
			//The main application could not keep up with the previous slow tick. Raise the alarm
			ISSUE_ERROR(ge_error_code, Error_code::ERR_OVERRUN_SLOW_TICK);
		}
		
	}
	//Increment, reset if counter is top before increment
	pre_slow = AT_TOP_INC( pre_slow, Prescaler::TOP_SLOW_TICK-1 );
	
	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------
	
	//Manually clear the interrupt flag
	RTC.PITINTFLAGS = RTC_PI_bm;
}

/***************************************************************************/
//! @brief ISR
//! \n USART0_RXC_vect
/***************************************************************************/
//! @return void
//! @details
//! \n initialize clock systems
/***************************************************************************/

ISR( USART0_RXC_vect )
{
	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------
	
	//Execute the UART handler RX
	bool u1_fail = gcl_uart0.isr_rx_exe();
	
	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------
	
}	//End ISR: USART0_RXC_vect

/***************************************************************************/
//! @brief HAL ISR
//! \n TCA0_OVF_vect | void
/***************************************************************************/
//! @return void
//! @details
//! \n	TA0 Overflow Interrupt
//! \n	TA0 is set as a timeout timer.
//! \n	Every time the timer expires, the driver either rearms it or doesn't
//! \n	The ISR scans in sequence all the Servomotors to generate 16b PPM signals
//! \n		Algorithm:
//! \n	Allowed to run?
//! \n	N: stop timer
//! \n	Next Servo
//! \n	>Set appropriate Servo IO
//! \n	>First Servo: Compute OCR from target, actual and speed
//! \n	>Last Servo Done: set PER to the time needed to do 20ms
//! \n	>Other Servo: Set OCR of said servo
/***************************************************************************/

ISR( TCA0_OVF_vect )
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------
	
	//Execute the servo driver ISR
	gc_servo.hal_timer_isr();
	
	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------
	
	//Manually clear the interrupt flag
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}
