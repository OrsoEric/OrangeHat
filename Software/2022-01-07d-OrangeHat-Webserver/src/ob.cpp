/****************************************************************
**	OrangeBot  Project
*****************************************************************************
**        /
**       /
**      /
** ______ \
**         \
**          \
*****************************************************************************
**
*****************************************************************************
**	Author: 			Orso Eric
**	Creation Date:
**	Last Edit Date:
**	Revision:			1
**	Version:			0.1 ALFA
****************************************************************************/

/****************************************************************************
**	HYSTORY VERSION
*****************************************************************************
**		2020-01-16
**	Upgrade to Uniparser V5
**	Added RX messages
**		2020-01-17
**	Added handlers
**	Tested ping
**	Tested signature
**	Tested encoder absolute message
**	Tested encoder relative message
**	Tested encoder speed message
**	Tested bad message
**		2020-01-26
**	Tested SET_PERF message. AT4809 Performance counters
**	Fixed extra terminator problem of ParserWrap
**		2020-02-01
**	Added SET_PID_ERR_DUAL%S:%S:%S:%S from AT4809
**	Added ERR%u error message from AT4809
****************************************************************************/

/****************************************************************************
**	DESCRIPTION
*****************************************************************************
**		HOW TO ADD A MESSAGE:
**	1) "panopticon.h" add new status variables to the class header
**	2) "panopticon.cpp" add reference access methods to status variables
**	2b) "init" initialize new parameters to default values
**	3) "ob.cpp" add handler function to function prototypes and implementation
**	4) "ob.cpp" register message format and handler in "orangebot_node_cpp_init"
**	5) "ob.cpp" write the handler function to set the correct status vars inside panopticon
**	6) "ob.cpp" set BOILERPLATE_NAPI_STRING to 0 in order to enable the test bench
**	7) "example.cpp" add test cases, "show_robot_status" and check that the message is parsed correctly inside the status vars
**	8) "ob.cpp" set BOILERPLATE_NAPI_STRING to 1 in order to make the class work for the RPI
**	9) "orangebot_node_bindings.cpp" add the new fields to get_status_wrap
**	10)	Transfer all the cpp and h files but example.cpp inside the raspberry pi scr directory
**	11) Compile the project there using "npm run build"
**	The new status vars should now be parsed correctly and avaiable to "orangebot.js" NODE.JS RaspberryPi 3B+
**	From here you can add fields in index.html and bindings inside orangebot.js to process it correctly
*****************************************************************************
**		MESSAGES FROM MAIN MOTOR BOARD
**		F%s\0***STRING***\0
**	Get motor board signature. Argument is the number of bytes of the signature
**	Handler is meant to raise a flag and intercept the required number of bytes without flipping them to the parser
**		SET_ENC%u:%d\0
**	Main motor board answers with the encoder absolute reading for motor of index first argument
**		SET_ENC_REL%s:%%s:%s:%s\0
**	Main Motor board answers with the relative encoder readings for all encoder channels
**		SET_ENC_SPD%s:%%s:%s:%s\0
**	Main Motor board answers with the encoder speed readings for all encoder channels
*****************************************************************************
**		MESSAGES TO MAIN MOTOR BOARD
**  	OFF\0
**	Disable Motors
**		ON\0
**	Enable Motors
**		PWM_DUAL%s:%s\0
**	Switch to PWM controls for the main wheels and set main wheel target PWM
**		SPD_DUAL%s:%s\0
**	Switch to speed PID and set speed target
**		POS_DUAL_REL%s:%s\0
**	Switch to position PID and set PID target. Meant for ultra precise positioning
**		GET_ENC\0
**	Ask for absolute encoder readings
**	Answer with four SET_ENC messages
**		GET_REL_ENC\0
**	Ask for relative encoder readings
**	Answer with one %s%s%s%s message
**		GET_ENC_SPD\0
**	Ask for all encoder speed readings
**		SET_PWM_SLOPE%s\0
**	Set the PWM slope of the PWM slew rate limiter controller
**		GET_POS_PID%s\n
**
**		SET_POS_PID%s:%s:%s\0
Set the gain of the position PID

	GET_POS_PID\0
Get the gain of the position PID
Answer with a SET_POS_PID%s:%s:%s message

	GET_SPD_PID%s:%s:%s\0
Set the gain of the speed PID

****************************************************************************/

