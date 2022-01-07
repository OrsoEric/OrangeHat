/**********************************************************************************
**	ENVIROMENT VARIABILE
**********************************************************************************/

#ifndef PANOPTICON_H_
	#define PANOPTICON_H_

/**********************************************************************************
**	GLOBAL INCLUDES
**********************************************************************************/

/**********************************************************************************
**	DEFINES
**********************************************************************************/

//maximum number of character of the board signature
#define MAX_SIGNATURE_LENGTH	32
//Number of VNH7040 Motor driver channels attached to the client board
#define NUM_VNH7040PWM			4
//Number of encoder channels attached to the client board
#define NUM_ENC					2
//Number of numeric parameters in a PID controller
#define NUM_PID_PARAMS			3

/**********************************************************************************
**	MACROS
**********************************************************************************/

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

//! @namespace Orangebot namespace FOREVER!
namespace Orangebot
{

/**********************************************************************************
**	TYPEDEFS
**********************************************************************************/

/**********************************************************************************
**	PROTOTYPE: STRUCTURES
**********************************************************************************/

/**********************************************************************************
**	PROTOTYPE: GLOBAL VARIABILES
**********************************************************************************/

/**********************************************************************************
**	PROTOTYPE: CLASS
**********************************************************************************/

/************************************************************************************/
//! @class 		Panopticon
/************************************************************************************/
//!	@author		Orso Eric
//! @version	0.1 alpha
//! @date		2020-01-17
//! @brief		Store a local copy of robot side parameters
//! @details
//!	This class stores a local copy of hardware robot side parameters \n
//! Communication modules take care of keeping all data synchronized and up to date
//! @bug		None
//! @warning	No warnings
//! @copyright	License ?
//! @todo		todo list
/************************************************************************************/

class Panopticon
{
	//Visible to all
	public:
		//--------------------------------------------------------------------------
		//	CONSTRUCTORS
		//--------------------------------------------------------------------------

		//! Default constructor
		Panopticon( void );

		//--------------------------------------------------------------------------
		//	DESTRUCTORS
		//--------------------------------------------------------------------------

		//!Default destructor
		~Panopticon( void );

		//--------------------------------------------------------------------------
		//	OPERATORS
		//--------------------------------------------------------------------------

		//--------------------------------------------------------------------------
		//	SETTERS
		//--------------------------------------------------------------------------

		//--------------------------------------------------------------------------
		//	GETTERS
		//--------------------------------------------------------------------------

		//--------------------------------------------------------------------------
		//	REFERENCES
		//--------------------------------------------------------------------------

		//Reference to platform signature
		std::string &signature( void );
		//AT4809 Error Code
		int &at4809_err_code( void );
		//AT4809 Performance counters
		int &cpu_scan_frequency( void );
		int &uart_rxi_bandwidth( void );
		int &uart_txo_bandwidth( void );
		//Reference to PWM field
		int &pwm( int index );
		//Reference to encoder count field
		int &enc_pos( int index );
		//Reference to encoder speed field
		int &enc_spd( int index );
		//Reference to PID Error
		int &pid_err( int index );
		//Reference to PID Slew Rate
		int &pid_slew_rate( int index );

		//--------------------------------------------------------------------------
		//	TESTERS
		//--------------------------------------------------------------------------

		//--------------------------------------------------------------------------
		//	PUBLIC METHODS
		//--------------------------------------------------------------------------

		//Varidac function that takes care of building a message
		//uint8_t build_tx_message( const char *msg, ... );

		//--------------------------------------------------------------------------
		//	PUBLIC STATIC METHODS
		//--------------------------------------------------------------------------

		//--------------------------------------------------------------------------
		//	PUBLIC VARS
		//--------------------------------------------------------------------------

	//Visible to derived classes
	protected:
		//--------------------------------------------------------------------------
		//	PROTECTED METHODS
		//--------------------------------------------------------------------------

		//--------------------------------------------------------------------------
		//	PROTECTED VARS
		//--------------------------------------------------------------------------

	//Visible only inside the class
	private:
		//--------------------------------------------------------------------------
		//	PRIVATE METHODS
		//--------------------------------------------------------------------------

		//Initialize class variables
		void init( void );
		//dummy method for easy copy
		bool dummy( void );

		//--------------------------------------------------------------------------
		//	PRIVATE VARS
		//--------------------------------------------------------------------------

		//Dummy for the check of reference access methods
		int g_dummy;
		//Board signature
		std::string g_signature;
		//Error code of the AT4809
		int g_at4809_err;
		//Scan frequency
		int g_cpu_scan_frequency;
		//UART bandwidth
		int g_uart_rxi_bandwidth;
		int g_uart_txo_bandwidth;
		//Latest PWM packet reading
		int g_pwm[ NUM_VNH7040PWM ];
		//Encoder channel readings
		int g_enc_pos[ NUM_ENC ];
		//Encoder speed reading
		int g_enc_spd[ NUM_ENC ];
		//Error of the PID controller
		int g_pid_err[ NUM_ENC ];
		//Slew rate of the command
		int g_pid_slew_rate[ NUM_ENC ];

};	//End Class: Panopticon

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

} //End Namespace

#else
    #warning "Multiple inclusion of hader file"
#endif
