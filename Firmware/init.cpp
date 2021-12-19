/****************************************************************************
**	INCLUDES
****************************************************************************/

//type definition using the bit width and signedness
#include <stdint.h>
//define the ISR routune, ISR vector, and the sei() cli() function
#include <avr/interrupt.h>
//name all the register and bit
#include <avr/io.h>

//General purpose macros
#include "at_utils.h"
//AT4809 PORT macros definitions
#include "at4809_port.h"

#include "global.h"

/****************************************************************************
**	MACROS
****************************************************************************/

//Configure a protected register in the initialization phase
//CPP: Writing in this register allows operation on IO protected registers for the next 4CLK
#define INIT_CONFIG_PROTECTED( target_register, value_mask, value_position, value )	\
	CCP = CCP_IOREG_gc, (target_register) = ( ((target_register) & ~(value_mask)) | (((value) << (value_position)) & (value_mask))  )

/****************************************************************************
**	FUNCTIONS PROTOTYPES
****************************************************************************/

//Initialize the micro controller fuses
extern void init_fuses( void );
//Initialize clock systems
extern void init_clock( void );
//Initialize pin map
extern void init_pin( void );
//Initialize RTC timer as periodic interrupt
extern void init_rtc( void );
//Initialize timer type A as triple 16b PWM generator
extern void init_timer_a_pwm16b( void );
//setup one of four timers type B of the AT4809 as PWM generator
extern void init_timer_b( TCB_t &timer );
//Initialize one of four USART transceivers
extern void init_uart( USART_t &usart );
//Initialize port multiplexer for alternate functions
extern void init_mux( void );

/****************************************************************************
**	FUNCTIONS DECLARATIONS
****************************************************************************/

/****************************************************************************
**  Function
**  init |
****************************************************************************/
//! @return bool |
//! @brief main init function
//! @details initialize all embedded peripherals and pins.
//!	remember to call the initialization functions in the main init
/***************************************************************************/

void init(void)
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Initialize clock systems
	init_clock();
	
	//initialize pin configuration
	init_pin();
	
	//Initialize RTC timer as Periodic interrupt source: RTC_PIT_vect
	init_rtc();
	
	//Activate interrupts
	sei();
	
	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return;
}	//End: init

/****************************************************************************
**  Function
**  init_clock |
****************************************************************************/
//! @brief initialize clock systems
//! @details setup the clock system multiplexers and the clock output
//! Clock source=internal oscillator 20MHz
//! CLK_PER = 20MHz
/***************************************************************************/

void init_clock( void )
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Main clock switch
	INIT_CONFIG_PROTECTED( CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_gm, CLKCTRL_CLKSEL_gp, (CLKCTRL_CLKSEL_t)CLKCTRL_CLKSEL_OSC20M_gc);
	//Configure CLK_OUT pin as disabled
	INIT_CONFIG_PROTECTED( CLKCTRL.MCLKCTRLA, CLKCTRL_CLKOUT_bm, CLKCTRL_CLKOUT_bp, 0);
	//Disable the main clock prescaler
	INIT_CONFIG_PROTECTED( CLKCTRL.MCLKCTRLB, CLKCTRL_PEN_bm, CLKCTRL_PEN_bm, 0);
	//Set the main clock prescaler to 2
	INIT_CONFIG_PROTECTED( CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_gm, CLKCTRL_PDIV_gp, (CLKCTRL_PDIV_t)CLKCTRL_PDIV_2X_gc);
	//Disable the clock multiplexer and prescaler protection
	INIT_CONFIG_PROTECTED( CLKCTRL.MCLKLOCK, CLKCTRL_LOCKEN_bm, CLKCTRL_LOCKEN_bp, 0);

	CLKCTRL.OSC20MCTRLA |= CLKCTRL_RUNSTDBY_bm; //1<<1;
	CCP = CCP_IOREG_gc;
	CLKCTRL.OSC20MCALIBB |= CLKCTRL_LOCK_bm; //1<<7

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return;
}	//End: init_clock

