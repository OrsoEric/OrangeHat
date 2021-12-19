#ifndef AT_LCD_H
	//header environment variable, is used to detect multiple inclusion
	//of the same header, and can be used in the c file to detect the
	//included library
	#define AT_LCD_H

	/****************************************************************************
	**	ENVROIMENT VARIABILE
	****************************************************************************/

	//Decide if I'm using the D0-D3 or D4-D7 of the LCD port in 4BIT mode
	//NOTE: Enable only one of the following defines
	//#define LCD_PORT_HIGH
	#define LCD_PORT_LOW

	/****************************************************************************
	**	GLOBAL INCLUDE
	**	TIPS: you can put here the library common to all source file
	****************************************************************************/

	/****************************************************************************
	**	DEFINES
	****************************************************************************/

	    ///----------------------------------------------------------------------
		///	DISPLAY DEFINITIONS
		///----------------------------------------------------------------------

	//Number of physical Columns and Rows of the LCD DISPLAY
	#define LCD_COL				16
	#define LCD_ROW				2
	//Length of the update vector, 1 bit for every display cell (LCD_COL*LCR_ROW/8)
	#define LCD_UPDT			4

	    ///----------------------------------------------------------------------
		///	PIN DEFINITIONS
		///----------------------------------------------------------------------
	
	//POWER
	#define LCD_PWR_PORT		PORTA
	#define LCD_PWR_PIN			7
	//DATA - LCD DATA PORT
	#define LCD_PORT			PORTB
	//EN - LCD Enable, Strobe 010 to execute an operation
	#define EN_PORT				PORTB
	#define EN_PIN				4
	//RS - LCD Register Select
	#define RS_PORT				PORTB
	#define RS_PIN				5
	

	    ///----------------------------------------------------------------------
		///	LCD TIMING DEFINITIONS
		///----------------------------------------------------------------------
		//only used in the hardwired timed function during initialization

	//[uS] Time in which EN will remain high during strobe
	#define EN_STROBE_TIME		200
	//[mS] Time required by the LCD to execute the slowest operation.
	#define LCD_LONG_WAIT		2.0

	    ///----------------------------------------------------------------------
		/// SETTINGS DEFINITIONS
		///----------------------------------------------------------------------

	//Position of the ADJ flag in lcd_cfg_flags
	#define LCD_ADJ_FLAG		0

	enum LCD_SETTINGS
	{
		LCD_ADJ,					//Number Adjust
		LCD_ADJ_R,					//Number Adjust Right eg.				|123    |  1 (DEFAULT)
		LCD_ADJ_L					//Number Adjust Left eg.				|123	|1
	};

	    ///----------------------------------------------------------------------
		///	ERRORS DEFINITIONS
		///----------------------------------------------------------------------

	enum LCD_ERRORS
	{
		LCD_OK 		= 0,			//Default state, everything is fine
		LCD_OOB		= 10,			//Out Of Bound, attempted accesso of a bad memory location
		LCD_WTF		= 99			//WhatTheFuck? Signal an algorithmic error
	};

	/****************************************************************************
	**	MACROS
	****************************************************************************/

	    ///----------------------------------------------------------------------
		///	PIN MACROS
		///----------------------------------------------------------------------

	//IF I'm using bit D4 to D7 of the LCD_PORT to interface with D4 to D7 of the LCD DISPLAY
	#ifdef LCD_PORT_HIGH
		//write the 4 LSB of data into the 4 MSB of the LCD port, leaving the other bit untouched
		#define LCD_WRITE( data ) \
			LCD_PORT = ((LCD_PORT & 0x0f) | ((data & 0x0f) << 4))
	#endif
	
	//IF I'm using bit D0 to D3 of the LCD_PORT to interface with D4 to D7 of the LCD DISPLAY
	#ifdef LCD_PORT_LOW
		//write the 4 MSB of data into the 4 MSB of the LCD port, leaving the other bit untouched
		#define LCD_WRITE( data ) \
			SET_MASKED_BIT( LCD_PORT.OUT, 0x0f, data )
			//LCD_PORT = ((LCD_PORT & 0xf0) | (data & 0x0f))
	#endif

	//Strobe the enable pin, time specify the delay after each edge
	#define STROBE_EN( time )			\
		SET_BIT( EN_PORT.OUT, EN_PIN );		\
		_delay_us( time );				\
		CLEAR_BIT( EN_PORT.OUT, EN_PIN );	\
		_delay_us( time )

    //Calculate the position of a digit in the lcd_show vector
    #define LCD_POS( row, col ) \
        ( (col) + ((row) * LCD_COL))

	    ///----------------------------------------------------------------------
		///	ERROR
		///----------------------------------------------------------------------

	//called when errors are detected
	#define LCD_ERROR( code )	\
		DPRINT( "ERROR: %d\n", (code) ); \
		lcd_error = (code)

	/****************************************************************************
	**	TYPEDEF
	****************************************************************************/

	typedef struct _Lcd_fsm_status Lcd_fsm_status;

	/****************************************************************************
	**	STRUCTURE
	****************************************************************************/

	struct _Lcd_fsm_status
	{
		uint8_t scan	: 5;	//current digit being scanned for update
		uint8_t exe		: 2;	//state of execution
		uint8_t adr		: 1;	//transmitting data/address
		uint8_t h		: 1;	//transmitting MSB/LSB
		uint8_t cursor	: 5;	//current cursor position (LCD display)
		uint8_t			: 2;	//unused bits
	};

	/****************************************************************************
	**	PROTOTYPE: FUNCTION
	****************************************************************************/

        ///----------------------------------------------------------------------
		///	INTERNAL FUNCTIONS
		///----------------------------------------------------------------------

	//Hardwired command send, used in lcd_init
	extern void lcd_send_cmd( uint8_t cmd );
	//FSM that send data, it controls DATA and EN channels, but not RS
	//xtern uint8_t	lcd_write( uint8_t data );

        ///----------------------------------------------------------------------
		///	LCD DRIVER FUNCTIONS
		///----------------------------------------------------------------------

	//Initialize LCD Display
	extern void lcd_init( void );
    //Main driver function. User should call this at a fixed frequency (10KHz)
    //This function will automatically sync the content of lcd_show with the LCD display
	extern void lcd_update( void );
	//This function configure certain behaviors of the print function
	extern void lcd_config( uint8_t cfg, uint8_t val );

	    ///----------------------------------------------------------------------
		///	LCD PRINT FUNCTIONS
		///----------------------------------------------------------------------

    //write a char in the lcd_show vector, update lcd_updt flags
    extern void lcd_print_char( uint8_t pos, uint8_t data );
	//Print a string on screen
	extern void lcd_print_str( uint8_t pos, const char *str );
	//print a uint8_t number on screen
	extern void lcd_print_u8( uint8_t pos, uint8_t num );
	//print a int8_t number on screen
	extern void lcd_print_s8( uint8_t pos, int8_t num );
	//print a uint16_t number on screen
	extern void lcd_print_u16( uint8_t pos, uint16_t num );
	//print a int16_t number on screen
	extern void lcd_print_s16( uint8_t pos, int16_t num );

	/****************************************************************************
	**	PROTOTYPE: GLOBAL VARIABILE
	****************************************************************************/

	//LCD SHOW vector
	//	Print calls will update this vector. Driver will sync this vector with the display itself
	extern uint8_t 	lcd_show[ LCD_ROW * LCD_COL ];
	//LCD CELLS UPDATE
	//	Print will rise the cell bit to '1', signaling the driver that content must be synced
	extern uint8_t	lcd_updt[ LCD_UPDT ];
	//Configuration flags for the print functions
	extern uint8_t	lcd_cfg_flags;
	//current error code of the library, default state is LCD_OK
	extern uint8_t	lcd_error;

#else
	#warning "multiple inclusion of the header file at_lcd.h"
#endif
