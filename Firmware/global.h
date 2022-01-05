#ifndef GLOBAL_H
	//header environment variable, is used to detect multiple inclusion
	//of the same header, and can be used in the c file to detect the
	//included library
	#define GLOBAL_H

	/****************************************************************************
	**	ENVROIMENT VARIABILE
	****************************************************************************/

	/****************************************************************************
	**	GLOBAL INCLUDE
	**	TIPS: you can put here the library common to all source file
	****************************************************************************/

	/****************************************************************************
	**	DEFINE
	****************************************************************************/

		///----------------------------------------------------------------------
		///	PIN ALIAS
		///----------------------------------------------------------------------
	
	#define BTN_PORT	PORTA
	#define BTN_PIN		6
	#define	LED0_PORT	PORTC
	#define LED0_PIN	4
	#define	LED1_PORT	PORTC
	#define LED1_PIN	5
	
	#define SERVO_PWR_PORT	PORTD
	#define SERVO_PWR_PIN	7
	
		///----------------------------------------------------------------------
		///	TIMINGS
		///----------------------------------------------------------------------
		//	Based on slow system tick.
		//	The TOP is a number that is counted. The number of counts is TOP+1
		
	typedef enum _Prescaler
	{
		//How many PIT makes a slow tick
		TOP_SLOW_TICK			= 1000,
	} Prescaler;

	/****************************************************************************
	**	ENUM
	****************************************************************************/

	//Error codes that can be experienced by the program
	typedef enum _Error_code
	{
		//All good
		OK,
		//The main application could not keep up with the previous fast tick. Raise the alarm
		ERR_OVERRUN_FAST_TICK,
		//The main application could not keep up with the previous slow tick. Raise the alarm
		ERR_OVERRUN_SLOW_TICK,
	} Error_code;

	/****************************************************************************
	**	MACRO
	****************************************************************************/

		///----------------------------------------------------------------------
		///	ERROR
		///----------------------------------------------------------------------

	#define ISSUE_ERROR( error_var, error_code )	\
		(error_var != Error_code::OK)?(error_var = error_code):(Error_code::OK)

	/****************************************************************************
	**	TYPEDEF
	****************************************************************************/

	//Global flags raised by ISR functions
	typedef struct _Isr_flags Isr_flags;

	/****************************************************************************
	**	STRUCTURE
	****************************************************************************/

	//Global flags raised by ISR functions
	struct _Isr_flags
	{
		//First byte
		uint8_t fast_tick	: 1;	//Fast tick. Generated by RTC PIT
		uint8_t slow_tick	: 1;	//Slow Tick. Generated by RTC PIT
	};

	/****************************************************************************
	**	PROTOTYPE: INITIALISATION
	****************************************************************************/

	//port configuration and call the peripherals initialization
	extern void init( void );

	/****************************************************************************
	**	PROTOTYPE: FUNCTION
	****************************************************************************/
		
	/****************************************************************************
	**	PROTOTYPE: GLOBAL VARIABILE
	****************************************************************************/

		///----------------------------------------------------------------------
		///	STATUS FLAGS
		///----------------------------------------------------------------------

	//Volatile flags used by ISRs
	extern volatile	Isr_flags g_isr_flags;
	//Error code of the application
	extern volatile Error_code ge_error_code;
	
#else
	#warning "multiple inclusion of the header file global.h"
#endif