/****************************************************************************
**  Function
**  init_pin |
****************************************************************************/
//! @brief initialize pin configuration
//! @details Initialize pin configuration and multiplexers
/***************************************************************************/

void init_pin( void )
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------
	//

	//----------------------------------------------------------------
	//!	PORTA
	//!	PA0				:
	//!	PA1				: 
	//!	PA2, TCB0		: 
	//!	PA3, TCB1		: 
	//!	PA4				: 
	//!	PA5				: 
	//!	PA6				: UC_BTN
	//!	PA7				: LCD PWR#
	//----------------------------------------------------------------
	//				0		1		2		3		4		5		6		7
	PORT_A_CONFIG(	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_R,	PIN_H );

	//----------------------------------------------------------------
	//!	PORTB
	//!	PB0,			: LCD D4
	//!	PB1,			: LCD D5
	//!	PB2				: LCD D6
	//!	PB3				: LCD D7
	//!	PB4,			: LCD EN
	//!	PB5,			: LCD RS
	//!	PB6				:
	//!	PB7				:
	//----------------------------------------------------------------
	//				0		1		2		3		4		5		6		7
	PORT_B_CONFIG(	PIN_L,	PIN_L,	PIN_L,	PIN_L,	PIN_L,	PIN_L,	PIN_Z,	PIN_Z );

	//----------------------------------------------------------------
	//!	PORTC
	//!	PC0				:
	//!	PC1				:
	//!	PC2				:
	//!	PC3				:
	//!	PC4				: UC_LED0
	//!	PC5				: UC_LED1
	//!	PC6				:
	//!	PC7				:
	//----------------------------------------------------------------
	//				0		1		2		3		4		5		6		7
	PORT_C_CONFIG(	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_L,	PIN_L,	PIN_Z,	PIN_Z );

	//----------------------------------------------------------------
	//!	PORTD
	//!	PD0, ADC		: 
	//!	PD1, ADC		: 
	//!	PD2, ADC		: 
	//!	PD3, ADC		: 
	//!	PD4				:
	//!	PD5				:
	//!	PD6				: 
	//!	PD7				:	UC_SERVO_POWER#	| true= allow power to flow from the regulator channel 2 to the servo connectors
	//----------------------------------------------------------------
	//				0		1		2		3		4		5		6		7
	PORT_D_CONFIG(	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z );

	//----------------------------------------------------------------
	//!	PORTE
	//!	PE0				: 
	//!	PE1				: 
	//!	PE2				: 
	//!	PE3				:
	//!	PE4				:
	//!	PE5				:
	//!	PE6				:
	//!	PE7				:
	//----------------------------------------------------------------
	//				0		1		2		3		4		5		6		7
	PORT_E_CONFIG(	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z );

	//----------------------------------------------------------------
	//!	PORTF
	//!	PF0				: 
	//!	PF1				: 
	//!	PF2				:
	//!	PF3				:
	//!	PF4				: 
	//!	PF5				: 
	//!	PF6				:
	//!	PF7				:
	//----------------------------------------------------------------
	//				0		1		2		3		4		5		6		7
	PORT_F_CONFIG(	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z,	PIN_Z );

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return;
}	//End: init_pin

/****************************************************************************
**  Function
**  init_rtc |
****************************************************************************/
//! @brief Initialize RTC timer as periodic interrupt
//! @details
//! Interrupt vectors:
//!		RTC_CNT_vect
//!		RTC_PIT_vect
/***************************************************************************/

void init_rtc( void )
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//! Fetch registers
	uint8_t ctrla_tmp		= RTC.CTRLA;
	uint8_t intctrl_tmp		= RTC.INTCTRL;
	uint8_t dgbctrl_tmp		= RTC.DBGCTRL;
	uint8_t clksel_tmp		= RTC.CLKSEL;
	uint8_t pitctrla_tmp	= RTC.PITCTRLA;
	uint8_t pitintctrl_tmp	= RTC.PITINTCTRL;

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//! Enable the RTC timer
	SET_BIT( ctrla_tmp, RTC_RTCEN_bp );

	//! Enable the RTC Correction
	//SET_BIT( ctrla_tmp, 2 );

	//! Let the RTC timer run in standby mode
	SET_BIT( ctrla_tmp, RTC_RUNSTDBY_bp );

	//! Let the RTC run in debug when CPU is halted
	//SET_BIT( dgbctrl_tmp, RTC_DBGRUN_bp );

	//----------------------------------------------------------------
	//! RTC Clock Source
	//----------------------------------------------------------------
	//	Clock source for the RTC timer. Select only one

	//Internal 32KHz oscillator
	SET_MASKED_BIT( clksel_tmp, RTC_CLKSEL_gm, RTC_CLKSEL_INT32K_gc );
	//SET_MASKED_BIT( clksel_tmp, RTC_CLKSEL_gm, RTC_CLKSEL_INT1K_gc );
	//SET_MASKED_BIT( clksel_tmp, RTC_CLKSEL_gm, RTC_CLKSEL_TOSC32K_gc );
	//SET_MASKED_BIT( clksel_tmp, RTC_CLKSEL_gm, RTC_CLKSEL_EXTCLK_gc );

	//----------------------------------------------------------------
	//! RTC Clock Prescaler
	//----------------------------------------------------------------
	//	Set prescaler. Only activate one

	SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV1_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV2_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV4_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV8_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV16_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV32_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV64_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV128_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV256_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV512_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV1024_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV2048_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV4096_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV8192_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV16384_gc );
	//SET_MASKED_BIT( ctrla_tmp, RTC_PRESCALER_gm, RTC_PRESCALER_DIV32768_gc );

	//----------------------------------------------------------------
	//! RTC Periodic Interrupt period
	//----------------------------------------------------------------

	//! Enable Periodic Interrupt timer
	SET_BIT( pitctrla_tmp, RTC_PITEN_bp );

	//! Period for the periodic interrupt. Activate only one
	//SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_OFF_gc );
	//SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_CYC4_gc );
	//SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_CYC8_gc );
	//SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_CYC16_gc );
	SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_CYC32_gc );
	//SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_CYC64_gc );
	//SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_CYC128_gc );
	//SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_CYC256_gc );
	//SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_CYC512_gc );
	//SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_CYC1024_gc );
	//SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_CYC2048_gc );
	//SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_CYC4096_gc );
	//SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_CYC8192_gc );
	//SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_CYC16384_gc );
	//SET_MASKED_BIT( pitctrla_tmp, RTC_PERIOD_gm, RTC_PERIOD_CYC32768_gc );

	//----------------------------------------------------------------
	//! RTC Interrupts
	//----------------------------------------------------------------

	//! Enable overflow interrupt
	//SET_BIT( intctrl_tmp, RTC_OVF_bp );
	//! Enable Compare Match interrupt
	//SET_BIT( intctrl_tmp, RTC_CMP_bp );
	//! Enable Periodic Interrupt timer
	SET_BIT( pitintctrl_tmp, RTC_PI_bp );

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Calibration PPM of the RTC counter. Meant to be done in software against more precise clock sources.
	RTC.CALIB = (uint8_t)0x00;

	//Wait for the ***
	//while (IS_BIT_ONE(RTC.STATUS, RTC_PERBUSY_bp));
	RTC.PER = (uint16_t)0;
	//Compare register for compare interrupt
	RTC.CMP = (uint16_t)0;

	//! Registers write back
	//Configuration registers
	RTC.DBGCTRL = dgbctrl_tmp;
	RTC.CLKSEL = clksel_tmp;
	RTC.PITCTRLA = pitctrla_tmp;
	//Write this register last as it activates the timer
	RTC.CTRLA = ctrla_tmp;
	//Activate interrupts
	RTC.INTCTRL = intctrl_tmp;
	RTC.PITINTCTRL = pitintctrl_tmp;

	return;
}	//End: init_rtc
