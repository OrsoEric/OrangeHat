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
**	OrangeBot Node C++
*****************************************************************
**  High performance C++ methods meant to be used by NODE.JS
**	orangebot.js script running inside the RPI3 B+ inside OrangeBot
**	The bindings take care of more compute intensive work not suited for javascript
**
**		OrangeBot Motor Board -> RPI 3B+ message decoding
**	Decode messages coming from the motor board
****************************************************************/

/****************************************************************************
**	INCLUDES
****************************************************************************/

#include <iostream>
//NODE bindings
#include <napi.h>
//C++ implementation of high performance methods
#include "ob.h"
////Stores robot status variables
#include "panopticon.h"

/****************************************************************************
**	NAMESPACE
****************************************************************************/

using std::cout;

/****************************************************************************
**	GLOBAL VAR PROTOTYPES
****************************************************************************/

//Stores the robot staus variabiles
extern Orangebot::Panopticon g_orangebot_platform;

/****************************************************************************
**	FUNCTION PROTOTYPES
****************************************************************************/

//Aid function that constructs a NODE.JS array from a C++ pointer
template <typename T>
extern Napi::Array construct_array( Napi::Env env, T *array_data, unsigned int array_size );
//Initialize bindings
extern Napi::Object init(Napi::Env env, Napi::Object exports);
//Interface between function and NODE.JS
extern Napi::Number parse_wrap( const Napi::CallbackInfo& info );
//Return a C++ object with all the robot status variables
extern Napi::Object get_status_wrap( const Napi::CallbackInfo& info );
//Prototypes to be declard inside the Orangebot namespace
namespace Orangebot
{
	//Obtain the reference to the robot status vars structure
	extern Orangebot::Panopticon &get_robot_status( void );
}

/****************************************************************************
**	AID FUNCTIONS
****************************************************************************/

/****************************************************************************
**	@brief Function
**	construct_array | Napi::Env, T*, unsigned int
****************************************************************************/
//! @param f bool
//! @return Napi::Array |
//! @details
//! Aid function that constructs a NODE.JS array from a C++ pointer
/***************************************************************************/

template <typename T>
Napi::Array construct_array( Napi::Env env, T *array_data, unsigned int array_size )
{
	//Construct int array
	Napi::Array ret_rmp = Napi::Array::New( env, array_size );
	//For each entry
	for (unsigned int t = 0; t < array_size; t++)
	{
		//Fill entry
		ret_rmp[t] = Napi::Number::New(env, (T)array_data[t] );
	}
	//Return constructed array
	return (Napi::Array)ret_rmp;
}	//End Function: construct_array | Napi::Env, T*, unsigned int

/****************************************************************************
**	BINDINGS
****************************************************************************/

/****************************************************************************
**	@brief Function
**	Init | Napi::Env | Napi::Object
****************************************************************************/
//! @param f bool
//! @return bool |
//! @details
//! Select the methods to be exported to the NODE.JS application and initialize the module
/***************************************************************************/

Napi::Object init(Napi::Env env, Napi::Object exports)
{
	cout << "CPP: Initialize module\n";
	//Initialize the module
	Orangebot::orangebot_node_cpp_init();
	//Register methods accessible from the outside in the NODE.JS environment
	exports.Set( "parse", Napi::Function::New(env, parse_wrap) );
	exports.Set( "get_status", Napi::Function::New(env, get_status_wrap) );

    return exports;
}	//End function: Init | Napi::Env | Napi::Object

/****************************************************************************
**	@brief Function
**	ParseWrapped | Napi::CallbackInfo&
****************************************************************************/
//! @param f bool
//! @return bool |
//! @details
//! Feed a string to Uniparser
//! Uniparser takes care of decoding the string to update relevant fields
/***************************************************************************/

//Interface between function and NODE.JS
Napi::Number parse_wrap(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
	//Check arguments
    if ((info.Length() != 1) || (!info[0].IsString()))
	{
		Napi::TypeError::New(env, "ERR: Expecting one argument of type String").ThrowAsJavaScriptException();
	}
	//Get argument
    Napi::String str = info[0].As<Napi::String>();
    //Convert to standard string
	//std::string std_str = std::string(str);
    //This string has an additional terminator. It must be removed to not mess up the parsing
    //std::string std_str_reduced = std::string( (char *)std_str.c_str(), (int)std_str.length() -2 );
	//Execute function
	//Orangebot::orangebot_parse( std_str_reduced );
	Orangebot::orangebot_parse( std::string( str ) );
	//Return
    return Napi::Number::New(env, (int)0);
} //End Function: ParseWrapped | Napi::CallbackInfo&

/****************************************************************************
**	@brief Function
**	get_status_wrap | Napi::CallbackInfo&
****************************************************************************/
//! @param f bool
//! @return Napi::Object |
//! @details
//! Return a C++ object with all the robot status variables
/***************************************************************************/

Napi::Object get_status_wrap( const Napi::CallbackInfo& info )
{
	Napi::Env env = info.Env();
	//Check arguments
    if (info.Length() != 0)
	{
		Napi::TypeError::New(env, "ERR: Expecting no arguments").ThrowAsJavaScriptException();
	}

	//Get reference to robot status vars
	Orangebot::Panopticon &robot_status = Orangebot::get_robot_status();
	//Construct empty return object in the NODE.JS environment
	Napi::Object ret_tmp = Napi::Object::New( env );
		//!Manually create and fill the fields of the return object
	//Platform Firmware Signature
	ret_tmp.Set("signature", (Napi::String)Napi::String::New( env, (std::string)robot_status.signature() ) );
	//AT4809 Error Code
	ret_tmp.Set("AT4809ERR", (Napi::Number)Napi::Number::New( env, (int)robot_status.at4809_err_code() ) );
	//AT4809 performance counters
	ret_tmp.Set("AT4809CPU", (Napi::Number)Napi::Number::New( env, (int)robot_status.cpu_scan_frequency() ) );
	ret_tmp.Set("AT4809RXI", (Napi::Number)Napi::Number::New( env, (int)robot_status.uart_rxi_bandwidth() ) );
	ret_tmp.Set("AT4809TXO", (Napi::Number)Napi::Number::New( env, (int)robot_status.uart_txo_bandwidth() ) );
	//Platform PWM setting
	ret_tmp.Set("pwm", (Napi::Array)construct_array<int>( env, (int *)&robot_status.pwm( 0 ), NUM_VNH7040PWM) );
	//Encoder absolute position
	ret_tmp.Set("enc_pos", (Napi::Array)construct_array<int>( env, (int *)&robot_status.enc_pos( 0 ), NUM_ENC) );
	//Encoder speed
	ret_tmp.Set("enc_spd", (Napi::Array)construct_array<int>( env, (int *)&robot_status.enc_spd( 0 ), NUM_ENC) );
	//PID Error
	ret_tmp.Set("pid_err", (Napi::Array)construct_array<int>( env, (int *)&robot_status.pid_err( 0 ), NUM_ENC) );
	//PID Slew Rate
	ret_tmp.Set("pid_slew_rate", (Napi::Array)construct_array<int>( env, (int *)&robot_status.pid_slew_rate( 0 ), NUM_ENC) );
	//Return a NODE.JS Object
	return (Napi::Object)ret_tmp;
}	//End Function: get_status_wrap | Napi::CallbackInfo&

NODE_API_MODULE( OrangebotNodeCpp, init )
