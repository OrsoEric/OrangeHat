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
**	Compiler Symbols: F_CPU=20000000
**		-D"F_CPU=20000000"			| <util/delay.h> hardwired delay
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
**	SERVO PINs. Toggle power ON and OFF to test power delivery
**		2021-12-31
**	soft start encapsulated inside servo.hpp class
**
****************************************************************/

/****************************************************************
**	USED PINS
****************************************************************
**
**	LEDs			|	Microcontroller
**	uc.led.green	|	uc.C4 (true = ON)
**	uc.led.blue		|	uc.C5 (true = ON)
**
**	BUTTON			|	Microcontroller
**	uc.btn.no0		|	uc.A6 (push = FALSE)
**
**	DISPLAY			|	Microcontroller
**	uc.lcd.pwr#		|	uc.A7
**	uc.lcd.D4		|	uc.B0
**	uc.lcd.D5		|	uc.B1
**	uc.lcd.D6		|	uc.B2
**	uc.lcd.D7		|	uc.B3
**	uc.lcd.EN		|	uc.B4
**	uc.lcd.RS		|	uc.B4
**
**	SERVO			|	Microcontroller
**	uc.servo.pwr#	|	uc.D7
**	uc.servo.ch1	|	uc.E0
**	uc.servo.ch2	|	uc.E1
**	uc.servo.ch3	|	uc.E2
**	uc.servo.ch4	|	uc.E3
**	uc.servo.ch5	|	uc.F0
**	uc.servo.ch6	|	uc.F1
**	uc.servo.ch7	|	uc.F2
**	uc.servo.ch8	|	uc.F3
**
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
//
#include "global.h"
//hard coded delay
#include <util/delay.h>


//Driver for 16X2LCD controller KS0066U with CDM1602K display
#include "at_lcd.h"

#include "servo.cpp"

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

	///----------------------------------------------------------------------
	///	SERVO DRIVER
	///----------------------------------------------------------------------

//Instance of the servo controller class
OrangeBot::Servo gc_servo; 

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
	
	//Construct and initialize the servo class
	gc_servo = OrangeBot::Servo();
	//Give power to servos and starts the driver
	gc_servo.power( true );
	
	//Activate interrupts
	sei();
	
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
			//Use the button to power the LCD
			//SET_BIT_VALUE( SERVO_PWR_PORT.OUT, SERVO_PWR_PIN, GET_BIT(BTN_PORT.IN, BTN_PIN) );
					
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
			
			//Power all SERVOs ON and OFF
			//SERVO_PWR_PORT.OUTTGL = MASK( SERVO_PWR_PIN );
			
			//Counter
			lcd_print_u16( LCD_POS(1,0), cnt );
			cnt++;
			
			if (cnt % 2 == 0)
			{
				gc_servo.set_servo( 0, -50 );
			}
			else
			{
				gc_servo.set_servo( 0, 50 );
			}
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