/****************************************************************************
**	KNOWN BUG
*****************************************************************************
**
****************************************************************************/

/****************************************************************************
**	INCLUDE
****************************************************************************/

//Standard C Libraries
#include <cstdio>
//#include <cstdlib>
#include <stdint.h>

//Standard C++ libraries
#include <iostream>
//#include <array>
//#include <vector>
//#include <queue>
//#include <string>
//#include <fstream>
//#include <chrono>
//#include <thread>

//OS Libraries
//#define _WIN32_WINNT 0x0500	//Enable GetConsoleWindow
//#include <windows.h>

//User Libraries
//Include user log trace
//#define ENABLE_DEBUG
#include "debug.h"
//Universal Parser V5
#include "uniparser.h"
//Class header
#include "ob.h"
//Stores robot status variables
#include "panopticon.h"

/****************************************************************************
**	NAMESPACES
****************************************************************************/

using std::cout;

namespace Orangebot
{

/****************************************************************************
**	GLOBAL VARIABILE
****************************************************************************/

//R parser used to decode messages from the motor board
Uniparser g_orangebot_motor_board_rx_parser;
//Memorize OrangeBot platform status vars
Panopticon g_orangebot_platform;

//signature characters left to parse
int g_signature_remaining_length;
//Size of the signature. = means invalid signature
int g_signature_length;
//Temp signature storage
char g_signature[MAX_SIGNATURE_LENGTH];

/****************************************************************************
**	FUNCTION PROTOTYPES
****************************************************************************/

//Obtain the reference to the robot status vars structure
extern Orangebot::Panopticon &get_robot_status( void );
//Hep convert from a small type to a regular int
template <typename T>
extern int to_int( T value );

//Expect the next (length) character received to be part of a signature
extern bool set_signature_length( int length );
//Process current data as part of a signature
extern bool parse_signature( uint8_t data );

	//----------------------------------------------------------------
	//	HANDLERS
	//----------------------------------------------------------------
	//	Handlers automatically called when a message with arguments is received
	//	Uniparser V5 is limited to regular function calls
	//	Uniparser V5 needs a wrapper to call a class instance method

	//! Status Group
//Ping Handler
extern void ping_handler( void );
//Signature Handler
extern void get_signature_handler( uint8_t str_length );
//Timestamp Handler
extern void get_timestamp_handler( int32_t timestamp );
//AT4809 Error code
extern void get_at4809_err_code( uint8_t err_code );
//AT4809 Performance Counters
extern void set_perf_handler( int32_t at4809_cpu, uint8_t at4809_rxi, uint8_t at4809_txo );

	//!Encoder Group
//Single encoder absolute count update
extern void get_one_enc_abs_handler( uint8_t index, int32_t enc );
//Quad encoder relative count update handler
extern void get_four_enc_rel_handler( int16_t enc_rel_a, int16_t enc_rel_b, int16_t enc_rel_c, int16_t enc_rel_d );
//Quad encoder speed update handler
extern void get_four_enc_spd_handler( int16_t enc_spd_a, int16_t enc_spd_b, int16_t enc_spd_c, int16_t enc_spd_d );
//Two encoder speed update handler
extern void get_two_enc_spd_handler( int16_t enc_spd_a, int16_t enc_spd_b );

	//! Control System Target Group
//Handle the PWM message from the motor board
extern void get_two_vnh7040_pwm_handler( int16_t pwm_a, int16_t pwm_b );

	//! Control System Group
//PWM slew rate limiter handler
extern void get_pwm_ctrl_handler( int16_t slew_rate );
//Speed PID control parameters handler
extern void get_spd_ctrl_handler( int16_t gain_proportional, int16_t gain_derivative, int16_t gain_integrative );
//Position PID control parameters handler
extern void get_pos_ctrl_handler( int16_t gain_proportional, int16_t gain_derivative, int16_t gain_integrative );
//Error and slew rate of TWO PID controllers
extern void get_pid_status_dual( int16_t err0, int16_t slew_rate0, int16_t err1, int16_t slew_rate1 );

/****************************************************************************
**	FUNCTION
****************************************************************************/

/***************************************************************************/
//!	@brief reference
//!	get_robot_status | void
/***************************************************************************/
//! @param x |
//! @return void |
//! @details
/***************************************************************************/

Panopticon &get_robot_status( void )
{
	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return g_orangebot_platform;
}	//end function: get_robot_status | void

/***************************************************************************/
//!	@brief
//!	to_int<int16_t> | int16_t
/***************************************************************************/
//! @param x |
//! @return void |
//! @details
//! Help convert from a small type to a regular int
/***************************************************************************/

template <>
int to_int( int16_t value )
{
	//Trace Enter with arguments
	DENTER();

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	int value_tmp = (int16_t)value;

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Fix illegal values
	if (value_tmp > 32767)
	{
		value_tmp -= 65536;
	}

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN();
	return value_tmp;
}	//end function: to_int<int16_t> | int16_t

/***************************************************************************/
//!	@brief Function
//!	orangebotNodeCppInit | void
/***************************************************************************/
//! @param x |
//! @return void |
//! @details
//! Initialize NODE.JS C++ library
/***************************************************************************/

void orangebot_node_cpp_init( void )
{
	//Trace Enter with arguments
	DENTER();

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	bool f_ret = false;

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//Construct platform structure
	g_orangebot_platform = Orangebot::Panopticon();
	//Initialize parser class
	g_orangebot_motor_board_rx_parser = Orangebot::Uniparser();

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

		//! Status Group
	//Register | Get Ping
	f_ret = g_orangebot_motor_board_rx_parser.add_cmd( "P", (void *)&ping_handler);
	//Register | Get Signature command
	f_ret = g_orangebot_motor_board_rx_parser.add_cmd( "F%u", (void *)&get_signature_handler);
	//Register | Get Timestamp
	f_ret = g_orangebot_motor_board_rx_parser.add_cmd( "TIME%d", (void *)&get_timestamp_handler);
	//Register | Set AT4809 Error code
	f_ret = g_orangebot_motor_board_rx_parser.add_cmd( "ERR%u", (void *)&get_at4809_err_code);
	//Register | Set AT4809 Performance
	f_ret = g_orangebot_motor_board_rx_parser.add_cmd( "SET_PERF%d:%u:%u", (void *)&set_perf_handler);

		//!Encoder Group
	//Get single absolute encoder reading
	f_ret |= g_orangebot_motor_board_rx_parser.add_cmd( "ENC_ABS%u:%d", (void *)&get_one_enc_abs_handler );
	//Get quad relative encoder reading
	f_ret |= g_orangebot_motor_board_rx_parser.add_cmd( "ENC_REL%S:%S:%S:%S", (void *)&get_four_enc_rel_handler );
	//Get quad encoder speed reading
	f_ret |= g_orangebot_motor_board_rx_parser.add_cmd( "ENC_SPD_DUAL%S:%S", (void *)&get_two_enc_spd_handler );
	//Get quad encoder speed reading
	f_ret |= g_orangebot_motor_board_rx_parser.add_cmd( "ENC_SPD%S:%S:%S:%S", (void *)&get_four_enc_spd_handler );

		//! Control System Target Group
	//Register | Dual PWM Command
	f_ret |= g_orangebot_motor_board_rx_parser.add_cmd( "PWM_DUAL%S:%S", (void *)&get_two_vnh7040_pwm_handler );

		//! Control System Group
	//Register | Get PWM Slew Rate Limiter Parameters
	f_ret |= g_orangebot_motor_board_rx_parser.add_cmd( "PWM_PARAM%S", (void *)&get_pwm_ctrl_handler );
	//Register | Get Speed PID Parameters
	f_ret |= g_orangebot_motor_board_rx_parser.add_cmd( "SPD_PARAM%S:%S:%S", (void *)&get_spd_ctrl_handler );
	//Register | Get Position PID Parameters
	f_ret |= g_orangebot_motor_board_rx_parser.add_cmd( "POS_PARAM%S:%S:%S", (void *)&get_pos_ctrl_handler );
	//Register | Get Pid status performance
	f_ret |= g_orangebot_motor_board_rx_parser.add_cmd( "SET_PID_ERR_DUAL%S:%S:%S:%S", (void *)&get_pid_status_dual );

	//If: fail
	if (f_ret == true)
	{
        DPRINT("ERR: Failed to register command\n");
	}
	//Initialize parser
	g_orangebot_motor_board_rx_parser.parse('\0');

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN_ARG("success: %d\n", f_ret);
	return;
}	//end function: orangebotNodeCppInit | void

/***************************************************************************/
//!	@brief
//!	orangebotParse | std::string
/***************************************************************************/
//! @param x |
//! @return void |
//! @details
/***************************************************************************/

void orangebot_parse( std::string str )
{
	//Trace Enter with arguments
	DENTER_ARG( "%s\n", str.c_str() );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//Counter
	unsigned int t;

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	cout << "CPP: Parsing: ";

	//For: all char in the string but the last (an extra \0)
	for (t = 0;t <= str.length()-BOILERPLATE_NAPI_STRING;t++)
	{
			//NODE:JS
		//If character is printable
		if ((str[t]>=' ') && (str[t]<='~'))
		{
			cout << str[t];
		}
		else
		{
			int num = str[t];
			cout << "(" << num << ")";
		}
		//If: there are no signature characters to be processed
		if (g_signature_remaining_length <= 0)
		{
			DPRINT(">%x<\n", str[t]);
			//Extract char and feed it to the parser
			g_orangebot_motor_board_rx_parser.parse( str[t] );
		}
		//If: there is at least a signature character to process
		else
		{
			//Process current data as part of a signature
			parse_signature( str[t] );
		}
	} //End for: all cahr in the string

	cout << "\n";

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN();
	return;
}	//end function: orangebotParse | std::string

/***************************************************************************/
//!	@brief function
//!	set_signature_length | int
/***************************************************************************/
//! @param length | int
//! @return void |
//! @details
//! Expect the next (length) character received to be part of a signature
/***************************************************************************/

bool set_signature_length( int length )
{
	//Trace Enter
	DENTER();

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	if (length >= MAX_SIGNATURE_LENGTH)
	{
		DRETURN_ARG("ERR: bad signature length: %d\n", length);
		return true; //OK
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Expect the next n characters to be part of a signature
	g_signature_remaining_length = length;
	//Reset previous signature
	g_signature_length = 0;

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN_ARG("signature length: %d\n", length);
	return false; //OK
}	//end function:	set_signature_length | int

/***************************************************************************/
//!	@brief function
//!	parse_signature | uint8_t
/***************************************************************************/
//! @param data | uint8_t | signature character
//! @return void |
//! @details
//! Process current data as part of a signature
/***************************************************************************/

bool parse_signature( uint8_t data )
{
	//Trace Enter
	DENTER_ARG("data: %x\n", data);

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//If: this data should not have been processed as signature
	if (g_signature_remaining_length <= 0)
	{
		//Invalidate signature
		g_signature_remaining_length = 0;
		g_signature_length = 0;
		DRETURN_ARG("ERR: no signature data left to be processed\n");
		return true; //FAIL
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Store current signature character
	g_signature[ g_signature_length ] = data;

	//Update current signature length
	g_signature_length++;
	//If: Exceed maximum size
	if (g_signature_length > MAX_SIGNATURE_LENGTH)
	{
		DRETURN_ARG("ERR: maximum signature size exceeded: %d\n", g_signature_length);
		cout << "CPP: maximum signature size exceeded:" << g_signature_length << "\n";
		//Invalidate signature
		g_signature_remaining_length = 0;
		g_signature_length = 0;
		return true; //FAIL
	}

	//Data has been processed
	g_signature_remaining_length--;
	//If: last character is not a terminator
	if ((g_signature_remaining_length == 0) && (data != '\0'))
	{
		DRETURN_ARG("ERR: signature is not null terminated %x\n", data);
		//Invalidate signature
		g_signature_remaining_length = 0;
		g_signature_length = 0;
		cout << "CPP: Failed to decode signature. No NULL terminator\n";
		return true; //FAIL
	}
	//If: string is complete and valid
	else if ((g_signature_remaining_length == 0) && (data == '\0'))
	{
		//Save the string inside the panopticon class
		g_orangebot_platform.signature() = std::string( g_signature );
		cout << "CPP: Signature decoded: " << g_orangebot_platform.signature();
		#ifdef ENABLE_DEBUG
		uint8_t *str_tmp = (uint8_t *)g_orangebot_platform.signature().c_str();
		DPRINT("Signature decoded: %s\n", str_tmp );
		#endif // ENABLE_DEBUG
	}

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN();
	return false; //OK
}	//end function:	set_signature_length | int

/****************************************************************************
**	PARSER HANDLERS
****************************************************************************/

/***************************************************************************/
//!	@brief
//!	ping_handler | void
/***************************************************************************/
//! @param x |
//! @return void |
//! @details
//! Reset communication timeout
/***************************************************************************/

void ping_handler( void )
{
	//Trace Enter
	DENTER();

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

	DRETURN();
	return;
}	//end function:	ping_handler | void

/***************************************************************************/
//!	@brief
//!	get_signature_handler | uint8_t
/***************************************************************************/
//! @param x |
//! @return void |
//! @details
//! Signature Handler
/***************************************************************************/

void get_signature_handler( uint8_t str_length )
{
	//Trace Enter
	DENTER_ARG("Signature length: %d\n", str_length);

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Setup the signature processor
	set_signature_length( str_length );

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN();
	return;
}	//end function:	get_signature_handler | uint8_t

/***************************************************************************/
//!	@brief
//!	get_timestamp_handler | int32_t
/***************************************************************************/
//! @param timestamp | int32_t | platform side timestamp
//! @return void |
//! @details
//! Timestamp Handler
/***************************************************************************/

void get_timestamp_handler( int32_t timestamp )
{
	//Trace Enter
	DENTER();

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------
	//! @todo add timestamp on NODE.JS

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN();
	return;
}	//end function:	get_timestamp_handler | int32_t

/***************************************************************************/
//!	@brief
//!	get_at4809_err_code | uint8_t
/***************************************************************************/
//! @param err_code | uint8_t | AT4809 Error code (OrangeBot::Error_code)
//! @return void |
//! @details
//! AT4809 Error code
/***************************************************************************/

void get_at4809_err_code( uint8_t err_code )
{
	//Trace Enter
	DENTER();

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Store error code from AT4809
	g_orangebot_platform.at4809_err_code() = err_code;

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN();
	return;
}	//end function:	get_at4809_err_code | uint8_t

/****************************************************************************
**	Function
**	Dummy | bool
****************************************************************************/
//! @param f bool
//! @return bool |
//! @brief dummy method to copy the code
//! @details verbose description
/***************************************************************************/

void set_perf_handler( int32_t at4809_cpu, uint8_t at4809_rxi, uint8_t at4809_txo )
{
	//Trace Enter with arguments
	DENTER_ARG("cpu: %d | rxi: %d | txo:%d\n", at4809_cpu, at4809_rxi, at4809_txo);

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------
	//! @details algorithm:

	g_orangebot_platform.cpu_scan_frequency() = at4809_cpu;
	g_orangebot_platform.uart_rxi_bandwidth() = at4809_rxi;
	g_orangebot_platform.uart_txo_bandwidth() = at4809_txo;

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Trace Return vith return value
	DRETURN_ARG("out: %d\n", 0);

	return;
}	//end function: Dummy | bool

/***************************************************************************/
//!	@brief
//!	get_one_enc_abs_handler | uint8_t, int32_t
/***************************************************************************/
//! @param x |
//! @return void |
//! @details
//! Single encoder absolute count update
/***************************************************************************/

void get_one_enc_abs_handler( uint8_t index, int32_t enc )
{
	//Trace Enter
	DENTER_ARG( "index: %d | enc: %d\n", index, enc );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//If: bad number of channels
	if (index >= NUM_ENC)
	{
		DRETURN_ARG("ERR: bad number of channel: %d\n", index);
        return; //FAIL
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Save the encoder reading
	g_orangebot_platform.enc_pos( index ) = enc;

    #ifdef ENABLE_DEBUG
		enc = g_orangebot_platform.enc_pos( index );
		DPRINT("enc_abs[%d]: %d\n", index, enc );
    #endif

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN();
	return;
}	//end function:	get_one_enc_abs_handler | uint8_t, int32_t

/***************************************************************************/
//!	@brief handler
//!	get_four_enc_rel_handler | int16_t, int16_t, int16_t, int16_t
/***************************************************************************/
//! @param enc_rel_* | int16_t | encoder position reading change since last message
//! @return void |
//! @details
//! Quad encoder relative count update handler
/***************************************************************************/

void get_four_enc_rel_handler( int16_t enc_rel_a, int16_t enc_rel_b, int16_t enc_rel_c, int16_t enc_rel_d )
{
	//Trace Enter
	DENTER();

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Save the encoder reading
	g_orangebot_platform.enc_pos( 0 ) += to_int<int16_t>(enc_rel_a);
	g_orangebot_platform.enc_pos( 1 ) += to_int<int16_t>(enc_rel_b);
	g_orangebot_platform.enc_pos( 2 ) += to_int<int16_t>(enc_rel_c);
	g_orangebot_platform.enc_pos( 3 ) += to_int<int16_t>(enc_rel_d);

	#ifdef ENABLE_DEBUG
	int t;
	int enc_tmp;
	for (t = 0;t < NUM_ENC;t++)
	{
		enc_tmp = g_orangebot_platform.enc_pos( t );
		DPRINT( "enc[%d]: %d\n", t, enc_tmp );
	}

	#endif // ENABLE_DEBUG

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN();
	return;
}	//End handler: get_four_enc_rel_handler | int16_t, int16_t, int16_t, int16_t

/***************************************************************************/
//!	@brief handler
//!	get_four_enc_spd_handler | int16_t, int16_t, int16_t, int16_t
/***************************************************************************/
//! @param enc_spd_* | int16_t | encoder speed reading
//! @return void |
//! @details
//! Quad encoder speed update handler
/***************************************************************************/

void get_four_enc_spd_handler( int16_t enc_spd_a, int16_t enc_spd_b, int16_t enc_spd_c, int16_t enc_spd_d )
{
	//Trace Enter
	DENTER();

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Save the encoder reading
	g_orangebot_platform.enc_spd( 0 ) = to_int<int16_t>(enc_spd_a);
	g_orangebot_platform.enc_spd( 1 ) = to_int<int16_t>(enc_spd_b);
	g_orangebot_platform.enc_spd( 2 ) = to_int<int16_t>(enc_spd_c);
	g_orangebot_platform.enc_spd( 3 ) = to_int<int16_t>(enc_spd_d);

	#ifdef ENABLE_DEBUG
	int t;
	int enc_tmp;
	for (t = 0;t < NUM_ENC;t++)
	{
		enc_tmp = g_orangebot_platform.enc_spd( t );
		DPRINT( "enc[%d]: %d\n", t, enc_tmp );
	}

	#endif // ENABLE_DEBUG

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN();
	return;
}	//End handler: get_four_enc_rel_handler | int16_t, int16_t, int16_t, int16_t

/***************************************************************************/
//!	@brief handler
//!	get_two_enc_spd_handler | int16_t, int16_t
/***************************************************************************/
//! @param enc_spd_* | int16_t | encoder speed reading
//! @return void |
//! @details
//! Quad encoder speed update handler
/***************************************************************************/

void get_two_enc_spd_handler( int16_t enc_spd_a, int16_t enc_spd_b )
{
	//Trace Enter
	DENTER();

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Save the encoder reading
	int tmp = enc_spd_a;
	tmp = to_int<int16_t>( tmp );
	g_orangebot_platform.enc_spd( 0 ) = to_int<int16_t>( tmp );
	tmp = enc_spd_b;
	tmp = to_int<int16_t>( tmp );
	g_orangebot_platform.enc_spd( 1 ) = to_int<int16_t>( tmp );

	#ifdef ENABLE_DEBUG
	int t;
	int enc_tmp;
	for (t = 0;t < NUM_ENC;t++)
	{
		enc_tmp = g_orangebot_platform.enc_spd( t );
		DPRINT( "enc[%d]: %d\n", t, enc_tmp );
	}

	#endif // ENABLE_DEBUG

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN();
	return;
}	//End handler: get_two_enc_spd_handler | int16_t, int16_t

/***************************************************************************/
//!	@brief
//!	get_vnh7040_pwm_handler | int16_t | int16_t
/***************************************************************************/
//! @param x |
//! @return void |
//! @details
/***************************************************************************/

void get_two_vnh7040_pwm_handler( int16_t pwm_a, int16_t pwm_b )
{
	//Trace Enter
	DENTER_ARG("pwm: %d | %d\n", pwm_a, pwm_b);

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	int tmp;

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	std::cout << "\nCPP: PWM: " << pwm_a << " | " << pwm_b << "\n";

	//Save PWM targets
	tmp = pwm_a;
	tmp = to_int<int16_t>( tmp );
	g_orangebot_platform.pwm( 0 ) = to_int<int16_t>(tmp);
	tmp = pwm_b;
	tmp = to_int<int16_t>( tmp );
	g_orangebot_platform.pwm( 1 ) = to_int<int16_t>(tmp);

	/*
	g_orangebot_platform.pwm( 0 ) = pwm_a;
	g_orangebot_platform.pwm( 1 ) = pwm_b;
	*/

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN();
	return;
}	//end function:	get_vnh7040_pwm_handler | int16_t | int16_t

/***************************************************************************/
//!	@brief
//!	get_pwm_ctrl_handler | int16_t
/***************************************************************************/
//! @param slew_rate | int16_t | platform PWM slew rate settings
//! @return void |
//! @details
//! PWM slew rate limiter handler
/***************************************************************************/

void get_pwm_ctrl_handler( int16_t slew_rate )
{
	//Trace Enter
	DENTER_ARG("OrangeBot pwm slew rate: %d\n", slew_rate);

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

	DRETURN();
	return;
}	//end function:	get_pwm_ctrl_handler | int16_t

/***************************************************************************/
//!	@brief
//!	get_spd_ctrl_handler | int16_t, int16_t, int16_t
/***************************************************************************/
//! @param gain_proportional | int16_t | platform PID parameter
//! @param gain_derivative | int16_t | platform PID parameter
//! @param gain_integrative | int16_t | platform PID parameter
//! @return void |
//! @details
//! Speed PID control parameters handler
/***************************************************************************/

void get_spd_ctrl_handler( int16_t gain_proportional, int16_t gain_derivative, int16_t gain_integrative )
{
	//Trace Enter
	DENTER_ARG("Gain | %d | %d | %d\n", gain_proportional, gain_derivative, gain_integrative);

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

	DRETURN();
	return;
}	//end function:	get_spd_ctrl_handler | int16_t, int16_t, int16_t

/***************************************************************************/
//!	@brief
//!	get_pos_ctrl_handler | int16_t, int16_t, int16_t
/***************************************************************************/
//! @param gain_proportional | int16_t | platform PID parameter
//! @param gain_derivative | int16_t | platform PID parameter
//! @param gain_integrative | int16_t | platform PID parameter
//! @return void |
//! @details
//! Position PID control parameters handler
/***************************************************************************/

void get_pos_ctrl_handler( int16_t gain_proportional, int16_t gain_derivative, int16_t gain_integrative )
{
	//Trace Enter
	DENTER_ARG("Gain | %d | %d | %d\n", gain_proportional, gain_derivative, gain_integrative);

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

	DRETURN();
	return;
}	//end function:	get_spd_ctrl_handler | int16_t, int16_t, int16_t

/***************************************************************************/
//!	@brief
//!	get_pid_status_dual | int16_t, int16_t, int16_t, int16_t
/***************************************************************************/
//! @param err* | int16_t | Error of the PID of index *
//! @param slew_rate* | int16_t | Command slew rate of the PID of index *
//! @return void |
//! @details
//! Error and slew rate of TWO PID controllers
/***************************************************************************/

void get_pid_status_dual( int16_t err0, int16_t slew_rate0, int16_t err1, int16_t slew_rate1 )
{
	//Trace Enter
	DENTER_ARG("PID0 | Err: %d | Slew Rate: %d | PID1 | Err: %d | Slew Rate: %d\n", err0, slew_rate0, err1, slew_rate1);

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Store PID performance vars
	g_orangebot_platform.pid_err( 0 ) = err0;
	g_orangebot_platform.pid_slew_rate( 0 ) = slew_rate0;
	g_orangebot_platform.pid_err( 1 ) = err1;
	g_orangebot_platform.pid_slew_rate( 1 ) = slew_rate1;

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN();
	return;
}	//end function:	get_pid_status_dual | int16_t, int16_t, int16_t, int16_t

/***************************************************************************/
//!	@brief
//!	function_template
/***************************************************************************/
//! @param x |
//! @return void |
//! @details
/***************************************************************************/

void function_template( void )
{
	//Trace Enter with arguments
	DENTER();

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

	DRETURN();
	return;
}	//end function:

/****************************************************************************
**	CLASSES
****************************************************************************/

} //End namespace: Orangebot
