
/****************************************************************
**	INCLUDES
****************************************************************/

//type definition using the bit width and sign
#include <stdint.h>
//RNG
#include <stdlib.h>
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

//Driver for the UART communication. Provides init, ISR and buffers
#include "uart.h"
//Embedded string (TODO embedded_string.cpp is a better implementation, and I have an even better implementation in the longan nano screen driver)
#include "string_uc.h"
//Universal Parser V5
#include "uniparser.h"
//Driver for the 16b multi channel timeout ISR servo driver
#include "servo.h"

/****************************************************************
** DEFINITIONS
****************************************************************/

//Maximum size of a signature string
#define MAX_SIGNATURE_LENGTH	32

/****************************************************************
** FUNCTION PROTOTYPES
****************************************************************/

	///----------------------------------------------------------------------
	///	PARSER
	///----------------------------------------------------------------------
	//	Handlers are meant to be called automatically when a command is decoded
	//	User should not call them directly
		
//Handle Ping message coming from the RPI 3B+
extern void ping_handler( void );
//Handle Request for signature coming from the RPI 3B+. Answer with board signature.
extern void send_signature_handler( void );
//Handle PPM message. Servo command in microseconds.
extern void set_servo_ppm( uint8_t iu8_index, int16_t is16_us );

/****************************************************************
** GLOBAL VARS PROTOTYPES
****************************************************************/

//Class to handle UART0 communication
extern User::Uart gcl_uart0;
//Instance of the servo controller class
extern OrangeBot::Servo gc_servo;

/****************************************************************
** GLOBAL VARS
****************************************************************/

	///--------------------------------------------------------------------------
	///	PARSER
	///--------------------------------------------------------------------------

//Board Signature
uint8_t *g_board_sign = (uint8_t *)"OrangeHot-2022-01-07";
//communication timeout counter
uint8_t g_uart_timeout_cnt = 0;
//Communication timeout has been detected
bool g_f_timeout_detected = false;

/***************************************************************************/
//!	function
//!	init_parser_commands | Orangebot::Uniparser &
/***************************************************************************/
//! @param parser_tmp | Orangebot::Uniparser | Parser to which commands are to be atached
//! @return bool | false = OK | true = command was invalid and was not registered
//! @brief
//! @details
/***************************************************************************/

bool init_parser_commands( Orangebot::Uniparser &parser_tmp )
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//Return flag
	bool f_ret = false;

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//! Register commands and handler for the universal parser class. A masterpiece :')
	//Register ping command. It's used to reset the communication timeout
	f_ret = parser_tmp.add_cmd( "P", (void *)&ping_handler );
	//Register the Find command. Board answers with board signature
	f_ret |= parser_tmp.add_cmd( "F", (void *)&send_signature_handler );
	//Platform set PPM command
	f_ret |= parser_tmp.add_cmd( "PPM%u:%S", (void *)&set_servo_ppm );
		
	//If: Uniparser V4 failed to register a command
	if (f_ret == true)
	{
		//TODO: signal error
	}

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------
	
	return f_ret;
}	//End function: init_parser_commands | Orangebot::Uniparser &

/***************************************************************************/
//!	@brief ping command handler
//!	ping_handler | void
/***************************************************************************/
//! @return void
//!	@details
//! Handler for the ping command. Keep alive connection
/***************************************************************************/

void ping_handler( void )
{
	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------
	
	//Reset communication timeout handler
	g_uart_timeout_cnt = 0;
	
	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	LED1_PORT.OUTTGL = MASK(LED1_PIN);

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return; //OK
}	//end handler: ping_handler | void

/***************************************************************************/
//!	@brief board signature handler
//!	signature_handler | void
/***************************************************************************/
//! @return void
//!	@details
//! Handler for the get board signature command. Send board signature via UART
/***************************************************************************/

void send_signature_handler( void )
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//Counter
	uint8_t t;
	//Temp return
	uint8_t ret;
	
	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//Reset communication timeout handler
	g_uart_timeout_cnt = 0;
	//Length of the board signature string
	uint8_t board_sign_length = User::String_uc::str_length( g_board_sign, MAX_SIGNATURE_LENGTH );
	//If: string is invalid
	if (board_sign_length > MAX_SIGNATURE_LENGTH)
	{
		report_error( Error_code::ERR_PARSER );
		return; //FAIL
	}
	//Construct string length
	uint8_t str[User::String_uc::Size::STRING_U8];
	//Convert the string length to number
	ret = User::String_uc::to_string<uint8_t>( board_sign_length, str );
	
	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Send a signature message with string argument to the RPI 3B+
	gcl_uart0.send( 'F' );
	//For: number of number of signature characters
	for (t = 0;t < ret;t++)
	{
		//Push the number of characters	
		gcl_uart0.send( str[t] );
	}
	//Send message terminator
	gcl_uart0.send( (uint8_t)0x00 );
	//For: number of signature characters
	for (t = 0;t < board_sign_length;t++)
	{
		//Send the next signature byte
		gcl_uart0.send( g_board_sign[t] );
	}
	//Send string terminator
	gcl_uart0.send( (uint8_t)0x00 );

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return; //OK
}	//end handler: signature_handler | void

/***************************************************************************/
//!	function
//!	set_servo_ppm | uint8_t, int16_t
/***************************************************************************/
//! @param iu8_index | index of the servo being controlled
//! @param is16_position | PPM in microseconds
//! @param is16_speed | PPM change in microseconds per second
//! @return void |
//! @brief
//! @details
/***************************************************************************/

//Handle PPM message. Servo command in microseconds.
void set_servo_ppm( uint8_t iu8_index, int16_t is16_position, int16_t is16_speed )
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//Reset communication timeout handler
	g_uart_timeout_cnt = 0;

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------
	
	//Ask the servo driver to set the position and speed according to what the RPI asks
	bool u1_ret = gc_servo.set_servo( iu8_index, is16_position, is16_speed );
	if (u1_ret == true)
	{
		
		report_error(Error_code::ERR_PARSER);
	}

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------
	
	return;
}	//End function: set_platform_pwm_handler | int16_t, int16_t
