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

/*********************************************************************************************************************************************************
**********************************************************************************************************************************************************
**	INCLUDES
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

//type definition using the bit width and sign
#include <stdint.h>
//define the ISR routune, ISR vector, and the sei() cli() function
#include <avr/interrupt.h>
//name all the register and bit
#include <avr/io.h>
//General purpose macros
#include "at_utils.h"
//AT4809 PORT macros definitions
#include "at4809_port.h"
//Common definitions
#include "global.h"
//Trace functions
#include "debug.h"
//hard coded delay
#include <util/delay.h>
//Class Header
#include "adc.h"

/*********************************************************************************************************************************************************
**********************************************************************************************************************************************************
**	NAMESPACE
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

//! @namespace custom namespace
namespace User
{


/*********************************************************************************************************************************************************
**********************************************************************************************************************************************************
**	CONSTRUCTORS
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

/***************************************************************************/
//! @brief Constructor
//! \n Adc | void
/***************************************************************************/
// @param
//! @return no return
//! @details
//! \n Empty constructor
/***************************************************************************/

Adc::Adc( void )
{
    DENTER();   //Trace Enter
    ///--------------------------------------------------------------------------
    ///	BODY
    ///--------------------------------------------------------------------------

    //Initialize class vars
    this -> init();

    ///--------------------------------------------------------------------------
    ///	RETURN
    ///--------------------------------------------------------------------------
    DRETURN();  //Trace Return
    return;
}   //end constructor: Adc | void

/*********************************************************************************************************************************************************
**********************************************************************************************************************************************************
**	DESTRUCTORS
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief Destructor
//! \n Adc | void
/***************************************************************************/
// @param
//! @return no return
//! @details
//! \n Empty destructor
/***************************************************************************/

Adc::~Adc( void )
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
}   //end destructor: Adc | void

/*********************************************************************************************************************************************************
**********************************************************************************************************************************************************
**	PUBLIC GETTERS
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

/***************************************************************************/
//! @brief Public getter
//! \n get_result | uint16_t& |
/***************************************************************************/
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n Return conversion result to caller
/***************************************************************************/

bool Adc::get_result( uint16_t &oru16_res )
{
	DENTER(); //Trace Enter

	if (this -> gu1_new_result == true)
	{
		oru16_res = this -> gu16_res;	
		return false; //OK
	}
	else
	{
		oru16_res = 0;
		return true; //OK
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN(); //Trace Return
	return false; //OK
}   //end getter: get_result | uint16_t& |

/***************************************************************************/
//! @brief Public getter
//! \n get_error | void |
/***************************************************************************/
//! @return Error_code
//! @details
//! \n Get current error state of the library
//! \n Try to recover from errors
/***************************************************************************/

inline Adc::Error_code Adc::get_error( void )
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
**	PUBLIC METHODS
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

/***************************************************************************/
//! @brief public method
//! \n init | void |
/***************************************************************************/
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n Initialize peripheral
/***************************************************************************/

bool Adc::init( void )
{
	DENTER(); //Trace Enter
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Initialize to success
	bool u1_fail = false;
	//Initialize class
	u1_fail |= this -> init_class_vars();
	//Initialize voltage reference hardware
	
	//Initialize hardware ADC
	u1_fail |= this -> init_adc( ADC0 );

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN_ARG("Fail: %d", u1_fail); //Trace Return
	return u1_fail;	//Propagate
}   //end public method: init | void |

/***************************************************************************/
//! @brief public method
//! \n isr_call | uint16_t |
/***************************************************************************/
//! @param uint16_t | result of the conversion
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n Method meant to be called by the ADC0 ISR
/***************************************************************************/

bool Adc::isr_call( uint16_t iu16_res )
{
	DENTER(); //Trace Enter
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------


	this -> gu16_res = iu16_res;
	this -> gu1_new_result = true;

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN(); //Trace Return
	return false;	//Propagate
}   //end public method: isr_call | uint16_t |

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

bool Adc::init_class_vars( void )
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
    return false;   //OK
}   //end method: init_class_vars | void

