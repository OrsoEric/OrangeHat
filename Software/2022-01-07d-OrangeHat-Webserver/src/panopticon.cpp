/****************************************************************************
**	OrangeBot Project
*****************************************************************************
**        /
**       /
**      /
** ______ \
**         \
**          \
*****************************************************************************
**	Panopticon
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
**
****************************************************************************/

/****************************************************************************
**	DESCRIPTION
*****************************************************************************
**	Stores a platform status variables
****************************************************************************/

/****************************************************************************
**	KNOWN BUG
*****************************************************************************
**
****************************************************************************/

/****************************************************************************
**	INCLUDES
****************************************************************************/

#include <cstdio>
#include <iostream>
//Debug trace log
//#define ENABLE_DEBUG
#include "debug.h"
//Class Header
#include "panopticon.h"

/****************************************************************************
**	NAMESPACES
****************************************************************************/

namespace Orangebot
{

/****************************************************************************
**	GLOBAL VARIABILES
****************************************************************************/

/****************************************************************************
*****************************************************************************
**	CONSTRUCTORS
*****************************************************************************
****************************************************************************/

/***************************************************************************/
//!	@brief Empty Constructor
//!	Panopticon | void
/***************************************************************************/
//! @return no return
//!	@details
//! Empty constructor
/***************************************************************************/

Panopticon::Panopticon( void )
{
	//Trace Enter
	DENTER();

	///--------------------------------------------------------------------------
	///	VARS
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Initialize class variables
	this -> init();

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	//Trace Return
	DRETURN();

	return;	//OK
}	//end constructor:

/****************************************************************************
*****************************************************************************
**	DESTRUCTORS
*****************************************************************************
****************************************************************************/

/***************************************************************************/
//!	@brief Empty Destructor
//!	Panopticon | void
/***************************************************************************/
//! @return no return
//!	@details
//! Empty destructor
/***************************************************************************/

Panopticon::~Panopticon( void )
{
	//Trace Enter
	DENTER();

	///--------------------------------------------------------------------------
	///	VARS
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	//Trace Return
	DRETURN();

	return;	//OK
}	//end destructor:

/****************************************************************************
*****************************************************************************
**	OPERATORS
*****************************************************************************
****************************************************************************/

/****************************************************************************
*****************************************************************************
**	SETTERS
*****************************************************************************
****************************************************************************/

/****************************************************************************
*****************************************************************************
**	GETTERS
*****************************************************************************
****************************************************************************/

/****************************************************************************
*****************************************************************************
**	REFERENCES
*****************************************************************************
****************************************************************************/

/***************************************************************************/
//!	@brief Public Reference
//!	at4809_err_code | void
/***************************************************************************/
//! @return int& reference to AT4809 Error Code
//!	@details
//! AT4809 Error Code
/***************************************************************************/

int &Panopticon::at4809_err_code( void )
{
	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	return this -> g_at4809_err;
}	//end method: at4809_err_code | void

/***************************************************************************/
//!	@brief Public Reference
//!	cpu_scan_frequency | void
/***************************************************************************/
//! @return int& reference to AT4809 CPU scan frequency
//!	@details
//! AT4809 performance counter
/***************************************************************************/

int &Panopticon::cpu_scan_frequency( void )
{
	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	return this -> g_cpu_scan_frequency;
}	//end method: cpu_scan_frequency | void

/***************************************************************************/
//!	@brief Public Reference
//!	uart_rxi_bandwidth | void
/***************************************************************************/
//! @return int& reference to AT4809 bandwidth
//!	@details
//! AT4809 performance counter
/***************************************************************************/

int &Panopticon::uart_rxi_bandwidth( void )
{
	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	return this -> g_uart_rxi_bandwidth;
}	//end method: uart_rxi_bandwidth | void

/***************************************************************************/
//!	@brief Public Reference
//!	g_uart_txo_bandwidth | void
/***************************************************************************/
//! @return int& reference to AT4809 bandwidth
//!	@details
//! AT4809 performance counter
/***************************************************************************/

int &Panopticon::uart_txo_bandwidth( void )
{
	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	return this -> g_uart_txo_bandwidth;
}	//end method: g_uart_txo_bandwidth | void

/***************************************************************************/
//!	@brief Public Reference
//!	pwm | int
/***************************************************************************/
//! @param index | uint8_t | index of the PWM channel to be addressed
//! @return int& reference to PWM channel
//!	@details
//! Directly address a private var from the outside
/***************************************************************************/

int &Panopticon::pwm( int index )
{
	//If: bad number of channels
	if (index >= NUM_VNH7040PWM)
	{
		DPRINT("ERR: <%s> | bad index: %d\n", __FUNCTION__, index);
        //Address a dummy variable so not to dirty actual useful fields and not cause memory violation access
        return this -> g_dummy; //FAIL
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	return this -> g_pwm[ index ];	//OK
}	//end method: pwm | int

/***************************************************************************/
//!	@brief Public Reference
//!	enc_pos | int
/***************************************************************************/
//! @param index | uint8_t | index of the encoder absolute reading
//! @return int& reference to PWM channel
//!	@details
//! Directly address a private var from the outside
/***************************************************************************/

int &Panopticon::enc_pos( int index )
{
	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	//If: bad number of channels
	if (index >= NUM_ENC)
	{
		DPRINT("ERR: <%s> | bad index: %d\n", __FUNCTION__, index);
        //Address a dummy variable so not to dirty actual useful fields and not cause memory violation access
        return g_dummy; //FAIL
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	return this -> g_enc_pos[ index ];	//OK
}	//end method: enc_pos | int

/***************************************************************************/
//!	@brief Public Reference
//!	enc_spd | int
/***************************************************************************/
//! @param index | uint8_t | index of the encoder speed reading
//! @return int& reference to PWM channel
//!	@details
//! Directly address a private var from the outside
/***************************************************************************/

int &Panopticon::enc_spd( int index )
{
	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	//If: bad number of channels
	if (index >= NUM_ENC)
	{
		DPRINT("ERR: <%s> | bad index: %d\n", __FUNCTION__, index);
        //Address a dummy variable so not to dirty actual useful fields and not cause memory violation access
        return g_dummy; //FAIL
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	return this -> g_enc_spd[ index ];	//OK
}	//end method: enc_spd | int

/***************************************************************************/
//!	@brief Public Reference
//!	pid_err | int
/***************************************************************************/
//! @param index | uint8_t | index of the encoder speed reading
//! @return int& reference to PID Error
//!	@details
//! Directly address a private var from the outside
/***************************************************************************/

int &Panopticon::pid_err( int index )
{
	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	//If: bad number of channels
	if (index >= NUM_ENC)
	{
		DPRINT("ERR: <%s> | bad index: %d\n", __FUNCTION__, index);
        //Address a dummy variable so not to dirty actual useful fields and not cause memory violation access
        return g_dummy; //FAIL
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	return this -> g_pid_err[ index ];	//OK
}	//end method: pid_err | int

/***************************************************************************/
//!	@brief Public Reference
//!	pid_slew_rate | int
/***************************************************************************/
//! @param index | uint8_t | index of the encoder speed reading
//! @return int& reference to PID Error
//!	@details
//! Directly address a private var from the outside
/***************************************************************************/

int &Panopticon::pid_slew_rate( int index )
{
	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	//If: bad number of channels
	if (index >= NUM_ENC)
	{
		DPRINT("ERR: <%s> | bad index: %d\n", __FUNCTION__, index);
        //Address a dummy variable so not to dirty actual useful fields and not cause memory violation access
        return g_dummy; //FAIL
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	return this -> g_pid_slew_rate[ index ];	//OK
}	//end method: pid_slew_rate | int

/***************************************************************************/
//!	@brief Public Reference
//!	signature | void
/***************************************************************************/
//! @return std::string& reference to signature
//!	@details
//! Reference to platform signature
/***************************************************************************/

std::string &Panopticon::signature( void )
{
	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	return this -> g_signature;	//OK
}	//end method: enc_abs | int

/****************************************************************************
*****************************************************************************
**	TESTERS
*****************************************************************************
****************************************************************************/

/****************************************************************************
*****************************************************************************
**	PUBLIC METHODS
*****************************************************************************
****************************************************************************/

/****************************************************************************
*****************************************************************************
**	PUBLIC STATIC METHODS
*****************************************************************************
****************************************************************************/

/****************************************************************************
*****************************************************************************
**	PRIVATE METHODS
*****************************************************************************
****************************************************************************/

/***************************************************************************/
//!	@brief Private Method
//!	init | void
/***************************************************************************/
//! @return no return
//!	@details
//! Initialize class variables
/***************************************************************************/

void Panopticon::init( void )
{
	//Trace Enter
	DENTER();

	///--------------------------------------------------------------------------
	///	VARS
	///--------------------------------------------------------------------------

	//counter
	int t;

	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Initialize error code to OK
	this -> g_at4809_err = 0;
	this -> g_cpu_scan_frequency = 0;
	this -> g_uart_rxi_bandwidth = 0;
	this -> g_uart_txo_bandwidth = 0;
	//For each DC motor channel
	for (t = 0; t< NUM_VNH7040PWM;t++)
	{
		//Initialize PWM
		this -> g_pwm[t] = 0;
	}

	//For each Encoder channel
	for (t = 0; t< NUM_ENC;t++)
	{
		//Initialize encoder count
		this -> g_enc_pos[t] = 0;
		this -> g_enc_spd[t] = 0;
		//Initialize PID status performance
		this -> g_pid_err[t] = 0;
		this -> g_pid_slew_rate[t] = 0;
	}

	//For each PID parameter channel
	for (t = 0; t< NUM_ENC;t++)
	{

	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	//Trace Return
	DRETURN();
	return;
}	//end method: init | void

/***************************************************************************/
//!	@brief Public Method
//!	Dummy | void
/***************************************************************************/
// @param
//! @return no return
//!	@details
//! Method
/***************************************************************************/

bool Panopticon::dummy( void )
{
	//Trace Enter
	DENTER();

	///--------------------------------------------------------------------------
	///	VARS
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	//Trace Return
	DRETURN();
	return false;	//OK
}	//end method:

/****************************************************************************
**	NAMESPACES
****************************************************************************/

} //End Namespace
