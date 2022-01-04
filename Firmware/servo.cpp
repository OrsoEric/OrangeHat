/**********************************************************************************
BSD 3-Clause License

Copyright (c) 2022, Orso Eric
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********************************************************************************/

/**********************************************************************************
**  ENVIROMENT VARIABILE
**********************************************************************************/

#ifndef SERVO_H_
    #define SERVO_H_

/**********************************************************************************
**  GLOBAL INCLUDES
**********************************************************************************/

//Bit Sign Types
#include <stdint.h>
//Program wide definitions and macros
#ifndef GLOBAL_H_
	#include "global.h"
#endif
//Useful macros
#ifndef AT_UTILS_H_
	#include "at_utils.h"
#endif
//define the ISR routune, ISR vector, and the sei() cli() function
#include <avr/interrupt.h>
//name all the register and bit
#include <avr/io.h>
//hard coded delay
#include <util/delay.h>

/**********************************************************************************
**  DEFINES
**********************************************************************************/

//Enable the file trace debugger
//#define ENABLE_DEBUG
//file trace debugger
#ifdef ENABLE_DEBUG
    #include <cstdio.h> 
    #include "debug.h"
#endif
//If DEBUG is not needed, blank out the implementations
#ifndef DEBUG_H_
    #define DEBUG_VARS_PROTOTYPES()
    #define DEBUG_VARS()
    #define DSHOW( ... )
    #define DSTART( ... )
    #define DSTOP()
    #define DTAB( ... )
    #define DPRINT( ... )
    #define DPRINT_NOTAB( ... )
    #define DENTER( ... )
    #define DRETURN( ... )
    #define DENTER_ARG( ... )
    #define DRETURN_ARG( ... )
#endif

/**********************************************************************************
**  MACROS
**********************************************************************************/

/**********************************************************************************
**  NAMESPACE
**********************************************************************************/

//! @namespace custom namespace
namespace OrangeBot
{

/**********************************************************************************
**  TYPEDEFS
**********************************************************************************/

/**********************************************************************************
**  PROTOTYPE: STRUCTURES
**********************************************************************************/

/**********************************************************************************
**  PROTOTYPE: GLOBAL VARIABILES
**********************************************************************************/

//Pointer to driver call
//void *gpf_tca0_ovf_isr_call = nullptr;

/**********************************************************************************
**  PROTOTYPE: CLASS
**********************************************************************************/

/************************************************************************************/
//! @class      Servo
/************************************************************************************/
//!	@author     Orso Eric
//! @version    2021-12-23
//! @brief      Servo Class
//! @copyright  BSD 3-Clause License Copyright (c) 2021, Orso Eric
//! @details
//! \n  Event based servo motor handler for microcontroller
//! \n  The first working code was first developed for Unit Zero running on an AT324
//! \n  OrangeBot uses an interface in class form that can be recycled
//! \n  The Engine comes from Unit Zero
//! \n  The class uses a single high resolution event timer to schedule pin change of servo channels
//! \n  Modulation is PPM, 50Hz with a deadtime of 1.5ms by default
//! \n  Class features an HAL to allow microcontroller specific hardware code interface to be encapsulated
//! \n  Use inheritance to inherit the interface class Servo_hal written for the implementation
//! \n		HISTORY
//! \n		2021-12-26
//! \n	Modified OrangeHat PCB and added a SoftStart to SERVO power to fix the brownout reset when powering servo bank
//! \n		2022-01-01
//! \n	TCA0 successfully called as a delay ISR timer to toggle SERVO0 every 20ms
//! \n		2022-01-03
//! \n	Driver expanded to control SERVO0 with 1.5ms and 20ms-1.5ms delay
//! \n	Physical servo connected and verified that it holds the position correctly
//! \n		2022-01-04
//! \n	Driver now handles scans of multiple servos
//! \n	IO of all servos programmed in
//! \n	Added some layers of error handling and safety check. emergency_stop function
/************************************************************************************/

class Servo
{
    //Visible to all
    public:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  PUBLIC ENUMS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
	
        //! @brief Configurations of the class
        typedef enum _Config
        {
				//Safeties. Disabling safeties will improve performance but make debug harder.
			//Check external inputs
            SAFETY_CHECK = true,
			//Check internal inputs and status variables
            PEDANTIC_CHECKS = true,
			
				//PIN Configurations
			//IO_PWR_PORT = PORTD,
			IO_PWR_PIN = 7,
			//Number of Servo the driver is meant to drive
			NUM_SERVOS = 2,
			
				//HAL Timer Parameters
			//Prescaler to the main timer clock
			HAL_TIMER_PRESCALER = 16,
			HAL_TIMER_MAX_CNT = (UINT16_MAX -1),
			//Resolution of a single count in nanoseconds per count [ns/cnt]. Needed to convert between time and delay to be programmed inside the hardware timer
			//HAL_TIMER_RESOLUTION = ((1000000000UL /F_CPU) /HAL_TIMER_PRESCALER ),
			HAL_TIMER_RESOLUTION = 800,
			
				//Servomotor PPM Parameters
			//Pulse width to have a zero output in microseconds [us]
			SERVO_PPM_ZERO = 1500,
			//Period of a single servo waveform in microseconds [us]
			SERVO_PPM_PERIOD = 20000,
			//Deviation from the deadtime that gives either a full forward or a full backward command in microseconds [us]
			SERVO_PPM_MAX_COMMAND = 400,
				
				//Servomotor Power Soft start configurations
			SOFT_START_TON = 10,
			SOFT_START_TOFF = 100,
			SOFT_START_TREDUCE = 1,
			SOFT_START_REPEAT = 2,
			SOFT_START_DELAY_US = 1,
			
        } Config;
        
        //! @brief Error codes of the class
        typedef enum _Error_code
        {
            OK,             //OK
			ERR_VAR_INIT,	//Failed to initialize vars
			ERR_HAL_INIT,	//Failed to initialize HAL
			ERR_TA0_INIT,	//Failed to initialize TA0
			ERR_BAD_SERVO_INDEX,	//Tried to index a SERVO that does not exist
			ERR_DELAY_OVERRUN,	//The programmed delay to generate the PPM signal has been exceeded
			ERR_OVERFLOW,		//Overflow occurred during calculations
			ERR_INPUT_OOB,		//Function received parameters out of range
			ERR_ALGORITHM,		//Algorithmic error. Internal vars are inconsistent or a piace of code that shouldn't fail has failed
            RECOVERY_FAIL,  //error_recovery() failed to recover from an error
        } Error_code;

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  PUBLIC TYPEDEFS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
		//! @brief Internal timer count vars in microseconds, the drivers keep them updated
		typedef struct _Timer
		{
			//Target delay in microseconds [us]
			uint16_t u16_target;
			//Max delay change in microseconds [us] per period 20[ms]
			uint8_t u8_speed;
			//Actual delay being rendered to the servo in microseconds [us]
			uint16_t u16_actual;
		} Timer;
		
