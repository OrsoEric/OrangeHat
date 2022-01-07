/**********************************************************************************
BSD 3-Clause License

Copyright (c) 2021, Orso Eric
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

#ifndef UART_H_
    #define UART_H_

/**********************************************************************************
**  GLOBAL INCLUDES
**********************************************************************************/

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
namespace User
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

/**********************************************************************************
**  PROTOTYPE: CLASS
**********************************************************************************/

/************************************************************************************/
//! @class 		Uart
/************************************************************************************/
//!	@author		Orso Eric
//! @date		2022-01
//! @brief		Uart Library
//! @details
//! \n	Encapsulates the code to initialize the UART with circular buffers and interrupt
//!	\n	to do it properly, the class should be able to link the instance of the correct ISR, look into it
//! @copyright	BSD 3-Clause License Copyright (c) 2020, Orso Eric
//! @todo		todo list
/************************************************************************************/

class Uart
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
				//Compile configuration
            //Check external inputs. After debug, keep this active if the class interfaces with user data that can be malicious.
            EXTERNAL_CHECKS = true,
            //Check internal inputs and status variables. After debug and stable release, this can be safely turned off as the code should be correct.
            INTERNAL_CHECKS = true,
            			
            //BUFFER
            RX_BUFFER_SIZE = 32,
            TX_BUFFER_SIZE = 16,
        } Config;
        
        //! @brief Error codes of the class
        typedef enum _Error_code
        {
            OK,             //OK
            RECOVERY_FAIL,  //error_recovery() failed to recover from an error
        } Error_code;

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  PUBLIC TYPEDEFS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  CONSTRUCTORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Empty Constructor
        Uart( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  DESTRUCTORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Empty destructor
        ~Uart( void );

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

		//Send a byte
		bool send( uint8_t iu8_data );
		//Send a string
		//bool send( uint8_t* iu8_data );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **  PUBLIC GETTERS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

		//! @brief get the number of transmitted bytes
		bool get_counter_tx( uint16_t &oru16_cnt );
		//! @brief get the number of received bytes
		bool get_counter_rx( uint16_t &oru16_cnt );
		
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

		//Method meant to be called by the UART RX ISR
		bool isr_rx_exe( void );
		//Background uart driver method. Meant to be executed periodically by the user
		bool update( void );

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
        **	PRIVATE INIT
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
        //Initialize class vars
        bool init( void );
		//Initialize uart hardware
		bool init_uart( USART_t &ist_uart, uint16_t iu16_baud );
        
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE METHODS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
        //Report an error. return false: OK | true: Unknown error code 
        bool report_error( Error_code error_code_tmp );
        //Tries to recover from an error. Automatically called by get_error. return false = OK | true = fail
        bool error_recovery( void );

        //Uart method to copy the code
        bool dummy( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE VARS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

			///----------------------------------------------------------------------
			///	PROFILING
			///----------------------------------------------------------------------
		
		//! @brief Error code of the class
		Error_code g_error;
		//! @brief Count the number of bytes received by the driver
		uint16_t gu16_rx_cnt;
		//! @brief Count the number of bytes transmitted by the driver
		uint16_t gu16_tx_cnt;

			///----------------------------------------------------------------------
			///	BUFFERS
			///----------------------------------------------------------------------
			//	Buffers structure and data vectors

		//! @brief Safe circular buffer for uart input data
		At_buf8 rpi_rx_buf;
		//! @brief Safe circular buffer for uart tx data
		At_buf8 rpi_tx_buf;
		//! @brief allocate the working vector for the buffer
		uint8_t v0[ Config::RX_BUFFER_SIZE ];
		//! @brief allocate the working vector for the buffer
		uint8_t v1[ Config::TX_BUFFER_SIZE ];

};	//End Class: Uart

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

} //End Namespace

#else
    #warning "Multiple inclusion of hader file UART_H_"
#endif
