/****************************************************************************
**	ORANGEBOT PROJECT
*****************************************************************************
**	AT MEGA PORT
**	Port configuration macros specific to AtMega uC For AVR studio 7
*****************************************************************************
**	Author: 			Orso Eric
**	Creation Date:		
**	Last Edit Date:
**	Revision:			1.1
**	Version:			2019-11-08
****************************************************************************/

/**********************************************************************************
**	PORT REGISTERS
***********************************************************************************
**	PORTx.DIR		|	enable the output drive for each individual pin | 0=input | 1=output
**	PORTx.OUT		|	Output value
**	PORTx.IN		|	Input value
**	PORTx.PORTCTRL	|	SRL='1' enables the slew rate limiter for all output in the port
**	PORTx.PINyCTRL	|	INVEN='1' invert input and all output
**					|	PULLUPEN='1' enable pull up resistor
**					|	ISC=0 INTDISABLE Interrupt disabled but input buffer enabled
**					|	ISC=1 BOTHEDGES Interrupt enabled with sense on both edges
**					|	ISC=2 RISING Interrupt enabled with sense on rising edge
**					|	ISC=3 FALLING Interrupt enabled with sense on falling edge
**					|	ISC=4 INPUT_DISABLE Interrupt and digital input buffer disabled
**					|	ISC=5 LEVEL Interrupt enabled with sense on low level
**	PORTx.OUTSET	|	'1' will set to one the individual out of the pin
**	PORTx.OUTCLR	|	'1' will set to zero the individual out of the pin
**	PORTx.OUTTGL	|	'1' will toggle individual out of the pin
**	PORTx.DIRSET	|	'1' will set to the individual pin to output
**	PORTx.DIRCLR	|	'1' will set to the individual pin to input
**	PORTx.DIRTGL	|	'1' will toggle the input output state of the individual pin
**********************************************************************************/

/**********************************************************************************
**	LOGS
***********************************************************************************
**		2019-11-08
**	Solved bug with the edge settings of the PORT macro. Now it works.
**********************************************************************************/

#ifndef AT4809_PORT_H
	//header environment variable, is used to detect multiple inclusion
	//of the same header, and can be used in the c file to detect the
	//included library
	#define AT4809_PORT_H

	/****************************************************************************
	**	ENVROIMENT VARIABILE
	****************************************************************************/

	/****************************************************************************
	**	GLOBAL INCLUDE
	**	TIPS: you can put here the library common to all source file
	****************************************************************************/

	/****************************************************************************
	**	ENUM
	****************************************************************************/
	
	//! Possible pin configurations
	#define	PIN_Z	(uint8_t)0x00
	#define	PIN_R	(uint8_t)0x01
	#define	PIN_H	(uint8_t)0x02
	#define	PIN_L	(uint8_t)0x03
	#define	PIN_SR	(uint8_t)0x04
	//Invert bit
	#define	PIN_INV	(uint8_t)0x08
	//Interrupt both edges
	#define	PIN_IE	(uint8_t)0x10
	//Interrupt rising edge
	#define	PIN_IR	(uint8_t)0x20
	//Interrupt falling edge
	#define	PIN_IF	(uint8_t)0x30
	//Interrupt low level
	#define	PIN_IL	(uint8_t)0x50

	/****************************************************************************
	**	MACRO
	*****************************************************************************
	**	Those macros setup pin in a port in one of four states
	**	'Z' |	Hi Impedance Input
	**	'R'	|	Input with pull-up resistance
	**	'L'	|	Output Low
	**	'H'	|	Output Hi
	****************************************************************************/
	
	//return true if DIR is out
	#define COND_OUT( pin )	\
		( ((pin) == PIN_L) || ((pin) == PIN_H)  )

	//return true if OUT is high
	#define COND_HIGH( pin )	\
		((pin) == PIN_H)
	
	//return true if pull up should be active
	#define COND_PULL( pin )	\
		((pin) == PIN_R)
	
	//return true if slew rate limiter is active
	#define COND_SLEW_RATE( pin )	\
		((pin) == PIN_SR)

	//return true if pin inverter should be active
	#define COND_INV( pin )	\
		((pin) == PIN_INV)

	#define COND_INT( pin )	\
		( ((pin) == PIN_IE) | ((pin) == PIN_IR) | ((pin) == PIN_IF) | ((pin) == PIN_IL) )

	

	//Configure direction register of the port
	#define PORT_CONFIG_DIR( PORTx, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 )	\
		PORTx.DIR =		SHL( COND_OUT( pin0 ), 0) |	\
						SHL( COND_OUT( pin1 ), 1) |	\
						SHL( COND_OUT( pin2 ), 2) |	\
						SHL( COND_OUT( pin3 ), 3) |	\
						SHL( COND_OUT( pin4 ), 4) |	\
						SHL( COND_OUT( pin5 ), 5) |	\
						SHL( COND_OUT( pin6 ), 6) |	\
						SHL( COND_OUT( pin7 ), 7)

	//Configure output register of the port
	#define PORT_CONFIG_OUT( PORTx, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 )	\
		PORTx.OUT =			SHL( COND_HIGH( pin0 ), 0) |	\
							SHL( COND_HIGH( pin1 ), 1) |	\
							SHL( COND_HIGH( pin2 ), 2) |	\
							SHL( COND_HIGH( pin3 ), 3) |	\
							SHL( COND_HIGH( pin4 ), 4) |	\
							SHL( COND_HIGH( pin5 ), 5) |	\
							SHL( COND_HIGH( pin6 ), 6) |	\
							SHL( COND_HIGH( pin7 ), 7)
	
	//Configure the slew rate limiter of the output
	#define PORT_CONFIG_SR( PORTx, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 )	\
		PORTx.PORTCTRL =	SHL( COND_SLEW_RATE( pin0 ), 0) |	\
							SHL( COND_SLEW_RATE( pin1 ), 1) |	\
							SHL( COND_SLEW_RATE( pin2 ), 2) |	\
							SHL( COND_SLEW_RATE( pin3 ), 3) |	\
							SHL( COND_SLEW_RATE( pin4 ), 4) |	\
							SHL( COND_SLEW_RATE( pin5 ), 5) |	\
							SHL( COND_SLEW_RATE( pin6 ), 6) |	\
							SHL( COND_SLEW_RATE( pin7 ), 7)
	
	#define PIN_CONFIG_HW( PINyCTRL, pin_config )	\
		PINyCTRL = SHL(COND_INV( pin_config ), PORT_INVEN_bp) | SHL(COND_PULL( pin_config ), PORT_PULLUPEN_bp) | ((COND_INT( pin_config ))?(PORT_ISC_gm & (pin_config>>4)):(0x00))

	//Configure individual pin hardware
	#define PORT_CONFIG_PINHW( PORTx, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 )	\
		PIN_CONFIG_HW( PORTx.PIN0CTRL, pin0 );	\
		PIN_CONFIG_HW( PORTx.PIN1CTRL, pin1 );	\
		PIN_CONFIG_HW( PORTx.PIN2CTRL, pin2 );	\
		PIN_CONFIG_HW( PORTx.PIN3CTRL, pin3 );	\
		PIN_CONFIG_HW( PORTx.PIN4CTRL, pin4 );	\
		PIN_CONFIG_HW( PORTx.PIN5CTRL, pin5 );	\
		PIN_CONFIG_HW( PORTx.PIN6CTRL, pin6 );	\
		PIN_CONFIG_HW( PORTx.PIN7CTRL, pin7 )
		
	//Configure an hardware port
	#define PORT_CONFIG( PORTx, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 ) \
		PORT_CONFIG_DIR( PORTx, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 ); \
		PORT_CONFIG_OUT( PORTx, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 ); \
		PORT_CONFIG_SR( PORTx, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 ); \
		PORT_CONFIG_PINHW( PORTx, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 )


	//those macros configure the single port
	#ifdef PORTA
		#define PORT_A_CONFIG( pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 ) \
			PORT_CONFIG( PORTA, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7)
	#endif

	//those macros configure the single port
	#ifdef PORTB
		#define PORT_B_CONFIG( pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 ) \
			PORT_CONFIG( PORTB, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7)
	#endif
	
	//those macros configure the single port
	#ifdef PORTC
		#define PORT_C_CONFIG( pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 ) \
			PORT_CONFIG( PORTC, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7)
	#endif
	
	//those macros configure the single port
	#ifdef PORTD
		#define PORT_D_CONFIG( pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 ) \
			PORT_CONFIG( PORTD, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7)
	#endif
	
	//those macros configure the single port
	#ifdef PORTE
		#define PORT_E_CONFIG( pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 ) \
			PORT_CONFIG( PORTE, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7)
	#endif
	
	//those macros configure the single port
	#ifdef PORTF
		#define PORT_F_CONFIG( pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7 ) \
			PORT_CONFIG( PORTF, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7)
	#endif
	

		///--------------------------------------------------------------------------
		///	UART
		///--------------------------------------------------------------------------
		//	UART specific macros

	//return true if the uart 0 is ready to transmit data
	#define UART0_TX_READY()	\
		(UCSR0A & MASK(UDRE0))

#else
	#warning "multiple inclusion of the header file at_mega_port.h"
#endif