		//! @brief User side command to a servo in microseconds
		typedef struct _Command
		{
			//Target position in microseconds of deviation from the zero position [us]. e.g. 1900[us]
			int16_t s16_position;
			//Max change in microseconds [us] allowed each second. e.g. 800[us/s] means it can change its full range in one second. can be faster/slower depending on the physical servo
			uint16_t u16_speed;
			//Writing to the command sets those flags to true. The driver clears the flags when writing back to the internal delay registers
			bool u1_position_changed;
			bool u1_speed_changed;
			//Driver sets this flag to true when the actual is changing slower than the maximum set speed
			bool u1_lock;
			//Driver sets this flag to true when the actual has not been changed
			bool u1_idle;
		} Command;
		
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  CONSTRUCTORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Empty Constructor
        Servo( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  DESTRUCTORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Empty destructor
        ~Servo( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  PUBLIC OPERATORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  PUBLIC SETTERS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

		//Set position of servo with given index (same as overload of operator at [])
		bool set_servo( uint8_t iu8_index, int16_t is16_pos );
		//Set position and speed of servo with given index
		bool set_servo( uint8_t iu8_index, int16_t is16_pos, uint16_t iu16_spd );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  PUBLIC GETTERS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Get current error state of the library
        Error_code get_error( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  PUBLIC TESTERS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  PUBLIC METHODS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

		//Initialize the Servo class. If success, the driver is running. The ISR is called periodically to execute all the OCR.
		bool init( void );

		//Power ON or OFF the SERVOs
		bool power( bool iu1_powered );
		
		//Driver code executed by the timer ISR
		bool hal_timer_isr( void );
		
		//Stop driver. Clear IO lines. Reinitialize class vars.
		bool emergency_stop( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  PUBLIC STATIC METHODS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  PUBLIC VARS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

    //Visible to derived classes
    protected:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  PROTECTED METHODS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  PROTECTED VARS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

    //Visible only inside the class
    private:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE ENUM
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE TYPEDEFS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
    
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE HAL
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

		//Initialize the HAL
		bool init_hal( void );

        //HAL driver. Initialize IO pins
        bool hal_init_io( void );
		//HAL Driver. Initialize TA0 to launch an ISR when a given time is expired a single time
		bool hal_init_ta0_timeout_mode( uint16_t iu16_top );
        //Set the IO level of a given servo pin to a given value
        //bool hal_servo_io( uint8_t iu8_index, bool iu1_value );
        //HAL driver. Initialize 
        //bool hal_init_timer( void );
		//Fixed Delay
		bool hal_delay_us( void );
		//Convert from a delay in microseconds to a delay in timer counts that can be programmed inside the timer
		uint16_t hal_microseconds_to_counts( uint16_t iu16_microseconds );
		//Program a delay inside the timer
		bool hal_timer_set_delay( uint16_t iu16_microseconds );
		//Set power pin of the SERVO banks. false = disconnected | true = powered
		bool hal_power( bool iu8_powered );
		//Set a physical IO pin of a servo with a given {index} to the desired {value}
		bool hal_servo_io( uint8_t iu8_servo_index, bool iu1_io_value );

		//Ask the ISR of TA0 to run or stop
		bool hal_run_isr( bool iu1_running );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE INIT
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
        //Initialize class vars
        bool init_class_vars( void );
        
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE METHODS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
        //Report an error. return false: OK | true: Unknown error code 
        bool report_error( Error_code error_code_tmp );
        //Tries to recover from an error. Automatically called by get_error. return false = OK | true = fail
        bool error_recovery( void );
		//Blocking hardwired version of the soft start method using delay and direct control of IOs
		bool soft_start( uint8_t iu8_ton, uint8_t iu8_toff, uint8_t iu8_treduce, uint8_t iu8_repeat );
		//Clear all servo IO lines
		bool clear_servo_io( void );
		//The driver uses command position and speed, and actual delay, to compute next delay
		bool compute_servo_delay( uint8_t iu8_index, uint16_t &ou16_delay );

        //Servo method to copy the code
        bool dummy( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE VARS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

		//Boilerplate: find a way to configure port in enum
		//PORT_t IO_PWR_PORT;

		//Index of the scan. e.g. 1 means that the driver is waiting for Servo 0 delay to elapse
		uint8_t gu8_index;
		//User given command to servos (microseconds)
		Command gast_command[ Config::NUM_SERVOS ];
		//Driver internal servo status (microseconds)
		Timer gast_timer[ Config::NUM_SERVOS ];
		//Sum of all the delays over a servo scan. Used to compute the final delay to get to the period
		uint16_t gu16_timer_sum;
        //! @brief Error code of the class
        Error_code g_error;

};	//End Class: Servo

//BOILERPLATE: when including the class as definition, the bits not needed are excluded by saying that the file only needs the definition
//	This will have problems for template based declarations and is a boilerplate. TODO: do it properly
#ifndef USER_INIBITH_IMPLEMENTATION

/*********************************************************************************************************************************************************
**********************************************************************************************************************************************************
**	CONSTRUCTORS
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

/***************************************************************************/
//! @brief Constructor
//! \n Servo | void
/***************************************************************************/
// @param
//! @return no return
//! @details
//! \n Empty constructor
/***************************************************************************/

Servo::Servo( void )
{
    DENTER();   //Trace Enter
    ///--------------------------------------------------------------------------
    ///	BODY
    ///--------------------------------------------------------------------------

	//Initialize servo class
	this -> init();

    ///--------------------------------------------------------------------------
    ///	RETURN
    ///--------------------------------------------------------------------------
    DRETURN();  //Trace Return
    return;
}   //end constructor: Servo | void

/*********************************************************************************************************************************************************
**********************************************************************************************************************************************************
**	DESTRUCTORS
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief Destructor
//! \n Servo | void
/***************************************************************************/
// @param
//! @return no return
//! @details
//! \n Empty destructor
/***************************************************************************/

Servo::~Servo( void )
{
    DENTER();		//Trace Enter
    ///--------------------------------------------------------------------------
    ///	INIT
    ///--------------------------------------------------------------------------

    ///--------------------------------------------------------------------------
    ///	BODY
    ///--------------------------------------------------------------------------

    ///--------------------------------------------------------------------------
    ///	RETURN
    ///--------------------------------------------------------------------------
    DRETURN();      //Trace Return
    return;         //OK
}   //end destructor: Servo | void

/*********************************************************************************************************************************************************
**********************************************************************************************************************************************************
**	PUBLIC GETTERS
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

/***************************************************************************/
//! @brief Public getter
//! \n get_error | void |
/***************************************************************************/
//! @return Error_code
//! @details
//! \n Get current error state of the library
//! \n Try to recover from errors
/***************************************************************************/

inline Servo::Error_code Servo::get_error( void )
{
    DENTER(); //Trace Enter
    ///--------------------------------------------------------------------------
    ///	BODY
    ///--------------------------------------------------------------------------

    //Fetch error
    Error_code err_code = this -> g_error;
    //Try to recover from error
    bool f_ret = this -> error_recovery();
    //If: failed to recover
    if (f_ret == true)
    {
        //Do nothing
    }

    ///--------------------------------------------------------------------------
    ///	RETURN
    ///--------------------------------------------------------------------------
    DRETURN(); //Trace Return
    return err_code; //OK
}   //end getter: get_error | void |

/*********************************************************************************************************************************************************
**********************************************************************************************************************************************************
**  PUBLIC SETTERS
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

/***************************************************************************/
//! @brief Public setter
//! \n set_servo | uint8_t | int16_t |
/***************************************************************************/
//! @param iu8_index | index of the servo being controlled
//! @param is16_pos | target position. User must have programmed in the configuration for this servo channel
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n Set position of servo with given index (same as overload of operator at [])
/***************************************************************************/

bool Servo::set_servo( uint8_t iu8_index, int16_t is16_pos )
{
	DENTER_ARG("index: %d | position: %d\n", iu8_index, is16_pos); //Trace Enter
	///--------------------------------------------------------------------------
	///	CHECK
	///--------------------------------------------------------------------------

	//If Servo index is OOB
	if ((Config::SAFETY_CHECK == true) && ((iu8_index < 0) || (iu8_index >= Config::NUM_SERVOS)))
	{
		this -> report_error( Error_code::ERR_INPUT_OOB );
		return true;
	}
	//If position is OOB
	if ((Config::SAFETY_CHECK == true) && ((is16_pos < -(int16_t)Config::SERVO_PPM_MAX_COMMAND) || (is16_pos > (int16_t)Config::SERVO_PPM_MAX_COMMAND)))
	{
		this -> report_error( Error_code::ERR_INPUT_OOB );
		return true;
	}
	
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Update command and inform driver that command has changed
	if (this -> gast_command[iu8_index].s16_position != is16_pos)
	{
		this -> gast_command[iu8_index].s16_position = is16_pos;	
		this -> gast_command[iu8_index].u1_position_changed = true;
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN(); //Trace Return
	return false; //OK
}   //end setter: set_servo | uint8_t | int16_t |

/***************************************************************************/
//! @brief Public setter
//! \n set_servo | uint8_t | int16_t | uint16_t |
/***************************************************************************/
//! @param iu8_index | index of the servo being controlled
//! @param is16_pos | target position. User must have programmed in the configuration for this servo channel
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n Set position and speed of servo with given index
/***************************************************************************/

bool Servo::set_servo( uint8_t iu8_index, int16_t is16_pos, uint16_t iu16_spd )
{
	DENTER_ARG("index: %d | position: %d\n", iu8_index, is16_pos); //Trace Enter
	///--------------------------------------------------------------------------
	///	CHECK
	///--------------------------------------------------------------------------

	//If Servo index is OOB
	if ((Config::SAFETY_CHECK == true) && ((iu8_index < 0) || (iu8_index >= Config::NUM_SERVOS)))
	{
		this -> report_error( Error_code::ERR_INPUT_OOB );
		return true;
	}
	//If position is OOB
	if ((Config::SAFETY_CHECK == true) && ((is16_pos < -(int16_t)Config::SERVO_PPM_MAX_COMMAND) || (is16_pos > (int16_t)Config::SERVO_PPM_MAX_COMMAND)))
	{
		this -> report_error( Error_code::ERR_INPUT_OOB );
		return true;
	}
	
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Update command and inform driver that command has changed
	if (this -> gast_command[iu8_index].s16_position != is16_pos)
	{
		this -> gast_command[iu8_index].s16_position = is16_pos;
		this -> gast_command[iu8_index].u1_position_changed = true;
	}
	//Update command and inform driver that command has changed
	if (this -> gast_command[iu8_index].u16_speed != iu16_spd)
	{
		this -> gast_command[iu8_index].u16_speed = iu16_spd;
		this -> gast_command[iu8_index].u1_speed_changed = true;
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN(); //Trace Return
	return false; //OK
}   //end setter: set_servo | uint8_t | int16_t | uint16_t |

/*********************************************************************************************************************************************************
**********************************************************************************************************************************************************
**	PUBLIC METHODS
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

/***************************************************************************/
//! @brief public method
//! \n init | void
/***************************************************************************/
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n	Master init method
//! \n		Initialize class vars
//! \n		Initialize the HAL
/***************************************************************************/

bool Servo::init( void )
{
	DENTER(); //Trace Enter
	///--------------------------------------------------------------------------
	///	VARS
	///--------------------------------------------------------------------------

	//Error propagation
	bool u1_ret = false;

	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Initialize class vars
	u1_ret |= this -> init_class_vars();
	if (u1_ret == true)
	{
		this -> report_error( Error_code::ERR_VAR_INIT );
	}

	//Initialize the IOs
	u1_ret |= this -> hal_init_io();
	if (u1_ret == true)
	{
		this -> report_error( Error_code::ERR_HAL_INIT );
	}
	
	//Initialize the TA0
	u1_ret |= this -> hal_init_ta0_timeout_mode( 24999 );
	if (u1_ret == true)
	{
		this -> report_error( Error_code::ERR_TA0_INIT );
	}

	//Set all servos to their dead time

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN(); //Trace Return
	return u1_ret;	//Propagate error
}   //end public method: init | void

/***************************************************************************/
//! @brief public method
//! \n power | void
/***************************************************************************/
//! @param iu1_powered | bool | false = disconnected | true = powered |
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n	power the servomotors
//! \n	starts the ISR driver to scan the servos
/***************************************************************************/

bool Servo::power( bool iu1_powered )
{
	DENTER_ARG("SERVO power: %d", iu1_powered); //Trace Enter
	///--------------------------------------------------------------------------
	///	VAR
	///--------------------------------------------------------------------------
	
	//Return propagation
	bool u1_ret;
	
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//SERVO OFF
	if (iu1_powered == false)
	{
		//Turn off SERVO bank
		u1_ret = this -> hal_power( false );
		//Stop the ISR
		this ->hal_run_isr( false );
		
	}
	//SERVO ON
	else
	{
		//Use a blocking soft start delay based routine
		u1_ret = this -> soft_start( Config::SOFT_START_TON, Config::SOFT_START_TOFF, Config::SOFT_START_TREDUCE, Config::SOFT_START_REPEAT );
		//Start the ISR
		this -> hal_run_isr( true );
	}




	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN_ARG("Success: %d", u1_ret); //Trace Return
	return u1_ret;	//Propagate Error
}   //end public method: power | void

/***************************************************************************/
//! @brief public method
//! \n hal_servo_io | uint8_t | bool
/***************************************************************************/
//! @param iu8_servo_index | index of the physical servo being addressed
//! @param iu1_io_value | desired IO line value
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n Set a physical IO pin of a servo with a given {index} to the desired {value}
//! \n	Fails when the desired servo does not exist
/***************************************************************************/

bool Servo::hal_servo_io( uint8_t iu8_servo_index, bool iu1_io_value )
{
	DENTER_ARG("Servo: %d -> IO: %d\n", iu8_servo_index, iu1_io_value); //Trace Enter
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	switch( iu8_servo_index )
	{
		//Drive IO line of SERVO
		case 0:
			SET_BIT_VALUE( PORTE.OUT, 0, iu1_io_value );
			break;
		case 1:
			SET_BIT_VALUE( PORTE.OUT, 1, iu1_io_value );
			break;
		case 2:
			SET_BIT_VALUE( PORTE.OUT, 2, iu1_io_value );
			break;
		case 3:
			SET_BIT_VALUE( PORTE.OUT, 3, iu1_io_value );
			break;
		case 4:
			SET_BIT_VALUE( PORTF.OUT, 0, iu1_io_value );
			break;
		case 5:
			SET_BIT_VALUE( PORTF.OUT, 1, iu1_io_value );
			break;
		case 6:
			SET_BIT_VALUE( PORTF.OUT, 2, iu1_io_value );
			break;
		case 7:
			SET_BIT_VALUE( PORTF.OUT, 3, iu1_io_value );
			break;
		default:
			this -> report_error( Error_code::ERR_BAD_SERVO_INDEX );
			return true;
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN(); //Trace Return
	return false;	//OK
}   //end public method: hal_servo_io | uint8_t | bool

/***************************************************************************/
//! @brief public method
//! \n hal_timer_isr | void
/***************************************************************************/
// @param
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n Method called by the overflow ISR of the timer
//! \n Toggles the servo pin and compute the next OCR
//! \n Re-arms or Stop the timer
/***************************************************************************/

bool Servo::hal_timer_isr( void )
{
	///--------------------------------------------------------------------------
	///	VARS
	///--------------------------------------------------------------------------
 
	//Return fail initialized to OK
	bool u1_ret = false;

	///--------------------------------------------------------------------------
	///	SERVO IOs
	///--------------------------------------------------------------------------
	//	Emit the correct IO lines

	//Fetch scan index
	uint8_t u8_index = this -> gu8_index;
	//First Servo: SET servo line
	if (u8_index == 0)
	{
		u1_ret = this -> hal_servo_io( u8_index, true );
	}
	//Inner servos
	else if ((u8_index > 0) && (u8_index < Config::NUM_SERVOS))
	{
		//Clear previous servo line
		u1_ret = this -> hal_servo_io( u8_index-1, false );
		//SET current servo line
		u1_ret |= this -> hal_servo_io( u8_index, true );
	}
	//Last Servo: CLEAR servo line
	else if (u8_index == Config::NUM_SERVOS)
	{
		//Clear previous servo line
		u1_ret = this -> hal_servo_io( u8_index -1, false );
	}
	//Algorithmic Error
	else
	{
		//FAIL
		u1_ret = true;
	}
	
	//If Fail
	if (u1_ret == true)
	{
		//SCRAM!!!
		u1_ret = this -> emergency_stop();
		return u1_ret;
	}

	///--------------------------------------------------------------------------
	///	COMPUTE DELAY
	///--------------------------------------------------------------------------
	//	Compute and program the delay for the next cycle
	
	//Temp delay in microseconds. HAL takes care of programming a delay in timer units
	uint16_t u16_delay;
	//Latch accumulator
	uint16_t u16_accumulator = gu16_timer_sum;
	//Compute Delay for servos
	if (u8_index < Config::NUM_SERVOS)
	{
		//Driver takes care of updating from user command, maintaining servo status and computing what the next delay is to be
		u1_ret = this -> compute_servo_delay( u8_index, u16_delay );
		//Accumulate delay inside accumulator
		u16_accumulator += u16_delay;
	}
	//Compute final delay and clear accumulator
	else if (u8_index == Config::NUM_SERVOS)
	{
		//If: the servo scan time exceed the PPM period
		if (u16_accumulator >= Config::SERVO_PPM_PERIOD)
		{
			//Fail
			u1_ret = true;
		}
		//If: scan time is good
		else
		{
			//Final delay is meant to achieve the PPM period
			u16_delay = Config::SERVO_PPM_PERIOD -u16_accumulator;
			//Reset the accumulator for the next cycle
			u16_accumulator = 0;	
		}
	}
	//Algorithmic error
	else
	{
		//Fail
		u1_ret = true;	
	}
	//If fail
	if (u1_ret == true)
	{
		//SCRAM!!!
		u1_ret = this -> emergency_stop();
		return u1_ret;
	}
	
	//Program the timer with the desired delay. ISR will be called again when said delay has elapsed
	u1_ret = this -> hal_timer_set_delay( u16_delay );
	//Fail
	if (u1_ret == true)
	{
		//SCRAM!!!
		u1_ret = this -> emergency_stop();
		return u1_ret;
	}
	//Success
	else
	{
		//Write back accumulator
		gu16_timer_sum = u16_accumulator;
	}
	//Next scan. TOP is the last number counted before reset
	u8_index = AT_TOP_INC( u8_index, Config::NUM_SERVOS );
	//Update the scan index
	this -> gu8_index = u8_index;

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	
	return false;	//OK
}   //end public method: hal_timer_isr

/***************************************************************************/
//! @brief Private method
//! \n emergency_stop | void
/***************************************************************************/
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n Stop driver. Clear IO lines. Reinitialize class vars.
/***************************************************************************/

bool Servo::emergency_stop( void )
{
	DENTER(); //Trace Enter
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Initialize error to OK
	bool u1_ret = false;
	//Stop timer
	u1_ret |= this -> hal_run_isr( false );
	//Clear servo IOs
	u1_ret |= this -> clear_servo_io();
	//Reinitialize class vars (except error)
	u1_ret |= this -> init_class_vars();
	
	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN_ARG("Fail? %d\n", u1_ret); //Trace Return
	return u1_ret;	//Propagate error
}   //end private method: emergency_stop | void

/*********************************************************************************************************************************************************
**********************************************************************************************************************************************************
**	PRIVATE HAL
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

/***************************************************************************/
//! @brief Private HAL
//! \n hal_init_io | void
/***************************************************************************/
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n HAL Driver
//! \n Initialize the IOs connected to the servomotors
/***************************************************************************/

bool Servo::hal_init_io( void )
{
    DENTER(); //Trace Enter
    ///--------------------------------------------------------------------------
    ///	BODY
    ///--------------------------------------------------------------------------

	//PWR pin as OUTPUT
	SET_BIT( PORTD.DIR, Config::IO_PWR_PIN );
	//Set Servo PINS as OUTPUT
	SET_BIT( PORTE.DIR, 0 );
	SET_BIT( PORTE.DIR, 1 );
	SET_BIT( PORTE.DIR, 2 );
	SET_BIT( PORTE.DIR, 3 );
	SET_BIT( PORTF.DIR, 0 );
	SET_BIT( PORTF.DIR, 1 );
	SET_BIT( PORTF.DIR, 2 );
	SET_BIT( PORTF.DIR, 3 );

    ///--------------------------------------------------------------------------
    ///	RETURN
    ///--------------------------------------------------------------------------
    DRETURN(); //Trace Return
    return false;	//OK
}   //end private HAL: hal_init_io

/****************************************************************************
**  @brief private HAL
**  hal_init_ta0_timeout_mode |
****************************************************************************/
//! @details
//!	HAL Driver. Initialize TA0 to launch an ISR when a given time is expired a single time
//!
//!	initialize timer type a as triple 16b PWM generator
//!	setup the only timer type A of the AT4809
//! Emit three high resolution 50Hz PWM signals to control up to three servos on PE0, PE1, PE2
//!
//! F_PWM	= CLK / DIV / (TOP+1)
//!	TOP		= CLK / F_PWM / DIV -1
//!	PWM_RES = log(TOP+1)/log(2)
//!
//!	Frequency = 20MHz
//!	Resolution = 16bit
//!	Prescaler = 1/16
//! Time Resolution = 16/20MHz = 800ns
//! Maximum Delay = 32768*16/20MHz = 26.2ms
//!
//! Interrupt vectors available:
//! TCA0_LUNF_vect
//! TCA0_OVF_vect
//! TCA0_HUNF_vect
//! TCA0_LCMP0_vect
//! TCA0_CMP0_vect
//! TCA0_CMP1_vect
//! TCA0_LCMP1_vect
//! TCA0_LCMP2_vect
//! TCA0_CMP2_vect
/***************************************************************************/

bool Servo::hal_init_ta0_timeout_mode( uint16_t iu16_top )
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//Load temporary registers
	uint8_t ctrla_tmp			= TCA0.SINGLE.CTRLA;
	uint8_t ctrlb_tmp			= TCA0.SINGLE.CTRLB;
	uint8_t ctrlc_tmp			= TCA0.SINGLE.CTRLC;
	uint8_t ctrld_tmp			= TCA0.SINGLE.CTRLD;
	uint8_t ctrle_tmp			= TCA0.SINGLE.CTRLESET;
	uint8_t dbgctrl_tmp			= TCA0.SINGLE.DBGCTRL;
	uint8_t port_mux_tca0_tmp	= PORTMUX.TCAROUTEA;
	uint8_t evctrl_tmp			= TCA0.SINGLE.EVCTRL;
	uint8_t intctrl_tmp			= TCA0.SINGLE.INTCTRL;
	
	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//! Enable Split Mode
	//----------------------------------------------------------------
	//	Function of registers change according to the mode.
	//	0 = 3x 16bit
	//	1 = 6x 8bit

	//SET_BIT( ctrld_tmp, TCA_SINGLE_SPLITM_bp );

	//----------------------------------------------------------------
	//! Enable TCA
	//----------------------------------------------------------------
	//	0 = disabled
	//	1 = enabled

	//SET_BIT( ctrla_tmp, TCA_SINGLE_ENABLE_bp );

	//----------------------------------------------------------------
	//! TCA Clock Prescaler
	//----------------------------------------------------------------
	//	Set the clock prescaler of this TCA. Activate only one value

	//SET_MASKED_BIT( ctrla_tmp, TCA_SINGLE_CLKSEL_gm , TCA_SINGLE_CLKSEL_DIV1_gc );
	//SET_MASKED_BIT( ctrla_tmp, TCA_SINGLE_CLKSEL_gm , TCA_SINGLE_CLKSEL_DIV2_gc );
	//SET_MASKED_BIT( ctrla_tmp, TCA_SINGLE_CLKSEL_gm , TCA_SINGLE_CLKSEL_DIV4_gc );
	//SET_MASKED_BIT( ctrla_tmp, TCA_SINGLE_CLKSEL_gm , TCA_SINGLE_CLKSEL_DIV8_gc );
	SET_MASKED_BIT( ctrla_tmp, TCA_SINGLE_CLKSEL_gm , TCA_SINGLE_CLKSEL_DIV16_gc );
	//SET_MASKED_BIT( ctrla_tmp, TCA_SINGLE_CLKSEL_gm , TCA_SINGLE_CLKSEL_DIV64_gc );
	//SET_MASKED_BIT( ctrla_tmp, TCA_SINGLE_CLKSEL_gm , TCA_SINGLE_CLKSEL_DIV256_gc );
	//SET_MASKED_BIT( ctrla_tmp, TCA_SINGLE_CLKSEL_gm , TCA_SINGLE_CLKSEL_DIV1024_gc );

	//----------------------------------------------------------------
	//! TCA Enable ComPare
	//----------------------------------------------------------------
	//	Enable compare output and waveform output pin override for each compare channel

	//SET_BIT( ctrlb_tmp, TCA_SINGLE_CMP0EN_bp );
	//SET_BIT( ctrlb_tmp, TCA_SINGLE_CMP1EN_bp );
	//SET_BIT( ctrlb_tmp, TCA_SINGLE_CMP2EN_bp );
	
	//----------------------------------------------------------------
	//! TCA Automatically manage LUPD
	//----------------------------------------------------------------
	//	LUPD in CTRLE is automatically managed
	
	//SET_BIT( ctrlb_tmp, TCA_SINGLE_ALUPD_bp );

	//----------------------------------------------------------------
	//! TCA Mode
	//----------------------------------------------------------------
	//	Set mode of operation of the timer
	
	SET_MASKED_BIT( ctrlb_tmp, TCA_SINGLE_WGMODE_gm , TCA_SINGLE_WGMODE_NORMAL_gc );
	//SET_MASKED_BIT( ctrlb_tmp, TCA_SINGLE_WGMODE_gm , TCA_SINGLE_WGMODE_FRQ_gc );
	//SET_MASKED_BIT( ctrlb_tmp, TCA_SINGLE_WGMODE_gm , TCA_SINGLE_WGMODE_SINGLESLOPE_gc );
	//SET_MASKED_BIT( ctrlb_tmp, TCA_SINGLE_WGMODE_gm , TCA_SINGLE_WGMODE_DSTOP_gc );
	//SET_MASKED_BIT( ctrlb_tmp, TCA_SINGLE_WGMODE_gm , TCA_SINGLE_WGMODE_DSBOTH_gc );
	//SET_MASKED_BIT( ctrlb_tmp, TCA_SINGLE_WGMODE_gm , TCA_SINGLE_WGMODE_DSBOTTOM_gc );

	//----------------------------------------------------------------
	//! TCA Event Count Enable
	//----------------------------------------------------------------
	//	Allow counter to count events on the input events

	//SET_BIT( evctrl_tmp, TCA_SINGLE_CNTEI_bp );
	
	//----------------------------------------------------------------
	//! TCA Event Control
	//----------------------------------------------------------------
	//	Select the type of edge the counter is sensitive to
	
	//SET_MASKED_BIT( evctrl_tmp, TCA_SINGLE_EVACT_gm, TCA_SINGLE_EVACT_POSEDGE_gc );
	//SET_MASKED_BIT( evctrl_tmp, TCA_SINGLE_EVACT_gm, TCA_SINGLE_EVACT_ANYEDGE_gc );
	//SET_MASKED_BIT( evctrl_tmp, TCA_SINGLE_EVACT_gm, TCA_SINGLE_EVACT_HIGHLVL_gc );
	//SET_MASKED_BIT( evctrl_tmp, TCA_SINGLE_EVACT_gm, TCA_SINGLE_EVACT_UPDOWN_gc );

	//----------------------------------------------------------------
	//! ENABLE TCA interrupts
	//----------------------------------------------------------------

	//Overflow interrupt
	SET_BIT( intctrl_tmp, TCA_SINGLE_OVF_bp );
	//Compare channel
	//SET_BIT( intctrl_tmp, TCA_SINGLE_CMP0_bp );
	//SET_BIT( intctrl_tmp, TCA_SINGLE_CMP1_bp );
	//SET_BIT( intctrl_tmp, TCA_SINGLE_CMP2_bp );

	//----------------------------------------------------------------
	//! ENABLE TCA debug
	//----------------------------------------------------------------

	SET_BIT( dbgctrl_tmp, TCA_SPLIT_DBGRUN_bp );

	//----------------------------------------------------------------
	//! Set TCA waveform outputs to a given port. You can activate only one port
	//----------------------------------------------------------------

	//SET_MASKED_BIT( port_mux_tca0_tmp, PORTMUX_TCA0_gm, PORTMUX_TCA0_PORTA_gc );	//default
	//SET_MASKED_BIT( port_mux_tca0_tmp, PORTMUX_TCA0_gm, PORTMUX_TCA0_PORTB_gc );
	//SET_MASKED_BIT( port_mux_tca0_tmp, PORTMUX_TCA0_gm, PORTMUX_TCA0_PORTC_gc );
	//SET_MASKED_BIT( port_mux_tca0_tmp, PORTMUX_TCA0_gm, PORTMUX_TCA0_PORTD_gc );
	//SET_MASKED_BIT( port_mux_tca0_tmp, PORTMUX_TCA0_gm, PORTMUX_TCA0_PORTE_gc );
	//SET_MASKED_BIT( port_mux_tca0_tmp, PORTMUX_TCA0_gm, PORTMUX_TCA0_PORTF_gc );

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//! Register write back.
	//Write back control registers
	TCA0.SINGLE.CTRLB = ctrlb_tmp;
	TCA0.SINGLE.CTRLC = ctrlc_tmp;
	TCA0.SINGLE.CTRLD = ctrld_tmp;
	TCA0.SINGLE.CTRLESET = ctrle_tmp;
	TCA0.SINGLE.DBGCTRL = dbgctrl_tmp;
	//Write back output waveform port selector
	PORTMUX.TCAROUTEA = port_mux_tca0_tmp;

	//TOP for all PWM channels
	TCA0.SINGLE.PER = (uint16_t)iu16_top;
	//Initial PWM settings for channels L0 to L2
	TCA0.SINGLE.CMP0 = (uint16_t)0;
	TCA0.SINGLE.CMP1 = (uint16_t)0;
	TCA0.SINGLE.CMP2 = (uint16_t)0;
	//Write back control A for last as it's the one that sets the clock and starts the timer
	TCA0.SINGLE.CTRLA = ctrla_tmp;
	//Write back event control register
	TCA0.SINGLE.EVCTRL = evctrl_tmp;
	//Write back interrupt enable
	TCA0.SINGLE.INTCTRL = intctrl_tmp;

	return false;
}	//End: init_timer_a_pwm16b

/***************************************************************************/
//! @brief Private HAL
//! \n hal_delay_us | void
/***************************************************************************/
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n HAL Driver
//! \n Fixed delay
/***************************************************************************/

bool Servo::hal_delay_us( void )
{
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	_delay_us( Config::SOFT_START_DELAY_US );
	
	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	
	return false;	//OK
}   //end private HAL: hal_delay_us

/***************************************************************************/
//! @brief Private HAL
//! \n hal_delay_us | void
/***************************************************************************/
//! @param iu16_microseconds | delay in microseconds to be programmed inside the timer
//! @return uint16_t | timer units that can be programmed inside the HAL hardware timer | MAX means overflow occurred
//! @details
//! \n HAL Driver
//! \n Convert from a delay in microseconds to a delay in timer counts that can be programmed inside the timer
/***************************************************************************/

uint16_t Servo::hal_microseconds_to_counts( uint16_t iu16_microseconds )
{
	///--------------------------------------------------------------------------
	///	VARS
	///--------------------------------------------------------------------------
	
	//Return timer count
	uint16_t u16_timer_cnt;
	
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//From us to ns
	uint32_t u32_tmp = (uint32_t)1*iu16_microseconds *1000;
	//from ns to counts
	u32_tmp /= Config::HAL_TIMER_RESOLUTION;
	//Detect overflow. Timer is 16b
	if (u32_tmp >= (Config::HAL_TIMER_MAX_CNT -1))
	{
		this -> report_error( Error_code::ERR_OVERFLOW );
		u16_timer_cnt = UINT16_MAX;
	}
	else
	{
		//truncate to 16b
		u16_timer_cnt = u32_tmp;
	}
	
	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	
	return u16_timer_cnt;
}   //end private HAL: hal_delay_us

/***************************************************************************/
//! @brief Private HAL
//! \n hal_timer_set_delay | uint16_t
/***************************************************************************/
//! @param iu16_microseconds | delay in microseconds to be programmed inside the timer
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n Stop the timer. Program in the new delay in microseconds. Restart the timer.
/***************************************************************************/

bool Servo::hal_timer_set_delay( uint16_t iu16_microseconds )
{
	DENTER_ARG("delay us: %d\n", iu16_microseconds); //Trace Enter
	///--------------------------------------------------------------------------
	///	CHECK
	///--------------------------------------------------------------------------
	
	//If: delay is out of range
	if ((Config::SAFETY_CHECK == true) && ( (iu16_microseconds < (Config::SERVO_PPM_ZERO -Config::SERVO_PPM_MAX_COMMAND)) || (iu16_microseconds > Config::SERVO_PPM_PERIOD) ))
	{
		this -> report_error( Error_code::ERR_INPUT_OOB );
		return true;
	}
	
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Stop the timer
	CLEAR_BIT( TCA0.SINGLE.CTRLA, TCA_SINGLE_ENABLE_bp );

	//Fetch previous count
	uint16_t u16_previous_overcount = TCA0.SINGLE.CNT;
	//Convert from microseconds to timer counts
	uint16_t u16_timer_count = this->hal_microseconds_to_counts( iu16_microseconds );
	if ((u16_timer_count == 0) || (u16_timer_count >= Config::HAL_TIMER_MAX_CNT))
	{
		this -> report_error( Error_code::ERR_OVERFLOW );
		//FAIL and keep the timer stopped
		return true;
	}
	//If leftover from previous delay is already more than the programmed delay
	else if (u16_previous_overcount >= u16_timer_count)
	{
		this->report_error(Error_code::ERR_DELAY_OVERRUN);
		//FAIL and keep the timer stopped
		return true;
	}
	//All good
	else
	{
		//Program the new top
		TCA0.SINGLE.PER = u16_timer_count -1;
		//Start the clock
		SET_BIT( TCA0.SINGLE.CTRLA, TCA_SINGLE_ENABLE_bp );	
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN_ARG("delay counts: %d\n", ); //Trace Return
	return false;	//OK
}   //end Private HAL: hal_timer_set_delay | uint16_t

/***************************************************************************/
//! @brief Private HAL
//! \n hal_power | void
/***************************************************************************/
//! @param iu8_powered | bool | false = disconnected | true = powered |
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n HAL Driver
//! \n Set power pin of the SERVO banks. false = disconnected | true = powered
/***************************************************************************/

bool Servo::hal_power( bool iu8_powered )
{
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------


	//Servo disconnected
	if (iu8_powered == false)
	{
		SET_BIT( SERVO_PWR_PORT.OUT, SERVO_PWR_PIN );
	}
	//Servo powered
	else
	{
		CLEAR_BIT( SERVO_PWR_PORT.OUT, SERVO_PWR_PIN );	
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	
	return false;	//OK
}   //end private HAL: hal_power

/***************************************************************************/
//! @brief Private Method
//! \n hal_run_isr | void
/***************************************************************************/
//! @param iu1_running | bool | false = stopped | true = running |
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n Ask the ISR of TA0 to run or stop
/***************************************************************************/

bool Servo::hal_run_isr( bool iu1_running )
{
	DENTER();		//Trace Enter
	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Driver set to STOP
	if (iu1_running == false)
	{
		CLEAR_BIT( TCA0.SINGLE.CTRLA, TCA_SINGLE_ENABLE_bp );
		
	}
	
	else
	{
		TCA0.SINGLE.PER = 24999;
		SET_BIT( TCA0.SINGLE.CTRLA, TCA_SINGLE_ENABLE_bp );
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN();      //Trace Return
	return false;   //OK
}   //end method: hal_run_isr | void

/*********************************************************************************************************************************************************
**********************************************************************************************************************************************************
**	PRIVATE INIT
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

/***************************************************************************/
//! @brief Private Method
//! \n init_class_vars | void
/***************************************************************************/
// @param
//! @return no return
//! @details
//! \n Initialize class vars
/***************************************************************************/

bool Servo::init_class_vars( void )
{
    DENTER();		//Trace Enter
    ///--------------------------------------------------------------------------
    ///	BODY
    ///--------------------------------------------------------------------------

	//Initialize scan
	this -> gu8_index = 0;
	//Initialize accumulator
	this -> gu16_timer_sum = 0;
	
	///--------------------------------------------------------------------------
	///	USER SERVO COMMAND
	///--------------------------------------------------------------------------
	
	//Defaults
	Command st_default_command;
	//Deviation from zero position in microseconds
	st_default_command.s16_position = 0;
	//Max speed in microseconds per second. 0 means speed limit disabled
	st_default_command.u16_speed = 0;
	//Inform the driver that command has changed, and that targets need to be recalculated
	st_default_command.u1_position_changed = true;
	st_default_command.u1_speed_changed = true;
	//Driver will take care of informing user of lock and idle
	st_default_command.u1_idle = false;
	st_default_command.u1_lock = false;
	//Scan all servos
	for (uint8_t u8_cnt = 0; u8_cnt < Config::NUM_SERVOS; u8_cnt++)
	{
		//Initialize to default
		this -> gast_command[u8_cnt] = st_default_command;
	}
	
	///--------------------------------------------------------------------------
	///	DRIVER SERVO STATUS
	///--------------------------------------------------------------------------
	
	//Defaults
	Timer st_default_timer;
	st_default_timer.u16_target = 0;
	st_default_timer.u8_speed = 0;
	st_default_timer.u16_actual = Config::SERVO_PPM_ZERO;
	//Scan all servos
	for (uint8_t u8_cnt = 0; u8_cnt < Config::NUM_SERVOS; u8_cnt++)	
	{
		//Initialize to default
		this -> gast_timer[u8_cnt] = st_default_timer;
	}

    ///--------------------------------------------------------------------------
    ///	RETURN
    ///--------------------------------------------------------------------------
    DRETURN();      //Trace Return
    return false;   //OK
}   //end method: init_class_vars | void

/*********************************************************************************************************************************************************
**********************************************************************************************************************************************************
**	PRIVATE METHODS
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

/***************************************************************************/
//! @brief Private method
//! \n report_error | void |
/***************************************************************************/
//! @return return false = OK | true = fail
//! @details
//! \n Report an error.
/***************************************************************************/

bool Servo::report_error( Error_code error_code_tmp )
{
	DENTER(); //Trace Enter
	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN();      //Trace Return
	return true;    //FAIL
}   //end private method: report_error | void |

/***************************************************************************/
//! @brief Private method
//! \n error_recovery | void |
/***************************************************************************/
//! @return return false = OK | true = fail
//! @details
//! \n Tries to recover from an error
//! \n Automatically called by get_error. 
/***************************************************************************/

bool Servo::error_recovery( void )
{
    DENTER(); //Trace Enter
    ///--------------------------------------------------------------------------
    ///	INIT
    ///--------------------------------------------------------------------------

    ///--------------------------------------------------------------------------
    ///	BODY
    ///--------------------------------------------------------------------------

    ///--------------------------------------------------------------------------
    ///	RETURN
    ///--------------------------------------------------------------------------
    DRETURN();      //Trace Return
    return true;    //FAIL
}   //end private method: error_recovery | void |

/***************************************************************************/
//!	@brief private method
//!	\n soft_start | uint8_t | uint8_t | uint8_t | uint8_t
/***************************************************************************/
//! @param iu8_ton | uint8_t | initial number of ON delays
//! @param iu8_toff | uint8_t | initial number of OFF delays
//! @param iu8_treduce | uint8_t | each cycle increase ON and decrease OFF by this number of delays
//! @param iu8_repeat | uint8_t | repeat ON OFF this number of repeats
//! @return bool | false = OK | true = FAIL |
//! @details
//!	\n	Blocking hardwired version of the soft start method using delay and direct control of IOs
//!	\n	@TODO: I can use TCA0 to emit pulses to EVOUTD in PWM mode for the soft start
/***************************************************************************/

bool Servo::soft_start( uint8_t iu8_ton, uint8_t iu8_toff, uint8_t iu8_treduce, uint8_t iu8_repeat )
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	bool u1_continue = true;

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//While not done
	while (u1_continue == true)
	{
		//if: there is room to reduce Toff
		if (iu8_toff > iu8_treduce)
		{
			//For each repeat
			for (uint8_t u8_tmp = 0; u8_tmp < iu8_repeat; u8_tmp++)
			{
				//Power on
				CLEAR_BIT( SERVO_PWR_PORT.OUT, SERVO_PWR_PIN );
				
				for (uint8_t u8_tmp_b = 0; u8_tmp_b < iu8_ton; u8_tmp_b++)
				{
					this->hal_delay_us();
				}
				
				//Power Off
				this -> hal_power( false );
				
				//Wait
				for (uint8_t u8_tmp_b = 0; u8_tmp_b < iu8_toff; u8_tmp_b++)
				{
					this->hal_delay_us();
				}
			}
			
			//Increase Ton
			iu8_ton = iu8_ton+iu8_treduce;
			//Reduce Toff
			iu8_toff = iu8_toff-iu8_treduce;
		}
		//if: Reached Toff = 0
		else
		{
			//Power on
			this -> hal_power( true );
			//Break cycle
			u1_continue = false;
		}
	}

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------
	
	return false;	//OK
}	//End private method: soft_start | uint8_t | uint8_t | uint8_t | uint8_t

/***************************************************************************/
//! @brief Private method
//! \n clear_servo_io | void
/***************************************************************************/
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n Clear all servo IO lines
/***************************************************************************/

bool Servo::clear_servo_io( void )
{
	DENTER(); //Trace Enter
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Clear error
	bool u1_ret = false;
	//For all servos
	for (uint8_t u8_cnt = 0; u8_cnt < Config::NUM_SERVOS; u8_cnt++)
	{
		//Try and clear that servo IO and accumulate error
		u1_ret |= this -> hal_servo_io( u8_cnt, false );
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN_ARG("fail? %d\n", u1_ret); //Trace Return
	return u1_ret;	//Propagate error
}   //end private method: clear_servo_io | void



/***************************************************************************/
//! @brief Private method
//! \n compute_servo_delay | uint8_t | uint16_t |
/***************************************************************************/
//! @param iu8_index | index of the servo being updated
//! @param ou16_delay | returns computed delay, saves driver effort of fetching result from class var
//! @return bool | bool | false = OK | true = FAIL |
//! @details
//! \n The driver uses command position and speed, and actual delay, to compute next delay
/***************************************************************************/

bool Servo::compute_servo_delay( uint8_t iu8_index, uint16_t &ou16_delay )
{
	DENTER_ARG("Servo:%d\n"); //Trace Enter
	///--------------------------------------------------------------------------
	///	CHECK
	///--------------------------------------------------------------------------

	//If Servo index is OOB
	if ((Config::PEDANTIC_CHECKS == true) && ((iu8_index < 0) || (iu8_index >= Config::NUM_SERVOS)))
	{
		this -> report_error( Error_code::ERR_INPUT_OOB );
		ou16_delay = Config::HAL_TIMER_MAX_CNT;
		return true;
	}
	
	///--------------------------------------------------------------------------
	///	COMMAND -> STATUS
	///--------------------------------------------------------------------------
	//	The driver check if command has changed and saves the vars inside the timer status vars

	//If command position changed
	if (gast_command[iu8_index].u1_position_changed == true)
	{
		//Fetch position
		int16_t s16_tmp = gast_command[iu8_index].s16_position;
		if ((Config::PEDANTIC_CHECKS == true) && ( (s16_tmp < -(int16_t)Config::SERVO_PPM_MAX_COMMAND) || (s16_tmp > (int16_t)Config::SERVO_PPM_MAX_COMMAND) ))
		{
			//Algorithmic error. The class should make sure that user can't write bollocks inside the position
			this -> report_error( Error_code::ERR_ALGORITHM );
			return true;
		}
		//The target delay is the delay to achieve zero position plus the deviation
		gast_timer[iu8_index].u16_target = Config::SERVO_PPM_ZERO +s16_tmp;
		//status and command are synced
		gast_command[iu8_index].u1_position_changed = false;
	}

	//Latch speed limit in microseconds per 20ms
	uint8_t u8_speed = 0;
	//If command speed changed
	if (gast_command[iu8_index].u1_speed_changed == true)
	{
		//Fetch position
		uint16_t u16_tmp = gast_command[iu8_index].u16_speed;
		//If speed limiter is disabled
		if (u16_tmp == 0)
		{
			//Update internal timer status
			gast_timer[iu8_index].u8_speed = 0;	
		}
		//if speed limiter is active
		else
		{
			//The target speed is the speed multiplied by the number of PPM cycles in a second
			u16_tmp = u16_tmp *(1000000 / Config::SERVO_PPM_PERIOD);
			if (u16_tmp > 255)
			{
				report_error(Error_code::ERR_OVERFLOW);
				return true;
			}
			u8_speed = u16_tmp;
			//Update internal timer status
			gast_timer[iu8_index].u8_speed = u8_speed;
		}
		//status and command are synced
		gast_command[iu8_index].u1_speed_changed = false;
	}
	//If speed has not changed
	else
	{
		//Fetch speed
		u8_speed = gast_timer[iu8_index].u8_speed;
	}

	///--------------------------------------------------------------------------
	///	STATUS -> COUNT
	///--------------------------------------------------------------------------

	//If: speed limit is inactive
	if (u8_speed == 0)
	{
		//Fetch target
		uint16_t u16_target = gast_timer[iu8_index].u16_target;
		//Fetch actual
		uint16_t u16_actual = gast_timer[iu8_index].u16_actual;
		//if already locked
		if (u16_actual == u16_actual)
		{
			//Already locked
			gast_command[iu8_index].u1_idle = true;
			gast_command[iu8_index].u1_lock = true;	
		}
		//Needs to move
		else
		{
			//Not locked
			gast_command[iu8_index].u1_idle = false;
			gast_command[iu8_index].u1_lock = false;
			//Update actual
			gast_timer[iu8_index].u16_actual = u16_target;
		}
		//Return to caller the updated delay
		ou16_delay = u16_target;
	}
	//if: speed limit active
	else
	{
		report_error( Error_code::ERR_ALGORITHM );
		return true;
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN_ARG("Delay %d\n", ou16_delay); //Trace Return
	return false;	//OK
}   //end private method: compute_servo_delay | uint8_t | uint16_t |

/***************************************************************************/
//! @brief Private method
//! \n dummy | void
/***************************************************************************/
// @param
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n Method
/***************************************************************************/

bool Servo::dummy( void )
{
    DENTER(); //Trace Enter
    ///--------------------------------------------------------------------------
    ///	INIT
    ///--------------------------------------------------------------------------

    ///--------------------------------------------------------------------------
    ///	BODY
    ///--------------------------------------------------------------------------

    ///--------------------------------------------------------------------------
    ///	RETURN
    ///--------------------------------------------------------------------------
    DRETURN(); //Trace Return
    return false;	//OK
}   //end private method:

#else
	#warning "SERVO_H_ implementation inibithed"
#endif // USER_INIBITH_IMPLEMENTATION
/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

} //End Namespace: OrangeBot

#else
    #warning "Multiple inclusion of hader file SERVO_H_"
#endif
