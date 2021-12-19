/****************************************************************
**	OrangeBot Project
*****************************************************************
**        /
**       /
**      /
** ______ \
**         \
**          \
*****************************************************************
**	OrangeHat
*****************************************************************
**  This firmware runs on the OrangeHat and is meant to test peripherals
**
**	Compiler flags: -std=c++11 -fno-threadsafe-statics -fkeep-inline-functions
**		-Os							|
**		-std=c++11					|
**		-fno-threadsafe-statics		| disable mutex around static local variables
**		-fkeep-inline-functions		| allow use of inline methods outside of .h for PidS16
****************************************************************/

/****************************************************************
**	DESCRIPTION
****************************************************************
**	OrangeHat test blink
****************************************************************/

/****************************************************************
**	HISTORY VERSION
****************************************************************
**		2021-08-19
**	Initial Release
**		2021-12-19
**	Import AT324 LCD16x2 Library for controller KS0066U with CDM1602K lcd display with backlight
**	Refactor the library to make use of the embedded string class
**	Refactor the library to use AT4809 IO
**	Compiles with no error
**	Power ON and OFF the display
**	Show a welcome message on the LCD on power ON: SUCCESS
**	Show a number that changes twice a second: SUCCESS
**	TODO: I learned a lot about C++ since I wrote the lcd library. Rewrite the LCD library as class.
****************************************************************/

/****************************************************************
**	USED PINS
****************************************************************
**	VNH7040
**				|	DRV0	|	DRV1	|	DRV2	|	DRV3	|	VNH7040
**	-------------------------------------------------------------------------
**	uC_SEN		|	PF0		|	PF0		|	PF0		|	PF0		|	SENSE ENABLE
**	uC_DIAG		|	PF1		|	PF1		|	PF1		|	PF1		|	SEL1
**	uC_PWM		|	PA2,B20	|	PA3,B21	|	PB4,B22	|	PB5,B23	|	PWM
**	uC_CTRLA	|	PA4		|	PA6		|	PB2		|	PD6		|	INA, SEL0
**	uC_CTRLB	|	PA5		|	PA7		|	PB3		|	PD7		|	INB
**
**	LEDs			|	Microcontroller
**	uc.led.green	|	uc.C4
**	uc.led.blue		|	uc.C5
**
**	ADC			|	Microcontroller
**	uc.led.green	|	uc.C4
**	uc.led.blue		|	uc.C5


****************************************************************/

/****************************************************************
**	KNOWN BUGS
****************************************************************
**
****************************************************************/

/****************************************************************
**	DEFINES
****************************************************************/

#define EVER (;;)

/****************************************************************
**	INCLUDES
****************************************************************/

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
//
#include "global.h"
//hard delay
#include <util/delay.h>
//Driver for 16X2LCD controller KS0066U with CDM1602K display
#include "at_lcd.h"

/****************************************************************
** FUNCTION PROTOTYPES
****************************************************************/

/****************************************************************
** GLOBAL VARIABLES
****************************************************************/

//Flags used by the ISRs
volatile Isr_flags g_isr_flags = { 0 };
//Error code of the application
volatile Error_code ge_error_code = Error_code::OK;

/****************************************************************************
**  Function
**  main |
****************************************************************************/
//! @return bool |
//! @brief dummy method to copy the code
//! @details test the declaration of a lambda method
/***************************************************************************/

int main(void)
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------
	
	uint16_t cnt = 0;
	
	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//! Initialize AT4809 internal peripherals
	init();
	
	//Power the LCD display
	SET_BIT( LCD_PWR_PORT.OUT, LCD_PWR_PIN );
	
	_delay_ms( 500.0 );
	
	//Power the LCD display
	CLEAR_BIT( LCD_PWR_PORT.OUT, LCD_PWR_PIN );
	
	_delay_ms( 500.0 );
	
	//Initialize
	lcd_init();
	
	//Welcome message
	lcd_print_str( LCD_POS( 0, 0 ), "OrangeHat" );
	
	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Main loop
	for EVER
	{
		//----------------------------------------------------------------
		//	BUTTON
		//----------------------------------------------------------------
		//	Use LED0 to mirror the state of the BTN

		//If: Fast Tick
		if (g_isr_flags.fast_tick == true)
		{
			//Reset slow tick
			g_isr_flags.fast_tick = false;
			//Take the value of the button and mirror it on LED1
			SET_BIT_VALUE( LED1_PORT.OUT, LED1_PIN, GET_BIT(BTN_PORT.IN, BTN_PIN) );
					
			//Driver that sync the user structure with the LCD.
			//This paradigm solve lots of timing problems of the direct call version.
			//You can print a million time a second, and the driver still won't bug out
			lcd_update();
			
		}
		
		//----------------------------------------------------------------
		//	SLOW SYSTEM TICK
		//----------------------------------------------------------------
		//	Slow tick generated by the RTC timer, prescaled by 
		//	32KHz/32/(Prescaler::TOP_SLOW_TICK+1) = 1000ms
		//	Toggle LED1

		//If: Slow Tick
		if (g_isr_flags.slow_tick == true)
		{
			//Reset slow tick
			g_isr_flags.slow_tick = false;
			//Toggle LED0	
			LED0_PORT.OUTTGL = MASK(LED0_PIN);
			
			//Power the LCD display ON and OFF
			//LCD_PWR_PORT.OUTTGL = MASK( LCD_PWR_PIN );
			
			//Counter
			lcd_print_u16( LCD_POS(1,0), cnt );
			cnt++;
		}
		
	}	//End: Main loop

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return 0;
}	//end: main

/***************************************************************************/
//!	@brief function
//!	function_template
/***************************************************************************/
//! @param x |
//! @return void |
//! @details
/***************************************************************************/

void function_template( void )
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------
	
	return;
}	//End function:
