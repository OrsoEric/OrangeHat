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
**
****************************************************************************/

/****************************************************************************
**	DESCRIPTION
*****************************************************************************
**
****************************************************************************/

/****************************************************************************
**	KNOWN BUG
*****************************************************************************
**
****************************************************************************/

/****************************************************************************
**	INCLUDES
****************************************************************************/

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
#include "uart.h"

/****************************************************************************
**	NAMESPACES
****************************************************************************/

namespace User
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
//!	Uart | void
/***************************************************************************/
// @param
//! @return no return
//!	@details
//! Empty constructor
/***************************************************************************/

Uart::Uart( void )
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

	//Initialize class
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
//!	Uart | void
/***************************************************************************/
// @param
//! @return no return
//!	@details
//! Empty destructor
/***************************************************************************/

Uart::~Uart( void )
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

/***************************************************************************/
//!	@brief public setter
//!	send | uint8_t |
/***************************************************************************/
//! @param iu8_data | data to be sent
//! @return bool | false = OK | true = FAIL |
//!	@details
//! \n	Load a data inside the UART TX buffer
/***************************************************************************/

bool Uart::send( uint8_t iu8_data )
{
	DENTER();	//Trace Enter
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//UART DEMO
	AT_BUF_PUSH( this -> rpi_tx_buf, iu8_data );

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN();	//Trace return
	return false;	//OK
}	//end public setter: send | uint8_t |

/****************************************************************************
*****************************************************************************
**	PUBLIC GETTERS
*****************************************************************************
****************************************************************************/

/***************************************************************************/
//!	@brief public getter
//!	get_counter_tx | uint16_t & |
/***************************************************************************/
//! @param oru16_cnt | transmitted bytes
//! @return bool | false = OK | true = FAIL |
//!	@details
//! \n	get the number of transmitted bytes
/***************************************************************************/

bool Uart::get_counter_tx( uint16_t &oru16_cnt )
{
	DENTER();	//Trace Enter
	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	
	uint16_t u16_cnt = this -> gu16_tx_cnt;
	oru16_cnt = u16_cnt;
	DRETURN_ARG("TX: %d\n", u16_cnt );	//Trace return
	return false;	//OK
}	//end public getter: get_counter_tx | uint16_t & |

/***************************************************************************/
//!	@brief public getter
//!	get_counter_rx | uint16_t & |
/***************************************************************************/
//! @param oru16_cnt | transmitted bytes
//! @return bool | false = OK | true = FAIL |
//!	@details
//! \n	get the number of transmitted bytes
/***************************************************************************/

bool Uart::get_counter_rx( uint16_t &oru16_cnt )
{
	DENTER();	//Trace Enter
	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	
	uint16_t u16_cnt = this -> gu16_rx_cnt;
	oru16_cnt = u16_cnt;
	DRETURN_ARG("RX: %d\n", u16_cnt );	//Trace return
	return false;	//OK
}	//end public getter: get_counter_rx | uint16_t & |

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

/***************************************************************************/
//!	@brief Public Method
//!	isr_rx_exe | void |
/***************************************************************************/
// @param
//! @return bool | false = OK | true = FAIL |
//!	@details
//! Method
/***************************************************************************/

bool Uart::isr_rx_exe( void )
{
	DENTER();	//Trace Enter
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Fetch the data and clear the interrupt flag
	uint8_t rx_data_tmp = USART0.RXDATAL;
	//If: RX buffer overflow
	if (AT_BUF_NUMELEM( this -> rpi_rx_buf )>=(Config::RX_BUFFER_SIZE-1))
	{
		//TODO: This is an error
		AT_BUF_FLUSH( this -> rpi_rx_buf );
	}
	//if: there is space in the RX buffer
	else
	{
		//Push byte into RX buffer for processing
		AT_BUF_PUSH( this -> rpi_rx_buf, rx_data_tmp );
		//Profile
		this -> gu16_rx_cnt++;
	}

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN();	//Trace Return
	return false;	//OK
}	//end public method: isr_rx_exe | void |

/***************************************************************************/
//!	@brief public method
//!	update | void |
/***************************************************************************/
//! @return bool | false = OK | true = FAIL |
//!	@details
//! Background uart driver method. Meant to be executed periodically by the user
/***************************************************************************/

bool Uart::update( void )
{
	DENTER();	//Trace Enter
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//if: RPI TX buffer is not empty and the RPI TX HW buffer is ready to transmit
	if ( (AT_BUF_NUMELEM( this -> rpi_tx_buf ) > 0) && (IS_BIT_ONE(USART0.STATUS, USART_DREIF_bp)))
	{
		//Get the byte to be filtered out
		uint8_t tx_tmp = AT_BUF_PEEK( this -> rpi_tx_buf );
		AT_BUF_KICK( this -> rpi_tx_buf );
		//Send data through the UART
		USART0.TXDATAL = tx_tmp;
		//Profile
		this -> gu16_tx_cnt++;
	}	//End If: RPI TX

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN();	//Trace Return
	return false;	//OK
}	//end : update | void |

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
// @param
//! @return no return
//!	@details
//! Method
/***************************************************************************/

