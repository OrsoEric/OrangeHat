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
			NUM_SERVOS = 8,
			
			SERVO_PWR_PIN = 7,
			
				//HAL Timer Parameters
			//Prescaler to the main timer clock
			HAL_TIMER_PRESCALER = 16,
			HAL_TIMER_MAX_CNT = UINT16_MAX,
			//Resolution of a single count in nanoseconds per count [ns/cnt]. Needed to convert between time and delay to be programmed inside the hardware timer
			//HAL_TIMER_RESOLUTION = ((1000000000UL /F_CPU) /HAL_TIMER_PRESCALER ), //BUGGED
			HAL_TIMER_RESOLUTION = 800,
			
				//Servomotor PPM Parameters
			//Pulse width to have a zero output in microseconds [us]
			SERVO_PPM_ZERO = 1500,
			//Period of a single servo waveform in microseconds [us]
			SERVO_PPM_PERIOD = 20000,
			//SERVO_PPM_FREQUENCY = 1000000/Config::SERVO_PPM_PERIOD, //BUGGED
			SERVO_PPM_FREQUENCY = 50,
			//Deviation from the deadtime that gives either a full forward or a full backward command in microseconds [us]
			SERVO_PPM_MAX_COMMAND = 400,
			//Minimum and maximum pulse width in microseconds [us]
			SERVO_PPM_MIN_PULSE = SERVO_PPM_ZERO -SERVO_PPM_MAX_COMMAND,
			SERVO_PPM_MAX_PULSE = SERVO_PPM_ZERO +SERVO_PPM_MAX_COMMAND,
				
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
			ERR_ALGORITHM,		//Algorithmic error. Internal vars are inconsistent or a piece of code that shouldn't fail has failed
            RECOVERY_FAIL,  //error_recovery() failed to recover from an error
        } Error_code;

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  PUBLIC TYPEDEFS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
		//! @brief Error Reporter. Error code and line number where error occurred
		typedef struct _Error_report
		{
			Error_code e_error_code;
			uint16_t u16_line_number;
		} Error_report;
		
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
        bool report_error( Error_code error_code_tmp, uint16_t u16_line_number );
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
		Error_report gst_error;
};	//End Class: Servo

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

} //End Namespace: OrangeBot

#else
    #warning "Multiple inclusion of hader file SERVO_H_"
#endif