/****************************************************************************
**  @brief Function
**  init_adc | void |
****************************************************************************/
//! @return
//! @details
//!	\n Initialize ADC
//! Interrupt vectors:
//!		RTC_CNT_vect
//!		RTC_PIT_vect
/***************************************************************************/

bool Adc::init_adc( ADC_t &irst_adc )
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//Fetch registers	
	uint8_t u8_ctrla_tmp		= irst_adc.CTRLA;
	uint8_t u8_ctrlb_tmp		= irst_adc.CTRLB;
	uint8_t u8_ctrlc_tmp		= irst_adc.CTRLC;
	uint8_t u8_ctrld_tmp		= irst_adc.CTRLD;
	uint8_t u8_ctrle_tmp		= irst_adc.CTRLE;
	uint8_t u8_ctrl_calib_tmp	= irst_adc.CALIB;
	uint8_t u8_ctrl_samp		= irst_adc.SAMPCTRL;
	uint8_t u8_ctrl_mux			= irst_adc.MUXPOS;
	uint8_t u8_command_tmp		= irst_adc.COMMAND;
	uint8_t u8_ctrl_event_tmp	= irst_adc.EVCTRL;
	uint8_t u8_ctrl_int_tmp		= irst_adc.INTCTRL;
	uint8_t u8_debug_tmp		= irst_adc.DBGCTRL;
	
	/*
	int8_t sigrow_offset = SIGROW.TEMPSENSE1; // Read signed value from signature row
	uint8_t sigrow_gain = SIGROW.TEMPSENSE0; // Read unsigned value from signature row
	uint16_t adc_reading = 0; // ADC conversion result with 1.1 V internal reference
	uint32_t temp = adc_reading - sigrow_offset;
	temp *= sigrow_gain; // Result might overflow 16 bit variable (10bit+8bit)
	temp += 0x80; // Add 1/2 to get correct rounding on division below
	temp >>= 8; // Divide result to get Kelvin
	uint16_t temperature_in_K = temp;
	*/
	
	//----------------------------------------------------------------
	//	ENABLE
	//----------------------------------------------------------------

	//Enable the ADC
	SET_BIT( u8_ctrla_tmp, ADC_ENABLE_bp );
	//Start conversion
	SET_BIT( u8_command_tmp, ADC_STCONV_bp );

	//----------------------------------------------------------------
	//	INTERRUPTS and EVENTS
	//----------------------------------------------------------------
	
	//Allow events to start a conversion
	//SET_BIT( u8_ctrl_event_tmp, ADC_STARTEI_bp );
	//Conversion complete causes an interrupt to occur
	SET_BIT( u8_ctrl_int_tmp, ADC_RESRDY_bp );
	//Conversion complete within window causes interrupt to occur
	//SET_BIT( u8_ctrl_int_tmp, ADC_WCMP_bp );

	//----------------------------------------------------------------
	//	CONFIGURATION
	//----------------------------------------------------------------
	//	Configure the ADC 
	
	//Free running. true = conversions starts immediately after old conversion
	//SET_BIT( u8_ctrla_tmp, ADC_FREERUN_bp );
	
	//true = 8b | false = 10b
	//SET_BIT( u8_ctrla_tmp, ADC_RESSEL_bp);
	
	//true = ADC runs when chip is in standby |
	SET_BIT( u8_ctrla_tmp, ADC_RUNSTBY_bp );
	
	//Set clock prescaler CLD_PER -> CLK_ADC
	//SET_MASKED_BIT( u8_ctrlc_tmp, ADC_PRESC_gm, ADC_PRESC_DIV2_gc );
	//SET_MASKED_BIT( u8_ctrlc_tmp, ADC_PRESC_gm, ADC_PRESC_DIV4_gc );
	//SET_MASKED_BIT( u8_ctrlc_tmp, ADC_PRESC_gm, ADC_PRESC_DIV8_gc );
	//SET_MASKED_BIT( u8_ctrlc_tmp, ADC_PRESC_gm, ADC_PRESC_DIV16_gc );
	//SET_MASKED_BIT( u8_ctrlc_tmp, ADC_PRESC_gm, ADC_PRESC_DIV32_gc );
	//SET_MASKED_BIT( u8_ctrlc_tmp, ADC_PRESC_gm, ADC_PRESC_DIV64_gc );
	SET_MASKED_BIT( u8_ctrlc_tmp, ADC_PRESC_gm, ADC_PRESC_DIV128_gc );
	//SET_MASKED_BIT( u8_ctrlc_tmp, ADC_PRESC_gm, ADC_PRESC_DIV256_gc );
	
	//Select voltage reference
	//SET_MASKED_BIT( u8_ctrlc_tmp, ADC_REFSEL_gm, ADC_REFSEL_INTREF_gc );
	SET_MASKED_BIT( u8_ctrlc_tmp, ADC_REFSEL_gm, ADC_REFSEL_VDDREF_gc );
	//SET_MASKED_BIT( u8_ctrlc_tmp, ADC_REFSEL_gm, ADC_REFSEL_VREFA_gc );
	
	//true = reduce capacitance for reference >= 1V | false = increased capacitance for reference < 1V
	SET_BIT( u8_ctrlc_tmp, ADC_SAMPCAP_bp );
	
	//true = 25% duty cycle when ADC_CLK < 1.5MHz | false = 50% duty cycle when ADC_CLK > 1.5MHz
	SET_BIT( u8_ctrl_calib_tmp, ADC_DUTYCYC_bp );
	
	//Select delay before first sample when OFF->ON, change REF, SLEEP->ON
	//SET_MASKED_BIT( u8_ctrld_tmp, ADC_INITDLY_gm, ADC_INITDLY_DLY0_gc );
	//SET_MASKED_BIT( u8_ctrld_tmp, ADC_INITDLY_gm, ADC_INITDLY_DLY16_gc );
	//SET_MASKED_BIT( u8_ctrld_tmp, ADC_INITDLY_gm, ADC_INITDLY_DLY32_gc );
	//SET_MASKED_BIT( u8_ctrld_tmp, ADC_INITDLY_gm, ADC_INITDLY_DLY64_gc );
	//SET_MASKED_BIT( u8_ctrld_tmp, ADC_INITDLY_gm, ADC_INITDLY_DLY128_gc );
	SET_MASKED_BIT( u8_ctrld_tmp, ADC_INITDLY_gm, ADC_INITDLY_DLY256_gc );
	
	//Default sampling time is 2 ADC cycles. Sampling time can be extended with this register to account for higher impedance sources
	SET_MASKED_BIT( u8_ctrl_samp, ADC_SAMPLEN_gm, 0 );
	
	//Select GND as initial sampling source. mux selection won't take effect until conversion is done
	SET_MASKED_BIT( u8_ctrl_mux, ADC_MUXPOS_gm, ADC_MUXPOS_TEMPSENSE_gc );
	//Allow peripheral to run during break
	SET_BIT( u8_debug_tmp, ADC_DBGRUN_bp );
	
	//----------------------------------------------------------------
	//	ACCUMULATOR
	//----------------------------------------------------------------
	
	//A number of samples are accumulated inside ADC.RES result registers before a complete conversion
	SET_MASKED_BIT( u8_ctrlb_tmp, ADC_SAMPNUM_gm, ADC_SAMPNUM_ACC1_gc );
	//SET_MASKED_BIT( u8_ctrlb_tmp, ADC_SAMPNUM_gm, ADC_SAMPNUM_ACC2_gc );
	//SET_MASKED_BIT( u8_ctrlb_tmp, ADC_SAMPNUM_gm, ADC_SAMPNUM_ACC4_gc );
	//SET_MASKED_BIT( u8_ctrlb_tmp, ADC_SAMPNUM_gm, ADC_SAMPNUM_ACC8_gc );
	//SET_MASKED_BIT( u8_ctrlb_tmp, ADC_SAMPNUM_gm, ADC_SAMPNUM_ACC16_gc );
	//SET_MASKED_BIT( u8_ctrlb_tmp, ADC_SAMPNUM_gm, ADC_SAMPNUM_ACC32_gc );
	//SET_MASKED_BIT( u8_ctrlb_tmp, ADC_SAMPNUM_gm, ADC_SAMPNUM_ACC64_gc );
	
	//----------------------------------------------------------------
	//	RANDOMIZE SAMPLING TIME
	//----------------------------------------------------------------
	
	//true = randomize sampling instant to compensate for periodic noises, counter is 4b CTRLD.SAMPDLY
	//SET_BIT( u8_ctrld_tmp, ADC_ASDV_bp );
	//Randomization counter. Wraps up when increments after 0x0f
	//SET_MASKED_BIT( u8_ctrlb_tmp, ADC_SAMPDLY_gm, 0 );
	
	//----------------------------------------------------------------
	//	WINDOW COMPARATOR
	//----------------------------------------------------------------
	//	define a window. ISR can be called when conversion result is within a certain range of WIN_LT and WINHT
	
	SET_MASKED_BIT( u8_ctrle_tmp, ADC_WINCM_gm, ADC_WINCM_NONE_gc );
	//SET_MASKED_BIT( u8_ctrle_tmp, ADC_WINCM_gm, ADC_WINCM_BELOW_gc );
	//SET_MASKED_BIT( u8_ctrle_tmp, ADC_WINCM_gm, ADC_WINCM_ABOVE_gc );
	//SET_MASKED_BIT( u8_ctrle_tmp, ADC_WINCM_gm, ADC_WINCM_INSIDE_gc );
	//SET_MASKED_BIT( u8_ctrle_tmp, ADC_WINCM_gm, ADC_WINCM_OUTSIDE_gc );
	
	//ADC0.WINLT
	//ADC0.WINHT
	
	
	
	
	
	//----------------------------------------------------------------
	//	WRITE BACK
	//----------------------------------------------------------------
	
	//Write back registers
	irst_adc.CTRLA = u8_ctrla_tmp;
	irst_adc.CTRLB = u8_ctrlb_tmp;
	irst_adc.CTRLC = u8_ctrlc_tmp;
	irst_adc.CTRLD = u8_ctrld_tmp;
	irst_adc.CTRLE = u8_ctrle_tmp;
	irst_adc.CALIB = u8_ctrl_calib_tmp;
	irst_adc.SAMPCTRL = u8_ctrl_samp;
	irst_adc.MUXPOS = u8_ctrl_mux;
	irst_adc.COMMAND = u8_command_tmp;
	irst_adc.EVCTRL = u8_ctrl_event_tmp;
	irst_adc.INTCTRL = u8_ctrl_int_tmp;
	irst_adc.DBGCTRL = u8_debug_tmp;
	
	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------
	return false;	//OK
}	//End: init_adc | void |

/*********************************************************************************************************************************************************
**********************************************************************************************************************************************************
**	PRIVATE METHODS
**********************************************************************************************************************************************************
*********************************************************************************************************************************************************/

/***************************************************************************/
//! @brief Private method
//! \n error_recovery | void |
/***************************************************************************/
//! @return return false = OK | true = fail
//! @details
//! \n Tries to recover from an error
//! \n Automatically called by get_error. 
/***************************************************************************/

bool Adc::error_recovery( void )
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
//! @brief Private method
//! \n dummy | void
/***************************************************************************/
// @param
//! @return bool | false = OK | true = FAIL |
//! @details
//! \n Method
/***************************************************************************/

bool Adc::dummy( void )
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


/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

} //End Namespace: User