bool Uart::init( void )
{
	
	DENTER();	//Trace Enter
	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Reset profilers
	this -> gu16_rx_cnt = 0;
	this -> gu16_tx_cnt = 0;
	//attach vector to buffer
	AT_BUF_ATTACH( this -> rpi_rx_buf, this -> v0, Config::RX_BUFFER_SIZE);
	AT_BUF_FLUSH( this -> rpi_rx_buf );
	//attach vector to buffer
	AT_BUF_ATTACH( this -> rpi_tx_buf, this -> v1, Config::TX_BUFFER_SIZE);
	AT_BUF_FLUSH( this -> rpi_tx_buf );
	//Activate USART0 @256Kb/s
	this->init_uart( USART0, 313 );

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------
	DRETURN();	//Trace Return
	return false;	//OK
}	//end method: init | void

/****************************************************************************
**  Function
**  init_uart | USART_t& | uint16_t |
****************************************************************************/
//! @brief Initialize one of four USART transceivers
//! @details
//!		Interrupt Vectors
//!	USART0_RXC_vect
//!	USART0_DRE_vect
//!	USART0_TXC_vect
//!	USART1_RXC_vect
//!	USART1_DRE_vect
//!	USART1_TXC_vect
//!	USART2_RXC_vect
//!	USART2_DRE_vect
//!	USART2_TXC_vect
//!	USART3_RXC_vect
//!	USART3_DRE_vect
//!	USART3_TXC_vect
//!
//!		| Normal Mode	| Fast Mode	| Sync Mode
//! ---------------------------------------------
//!	S	| 16			| 8			| 2
//!	---------------------------------------------
//!		Communication speed of the UART
//!	Speed [Hz] = 64 *clk [Hz] / S / BAUD
//!		Computation of the baud rate register
//! BAUD = 64 *clk [Hz] / S / Speed [Hz]
//!
//! Baud Rate table. CLK_PER is set from the main prescaler which minimum value is 2
//!	Speed [Hz]	||	BAUD	| CLK_PER [Hz]	| Mode		| Actual Speed [Hz]
//!	------------------------------------------------------------------------
//!	250.0K		||	320		| 20MHz			| Normal	| 250.0KHz
//!	250.0K		||	640		| 20MHz			| Fast		| 250.0KHz
//!	256.0K		||	313		| 20MHz			| Normal	| 255.6KHz <<<
//!	256.0K		||	625		| 20MHz			| Fast		| 265.0KHz
//!	------------------------------------------------------------------------
/***************************************************************************/

bool Uart::init_uart( USART_t &ist_uart, uint16_t iu16_baud )
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//! Fetch registers
	uint8_t ctrl_a		= ist_uart.CTRLA;
	uint8_t ctrl_b		= ist_uart.CTRLB;
	uint8_t ctrl_c		= ist_uart.CTRLC;
	uint8_t dbgctrl_tmp	= ist_uart.DBGCTRL;
	uint8_t ctrl_ev		= ist_uart.EVCTRL;
	
	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	
	//!Generic configuration bits
	//Enable Loop back mode (internally connect RXI with TXO
	//SET_BIT( ctrl_a, USART_LBME_bp );
	//Multi processor mode: The receiver will wait for a frame containing an address
	//SET_BIT( ctrl_b, USART_MPCM_bp );
	//TXO will work in open drain mode, requiring a pull up resistor but allowing multiple transmitters on the TXO line
	//SET_BIT( ctrl_b, USART_ODME_bp );
	//Start mode enable: A UART start bit will wake the device up
	//SET_BIT( ctrl_b, USART_SFDEN_bp );
	//Enable the transmitter
	SET_BIT( ctrl_b, USART_TXEN_bp );
	//Enable the receiver
	SET_BIT( ctrl_b, USART_RXEN_bp );
	//When enabled, the transmitter will send the configured number of stop bits. Otherwise the transmitter send a single stop bit.
	SET_BIT( ctrl_c, USART_SBMODE_bp );
	//UART will keep running when in debug
	//SET_BIT( dbgctrl_tmp, USART_DBGRUN_bp);
	
	//! UART operation mode
	//Asynchronous UART Mode
	SET_MASKED_BIT( ctrl_c, USART_CMODE_gm, USART_CMODE_ASYNCHRONOUS_gc );
	//Synchronous UART Mode
	//SET_MASKED_BIT( ctrl_c, USART_CMODE_gm, USART_CMODE_SYNCHRONOUS_gc );
	//Infrared Communication UART Mode
	//SET_MASKED_BIT( ctrl_c, USART_CMODE_gm, USART_CMODE_IRCOM_gc );
	//Master SPI Mode
	//SET_MASKED_BIT( ctrl_c, USART_CMODE_gm, USART_CMODE_MSPI_gc );
	
	//! 485 Mode
	//Disable 485 Mode
	SET_MASKED_BIT( ctrl_a, USART_RS485_gm, USART_RS485_OFF_gc );
	//External 485 mode: Transmit Enable pin will activate external driver upon TX
	//SET_MASKED_BIT( ctrl_a, USART_RS485_gm, USART_RS485_EXT_gc );
	//Internal 485 mode
	//SET_MASKED_BIT( ctrl_a, USART_RS485_gm, USART_RS485_INT_gc );
	
	//!	RX Mode
	//Normal mode
	SET_MASKED_BIT( ctrl_b, USART_RXMODE_gm, USART_RXMODE_NORMAL_gc );
	//Double Speed mode
	//SET_MASKED_BIT( ctrl_b, USART_RXMODE_gm, USART_RXMODE_CLK2X_gc );
	//Asynchronous Slave mode: A sync character will be used to automatically set baud rate
	//SET_MASKED_BIT( ctrl_b, USART_RXMODE_gm, USART_RXMODE_GENAUTO_gc );
	//Asynchronous Slave mode: A sync character will be used to automatically set baud rate. Special rules allow validation of sync character
	//SET_MASKED_BIT( ctrl_b, USART_RXMODE_gm, USART_RXMODE_LINAUTO_gc );
	
	//! Word Size
	//5 bit
	//SET_MASKED_BIT( ctrl_c, USART_CHSIZE_gm, USART_CHSIZE_5BIT_gc );
	//6 bit
	//SET_MASKED_BIT( ctrl_c, USART_CHSIZE_gm, USART_CHSIZE_6BIT_gc );
	//7 bit
	//SET_MASKED_BIT( ctrl_c, USART_CHSIZE_gm, USART_CHSIZE_7BIT_gc );
	//8 bit
	SET_MASKED_BIT( ctrl_c, USART_CHSIZE_gm, USART_CHSIZE_8BIT_gc );
	//9 bit, low byte first
	//SET_MASKED_BIT( ctrl_c, USART_CHSIZE_gm, USART_CHSIZE_9BITL_gc );
	//9 bit, high byte first
	//SET_MASKED_BIT( ctrl_c, USART_CHSIZE_gm, USART_CHSIZE_9BITH_gc );
	
	
	//! Parity Mode
	//No parity bit
	SET_MASKED_BIT( ctrl_c, USART_PMODE_gm, USART_PMODE_DISABLED_gc );
	//Parity bit is automatically computed and sent in each frame and checked by the receiver. Even parity
	//SET_MASKED_BIT( ctrl_c, USART_PMODE_gm, USART_PMODE_EVEN_gc );
	//Parity bit is automatically computed and sent in each frame and checked by the receiver. Odd parity
	//SET_MASKED_BIT( ctrl_c, USART_PMODE_gm, USART_PMODE_ODD_gc );

	//! Master SPI Mode
	//In master SPI mode set the data order. Enabled=LSB first
	//SET_BIT( ctrl_c, USART_UDORD_bp );
	//Set the phase sensitivity of the clock
	//SET_BIT( ctrl_c, USART_UCPHA_bp );
	
	
	//----------------------------------------------------------------
	//	INITERRUPTS
	//----------------------------------------------------------------

	//RX interrupt will be trigger in case of auto baud error detected through ISFIF flag
	//SET_BIT( ctrl_a, USART_ABEIE_bp );
	//Enable Receiver Start Frame interrupt
	//SET_BIT( ctrl_a, USART_RXSIE_bp );
	//Enable Data register empty interrupt
	//SET_BIT( ctrl_a, USART_DREIE_bp );
	//Enable TX Interrupt
	//SET_BIT( ctrl_a, USART_TXCIE_bp );
	//Enable RX Interrupt
	SET_BIT( ctrl_a, USART_RXCIE_bp );
	//Enable Infrared Interrupt
	//SET_BIT( ctrl_ev, USART_IREI_bp );
	

	//----------------------------------------------------------------
	//	WRITE BACK
	//----------------------------------------------------------------
	
	//! Baud rate register
	//Set the baud rate of the peripheral
	ist_uart.BAUD = iu16_baud;
	
	//! Infrared mode
	//Infrared transmitter pulse length
	ist_uart.TXPLCTRL = 0;
	//Infrared receiver pulse length
	ist_uart.RXPLCTRL = 0;
	
	//Write back configuration registers
	ist_uart.DBGCTRL = dbgctrl_tmp;
	ist_uart.CTRLC = ctrl_c;
	//Writing back this register enables interrupts
	ist_uart.EVCTRL = ctrl_ev;
	ist_uart.CTRLA = ctrl_a;
	//Writing back his register will enables the UART
	ist_uart.CTRLB = ctrl_b;

	return false; //OK
}	//End: init_uart

/***************************************************************************/
//!	@brief public Method
//!	dummy | void |
/***************************************************************************/
// @param
//! @return bool | false = OK | true = FAIL |
//!	@details
//! Method
/***************************************************************************/

bool Uart::dummy( void )
{
	DENTER();	//Trace Enter
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
	DRETURN();	//Trace Return
	return false;	//OK
}	//end : dummy | void |

/****************************************************************************
**	NAMESPACES
****************************************************************************/

} //End Namespace
