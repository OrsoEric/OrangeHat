/****************************************************************************
**	ORANGEBOT PROJECT
*****************************************************************************
**	Library to make use of lcd display in 4bit or 8bit mode
**	The library is interrupt based to reduce the MPU consumption
*****************************************************************************
**	Author: 			Orso Eric
**	Creation Date:
**	Last Edit Date:
**	Revision:
**	Version:			3.0
****************************************************************************/

/****************************************************************************
**	HYSTORY VERSION
*****************************************************************************
**	V3.0
**	>Added detailed description for the LCD DISPLAY hardware, timing and instructions
**	>Cleaned function names from library V2.0
**	>Now the lcd_init has bit description of the initialization commands for the display
**	To edit the settings you just need to uncomment or comment the right SET_BIT option
**	Much like all my others init function for ATMEL works
**	>Tested the init function in 4BIT_MSB mode (my bodule)
**		pin and timing defines, lcd_send_cmd, lcd_init
**	>Added lcd_error var, keep track of library error status. Default state is LCD_OK
**	>Tested the lcd_print_char. The function now write into lcd_show, and rise a lcd_updt flag
**	>Updated and tested LCD_BUF macros. Now i store bot and top at the end of the data vector
**	>Rewritten and tested lcd_update(), the core function. It works like a charm! The refresh rate is majestic!
**	A char take 12 lcd_update() calls for random access or 6 calls for sequential access
**	>CleanUp: I was able to remove the command queque altogether
****************************************************************************/

/****************************************************************************
**	USED PIN
**	TIPS: compile this field before writing any code: it really helps!
*****************************************************************************
**	>RS (Register Select)
**	>RW (Read/#Write)
**	>E  (Enable)
**  >D4 - D7
****************************************************************************/

/****************************************************************************
**  LCD DISPLAY
*****************************************************************************
**		DATASHEET
**	DISPLAY		: CDM1602K
**	CONTROLLER	: KS0066U
**      PINS
**  >RS (Register Select)
**  '0' |   I'm sending an instruction
**  '1' |   I'm sending a data
**  >RW (Read/#Write)
**  '0' | I'm writing to the LCD
**  '1' | I'm reading from the LCD (This driver will not perform reads)
**  >E  (Enable)
**  Usually '0'
**  Strobe '0'->'1'->'0' will execute an operation
**  >D0 - D7
**  Bidirectional data bus
**  The LCD can operate in 4 bit mode, using only D4-D7 (This driver will only handle 4bit mode)
*****************************************************************************
**      WRITE INSTRUCTIONS (R/#W = '0')
**	Most instruction are fast, they take about 40uS. Slow instruction 1.5mS are marked
**  RS  |	D0	D1	D2	D3	D4	D5	D6	D7	|	Instruction Name and Details
**  --------------------------------------------------------------------------
**	0	|	1	0	0	0	0	0	0	0	|	0) Clean LCD (slow) - reset the content
**	0	|	/	1	0	0	0	0	0	0	|	1) Reset cursor position, Reset display shift (Slow)
**	0	|	S	D	1	0	0	0	0	0	|	2) Set shift, Set cursor direction
**											|	S = '1' -> Shift
**											|	D = '1' -> Increment
**	0	|	B	C	E	1	0	0	0	0	|	3) Enable Show Display, Show Cursor, Cursor Blink
**											|	B = '1' -> Cursor will blink
**											|	C = '1' -> Show Cursor
**											|	E = '1' -> Display ON (content will be shown
**	0	|	/	/	F	D	1	0	0	0	|	4) Cursor working (NOTE: not sure about this command)
**											|	S = '1' -> cursor will shift display | '0' normal operation
**											|	D = '1' -> advance backward
**	0	|	/	/	F	N	W	1	0	0	|	5) Font type, Bus Width
**											|	F = '1'	-> Font 5x10 | '0' -> Font 5x7
**											|	N = '1' -> 2 lines	| '0' -> 1 line
**											|	W = '1' -> 8bit mode | '0' -> 4bit mode D4-D7
**	0	|	A0	A1	A2	A3	A4	A5	1	0	|	6) Set CGRAM address (Character Generator RAM) (Font)
**	0	|	A0	A1	A2	A3	A4	A5	A6	1	|	7) Set DDRAM address (Data Display RAM)
**	1	|	D0	D1	D2	D3	D4	D5	D6	D7	|	Write data to CGRAM or DDRAM
**											|	Decided by the most recent Set Address operation
**											|	Entry mode will define the cursor address after the operation
*****************************************************************************
**		4BIT OPERATION
**	the display will only use its 4 to 7 bit of the data port
**	First send bit 4 to 7 (high), strobe EN, then send bit 0 to 3 (low)
**	Send Command 5 first. Special care has to be taken, since the default mode is 8bit
**	Special care has to be
****************************************************************************/

/****************************************************************************
**	DESCRIPTION
*****************************************************************************
**	>Print functions update lcd_show and lcd_update vectors
**	>The driver will take care of syncing vectors with the LCD DISPLAY itself
**	>The user must call the display_init function to setup the display
**	>The user must call the display_update function at a fixed rate
**	that is the function sync the vectors with the content
**  >I only use 4bit mode
****************************************************************************/

/****************************************************************************
**	KNOWN BUG
*****************************************************************************
**	>BUG Architecture (SOLVED)
**	The version V2 of the library had queuing problems, since print calls
**	directly queued command for the display, if I called too many prints,
**	the buffer would overflow.
**	SOLUTION: Architecture Shift V2 -> V3
**	Print calls update the lcd_show vector which contains what the user wants to display
**	The driver will read from there and send commands to the display at his
**	defined refresh rate. This way user won't have to worry about calling
**	too many prints. The function is also a lot more efficient
**	>BUG Cursor (SOLVED)
**	When I write in 0,15, the cursor moves to 0,16 (out of screen) NOT 1,0
**	SOLUTION: I disable address optimization for position 1,0 only in lcd_update()
****************************************************************************/

/****************************************************************************
**	USED PIN
**	TIPS: compile this field before writing any code: it really helps!
*****************************************************************************
**
****************************************************************************/

/****************************************************************************
**	INCLUDE
****************************************************************************/

#define F_CPU 20000000

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include "at_utils.h"
#include "debug.h"
#include "at_lcd.h"
#include "embedded_string.hpp"

/****************************************************************************
**	GLOBAL VARIABILE
****************************************************************************/

//LCD SHOW vector
//	Print calls will update this vector. Driver will sync this vector with the display itself
uint8_t lcd_show[ LCD_ROW * LCD_COL ];
//LCD CELLS UPDATE
//	Print will rise the cell bit to '1', signaling the driver that content must be synced
uint8_t	lcd_updt[ LCD_UPDT ];
//Configuration flags for the print functions
uint8_t	lcd_cfg_flags;
//current error code of the library, default state is LCD_OK
uint8_t	lcd_error;

/****************************************************************************
**	FUNCTIONS
****************************************************************************/

/****************************************************************************
*****************************************************************************
**	DRIVER GROUP
*****************************************************************************
****************************************************************************/

/****************************************************************************
**	LCD SEND CMD
*****************************************************************************
**	This function send a command to the LCD display. Use hard wired delay
**	Use 4 bit mode only
****************************************************************************/

void lcd_send_cmd( uint8_t cmd )
{
	//***********************************************************************
	//	STATIC VARIABILE
	//***********************************************************************

	//***********************************************************************
	//	LOCAL VARIABILE
	//***********************************************************************

	//***********************************************************************
	//	BODY
	//***********************************************************************
	//	This routine is used to send a command. use hard wired delay. Used only during init

	//write 4 bit of command to the LCD data port, define decide if low or high bits are written
	//macro will always use the 4 LSB of the input
	//Send the 4MSB of cmd first, I move them to LSB for the macro to send, the macro will move them to the appropriate position
	LCD_WRITE( cmd >> 4 );
	//Clear RS: I'm sending an instruction
	CLEAR_BIT( RS_PORT.OUT, RS_PIN );
	//Strobe the enable pin, time specify the delay after each edge
	STROBE_EN( EN_STROBE_TIME );
	//Now send the 4LSB of cmd
	LCD_WRITE( cmd );
	//Strobe the enable pin, time specify the delay after each edge
	STROBE_EN( EN_STROBE_TIME );
	//the slowest command take 1.5ms
	_delay_ms( LCD_LONG_WAIT );

	//***********************************************************************
	//	RETURN
	//***********************************************************************

	return;
}   //End function: lcd_send_cmd

/****************************************************************************
**	LCD INIT
*****************************************************************************
**	This function will initialize the LCD display
**  SETTINGS
**
****************************************************************************/

void lcd_init( void )
{
	//***********************************************************************
	//	STATIC VARIABILE
	//***********************************************************************

	//***********************************************************************
	//	LOCAL VARIABILE
	//***********************************************************************

	uint8_t u8t;

	//***********************************************************************
	//	BODY
	//***********************************************************************

		///----------------------------------------------------------------------
		///	COMMAND 5: Bus Width, Number of lines, Font Type
		///----------------------------------------------------------------------

	//Clear the temp var
	u8t = 0x00;
	//Command 5
	SET_BIT( u8t, 5 );
	//Bus Width
	//	0	|	4bit
	//	1	|	8bit
	//SET_BIT( u8t, 4 );
	//Number of lines
	//	0	|	1 line
	//	1	|	2 lines
	SET_BIT( u8t, 3 );
	//Font type
	//	0	|	5x10
	//	1	|	5x7
	//SET_BIT( u8t, 2 );
	//Don't Care
	//SET_BIT( u8t, 1 );
	//Don't Care
	//SET_BIT( u8t, 0 );
	//Send the command a special way (this time only)
	//BUG: The default mode for the LCD is 8bit, the first command must be sent twice, and this way
	lcd_send_cmd( u8t >> 4 );
	//Send the command
	lcd_send_cmd( u8t );

		///----------------------------------------------------------------------
		///	COMMAND 0: Clear the display
		///----------------------------------------------------------------------

	//Clear the temp var
	u8t = 0x00;
	//Command 0
	SET_BIT( u8t, 0 );
	//Send the command
	lcd_send_cmd( u8t );

		///----------------------------------------------------------------------
		///	COMMAND 3: Show Display, Show Cursor, Blink Cursor
		///----------------------------------------------------------------------

	//Clear the temp var
	u8t = 0x00;
	//Command 3
	SET_BIT( u8t, 3 );
	//Show Display Content
	//	0	|	Hidden
	//	1	|	Show
	SET_BIT( u8t, 2 );
	//Show Cursor
	//	0	|	Hide Cursor
	//	1	|	Show Cursor
	//SET_BIT( u8t, 1 );
	//Blink Cursor
	//	0	|	Don't Blink
	//	1	|	Blink
	//SET_BIT( u8t, 0 );
	//Send the command
	lcd_send_cmd( u8t );

		///----------------------------------------------------------------------
		///	INITIALIZE VARIABILES
		///----------------------------------------------------------------------
		//Here, I initialize the lcd variabiles

	//For every digit
	for (u8t = 0;u8t < LCD_ROW*LCD_COL; u8t++)
	{
		//initialize the show vector with spaces
		lcd_show[ u8t ]		= ' ';
	}
	//For every 8 digit bit
	for (u8t = 0;u8t < LCD_UPDT; u8t++)
	{
		//initialize the 8 update bit to 0 (0 = up to date)
		lcd_updt[ u8t ]		= 0;
	}
	//Clear configuration flags (defaults)
	lcd_cfg_flags			= 0x00;
	//Error code
	lcd_error 				= LCD_OK;

	//***********************************************************************
	//	RETURN
	//***********************************************************************

	return;
}   //End function: lcd_init

/****************************************************************************
**	LCD UPDATE
*****************************************************************************
**  Main driver function. User should call this at a fixed frequency (10KHz)
**  This function will automatically sync the content of lcd_show with the LCD display
**	DESCRIPTION:
**	I have two states of operations:
**	>Send commands to display
**	>Generate commands
**	I only generate commands if the comand queque is empty and there are update flags rised
**	If the command queque is empty, and there are no update flag rised, the function will do nothing (very efficient)
**	A huge advantage of the command queque dont this way, is that now I can stop adding commands to the queque if the queque is full
**	A bug of the version 2.0 of the library was that prints directly added command to the queque, so the user could simply overflow the buffer
**	There was no way to solve i, because i could only ignore commands at best
**	Now the update function is the one that decide what to upload to the queque
*****************************************************************************
**		ALGORITHM
*****************************************************************************
**	S0 is the IDLE state, when I'm S0 i search for a '1' in lcd_updt scanning forward
**	If I find it, I'll have to send data to the LCD to sync it with the buffer
**	I use two flags, ADR and H to signal S3 what to do
**	If the lcd cursor is already in the right position I can skip sending the address, i wont' rise ADR flag
**	S1), and S2)= are strightforward, they strobe EN
**	S3) is the important final state. In here I use the ADR and H flag to send the LSB of the byte and to
**	know if i have to send data or address information
**	I follow it with S1 as long as I need to continue
**	Once I'm done, i return to status S0, and I'm ready for the next flag search
**	The last operation of S3 is to check if the sent data is still coherent with the buffer
**	since the user might have already written something new. if it's coherent, i clear the update flag
**
**	STATUS VAR
**	exe(2) 		: i only needs 2 bits to encode the status
**	adr(1)		: signal S3 if i'm sending address informations
**	h(1)		: signal S3 if i have to send the LSB
**	scan(5) 	: current scan position
**	cursor(5)	: current cursor position on the LCD display
**
**	S0) - IDLE
**		Search for a flag, did I find it?
**		N: RET S0 												//nothing to do, the LCD is synced with the buffer
**		is the cursor in the right position?
**		N: CLEAR RS, SET H, SET ADR, Send ADR(H)
**		Y: SET RS, SET H, Send DATA(H)
**		RET S1
**	S1) - SET EN
**		SET EN
**		RET S2
**	S2) - CLEAR EN
**		CLEAR EN
**		if ADR is clear && H is clear?
**			I'm done, RET S0
**		RET S3
**	S3)
**		if ADR is set && H is set?
**			CLEAR H, Send ADR(L), UPDATE status.cursor, RET S1
**		if ADR is set && H is clear?
**			CLEAR ADR, SET H, Send DATA(H), RET S1
**		if ADR is clear && H is set?
**			CLEAR H, Send DATA(L), UPDATE lcd_updt flag, RET S1
****************************************************************************/

void lcd_update( void )
{
	///**************************************************************************
	///	STATIC VARIABILE
	///**************************************************************************

	//Status variable for the LCD FSM
	static Lcd_fsm_status status;
	//Store the char being sent, it prevent glitches when the user update a char currently being scanned
	static uint8_t data_buf;

	///**************************************************************************
	///	LOCAL VARIABILE
	///**************************************************************************

	uint8_t exe_temp;
	uint8_t scan_temp;
	uint8_t u8t, u8t1, u8t2;

	///**************************************************************************
	///	PARAMETER CHECK
	///**************************************************************************

	///**************************************************************************
	///	BODY
	///**************************************************************************

	///Fetch
	//Move status var to temp vars
	exe_temp 		= status.exe;
	scan_temp 		= status.scan;
	///FSM
	//If: S0 - IDLE
	if (exe_temp == 0)
	{
		///----------------------------------------------------------------------
		///	S0 - IDLE
		///----------------------------------------------------------------------
		//	If: I'm idle, I'm searching for the next rised flag using scan
		//	the flag are stored in 4 byte, every bit is a flag for one of 32 digit of the screen
		//	>First I scan the current byte pointed by scan, and see if there are rised update flag there
		//	>Then I scan the following bytes for at least one flag
		//		Y> I scan for the first bit, i will find it
		//		N> Return S0), there is nothing to do
		//	>Do I have to send the address?
		//		Y> rise address flag, send_data=address
		//		N> send_data = data
		//
		///First byte flag detection
		//The byte I'm currently scanning, has a raised flag that I have yet to scan?
		//ex1. 0 1 1 0 0 0 1 0  |  ex2. 0 1 1 0 0 0 1 0
		//scan           ^      |  scan               ^
		//mask 0 0 0 0 0 1 1 1  |  mask 0 0 0 0 0 0 0 1
		//In the ex1. there is a 1 after the scan index, in ex2 no.
		//what I do is to generate a mask with 1 after the scan index
		//The & with the flag byte will return 1 if I have at least one rised flag to scan
		//otherwise I will scan the next bytes
		///
		//generate the mask, the position inside the byte is the 3LSB of scan
		u8t = (0xff << (scan_temp & 0x07));
		//fetch the current flag byte, , the index of the vector is the 2MSB of scan
		u8t1 = lcd_updt[ ((scan_temp & 0x18) >> 3) ];
		//If: The byte currently pointed by scan, still has unreaded update flag
		if ((u8t & u8t1) != 0x00)
		{
			//Here: I know there is at least one rised flag after scan position. I search for it.
			//Create a mask with a 1 in scan
			u8t = MASK( scan_temp & 0x07 );
			//While: mobile mask scan. shift up the bit and & with lcd_updt until I find the flag I'm searching for
			while (u8t != 0x00)
			{
				//If: I found the flag
				if ((u8t & u8t1) != 0x00)
				{
					//DEBUG PRINTF
					DPRINT("S0 - Found rised flag in first status.scan byte: %d\n", scan_temp);
					//DPRINT("S0 - >%2x< >%2x< >%2x<\n", u8t, u8t1, u8t&u8t1);

					//break while
					u8t = 0x00;
				}	// End If: I found the flag
				//If: I did not find the flag
				else
				{
					//increase scan while I'm at it
					scan_temp++;
					//Scan the next bit. u8t will overflow to 0x00 once I'm done
					u8t = u8t << 1;
					//If: the mask overflowed
					if (u8t == 0x00)
					{
						//ERROR: The first check told me that there was a rised bit, but I didn't find it! Algorithmic error
						LCD_ERROR( LCD_WTF );
					}
				}	//End If: I did not find the flag
			}	// End While: mobile mask scan. shift up the bit and & with lcd_updt until I find the flag I'm searching for
		}	//End If: The byte currently pointed by scan, still has unreaded update flag
		//If: I have to search for an update flag in the following four bytes
		else
		{
			//I have to circularly scan the bytes of lcd_updt and scan four of them
			//The fourth is the starting byte, but I have not scanned all bits of it
			//	0	1	2	3
			//	->	| -> scan
			///Setup var
			//clear the 3LSB of scan index, i will start bit search from the first bit of the byte
			scan_temp = scan_temp & 0x18;
			//Get the byte index
			u8t1 = ((scan_temp & 0x18) >> 3);
			//Setup increment
			u8t = 1;
			///Search
			//While: Scan (LCD_UPDT) byte after the one pointed by scan for a rised flag
			while (u8t <= LCD_UPDT)
			{
				//Fetch the content of the byte I'm scanning.
				u8t2 = lcd_updt[ u8t1 ];
				//If: the byte I'm scanning contain at least one rised flag
				if (u8t2 != 0x00)
				{
					//DEBUG PRINTF
					//DPRINT("S0 - Found a byte with risen flag: byte %d, scan %d\n", u8t1, scan_temp);
					//break cycle
					u8t = LCD_UPDT +1;
				}
				//If: the byte I'm scanning contain no rised flags
				else
				{
					//Advance relative increment
					u8t++;
					//Advance the absolute index
					u8t1 = ((u8t1 +1) >= (LCD_UPDT))?(0):(u8t1+1);
					//I update scan index while I'm at it, i advance by 8 bits
					scan_temp = AT_CIRCULAR_SUM( scan_temp, 8, LCD_ROW *LCD_COL );
					//If: the search failed, it means that there are NO risen flags
					if (u8t > LCD_UPDT)
					{
						//It means I have nothing to do, I stay in S0-IDLE
						DPRINT("S0 - No flags found, nothing to do here: scan_temp: %d\n", scan_temp);
						//I'm done
						return;
					}
				}
			}	//End While: Scan (LCD_UPDT) byte after the one pointed by scan for a rised flag
			///HERE:
			//I have found a byte with a risen flag, I have to find the bit now
			///Setup
			//mask with LSB rised
			u8t = 0x01;
			//u8t1 has the index, I fetch the content instead. Scan temp already point to the first bit of that byte
			u8t1 = lcd_updt[ u8t1 ];
			///Search
			//While: I'm searching for a rised flag inside a byte
			while (u8t != 0x00)
			{
				//If: I find the flag I'm looking for
				if ((u8t & u8t1) != 0x00)
				{
					//break the while
					u8t = 0x00;
				}
				//If: flag is clear
				else
				{
					//advance mask
					u8t = u8t << 1;
					//while I'm at it, I update the scan index as well
					scan_temp++;
					//If: I completed the scan and didn't find the flag
					if (u8t == 0x00)
					{
						//ERROR: The first check told me that there was a rised bit, but I didn't find it! Algorithmic error
						LCD_ERROR( LCD_WTF );
					}
				}
			} //End While: I'm searching for a rised flag inside a byte
			//
			DPRINT("S0 - Flag after first byte found: %d\n", scan_temp);

		}	//End If: I have to search for an update flag in the following four bytes
		///HERE:
		//scan_temp hold the position of a char to be processed
		///LOAD DATA BUFFER, UPDATE STATUS VARS, JUMP
		//BUG the cursor from 0,15 move out of screen to 0,16 NOT 1,0 For this position only, I always have to specify the address
		//If: the cursor is already in the correct position
		if (((scan_temp & 0x0f) != 0) && (scan_temp == status.cursor))
		{
			//I will skip sending the address
			status.adr 		= 0;
			//Load data into the buffer
			u8t 			= lcd_show[ scan_temp ];
			data_buf 		= u8t;
			//SET RS line, i'm sending data
			SET_BIT( RS_PORT.OUT, RS_PIN );
			DPRINT("S0 - Cursor is Up To Date, sending digit: %x\n", u8t);
		}
		//If: I have to move the cursor
		else
		{
			//I'm sending the address
			status.adr 		= 1;
			//Load data into the buffer
			//	B7='1' is signature for DDRAM write instruction
			//	B6='1' is address for the second row, i have to move B4 to B6
			//	B3...B0 is the LSB of the address
			u8t 			= MASK(7) | ((scan_temp & 0x10) << 2) | (scan_temp & 0x0f);
			data_buf 		= u8t;
			//CLEAR RS line, i'm sending an address
			CLEAR_BIT( RS_PORT.OUT, RS_PIN );
			DPRINT("S0 - Sending cursor position: scan: %d cursor: %d adr: %x\n", scan_temp, status.cursor, u8t);
		}
		//I'm sending the H part
		status.h		= 1;
		//send the H part of the buffer (still stored in u8t)
		LCD_WRITE( u8t >> 4 );
		//Jump to S1)
		exe_temp 		= 1;
	}	//End If: S0 - IDLE
	//If: S1 - SET EN
	else if (exe_temp == 1)
	{
		DPRINT("S1 - SET_EN\n");

		SET_BIT( EN_PORT.OUT, EN_PIN );
		//Jump S2)
		exe_temp 		= 2;
	}	//End If: S1 - SET EN
	//If: S2 - CLEAR EN
	else if (exe_temp == 2)
	{
		//
		CLEAR_BIT( EN_PORT.OUT, EN_PIN );
		//If I have sent DATA(L)
		if ((status.adr == 0) && (status.h == 0))
		{
			DPRINT("S2 - CLEAR_EN, DATA LSB was sent, I'm done\n");
			//I'm done, I don't have to execute the next state (no pins needs to be toggled
			exe_temp 		= 0;
		}
		//default case
		else
		{
			DPRINT("S2 - CLEAR_EN\n");
			//Jump S3)
			exe_temp 		= 3;
		}
	}	//End If: S2 - CLEAR EN
	//If: S3 -
	else if (exe_temp == 3)
	{
		///----------------------------------------------------------------------
		///	S3 -
		///----------------------------------------------------------------------
		//	The final state, i use status.h, status.adr and data_buf
		//	Here i setup DATA and RS line, and jump to S1) to strobe EN line

		//If: I have sent ADR(H)
		if ((status.adr == 1) && (status.h == 1))
		{
			///Send ADR(L)
			//Clear H flag
			status.h 			= 0;
			//load 4LSB of data buffer
			u8t 				= data_buf;
			//Send ADR(L)
			LCD_WRITE( u8t & 0x0f );
			///Update cursor
			//I have written ADR, I need to update the cursor
			status.cursor = scan_temp;
			DPRINT("S3 - Send ADR LSB\n");
		}
		//If: I have sent ADR(L)
		else if ((status.adr == 1) && (status.h == 0))
		{
			///Send DATA(H)
			//Set RS line (I'm sending data, not instructuins)
			SET_BIT( RS_PORT.OUT, RS_PIN );
			//Clear ADR flag
			status.adr 			= 0;
			//Set H flag
			status.h 			= 1;
			//load digit into buffer
			//Remember that user can async write into it, that's why I need a buffer in the first place
			u8t 				= lcd_show[ scan_temp ];
			data_buf			= u8t;
			//Send DATA(H)
			LCD_WRITE( u8t >> 4 );
			DPRINT("S3 - Send DATA MSB\n");
		}
		//If: I have sent DATA(H)
		else if ((status.adr == 0) && (status.h == 1))
		{
			///Send DATA(L)
			//Clear H flag
			status.h 			= 0;
			//load 4LSB of data buffer
			u8t 				= data_buf;
			//Send ADR(L)
			LCD_WRITE( u8t & 0x0f );
			///Update lcd_updt
			//If: the LCD is synced with lcd_show
			if (u8t == lcd_show[ scan_temp ])
			{
				//I can clear the flag, B3 B4 address the byte inside the vector, B0-B2 address the bit inside the byte
				CLEAR_BIT( lcd_updt[ ((scan_temp & 0x18) >> 3) ], (scan_temp & 0x07) );
			}
			///Update cursor
			//A successful write to the LCD will advance the cursor
			u8t = status.cursor;
			//circular increment of cursor
			u8t = ((u8t +1) < (LCD_ROW*LCD_COL))?(u8t +1):(0);
			//write back cursor
			status.cursor = u8t;
			DPRINT("S3 - Send DATA LSB\n");
		}
		//If: I have sent DATA(L), but I should have quit earlier (no need to strobe again). This is an error
		else
		{
			//
			LCD_ERROR( LCD_WTF );
		}
		//Jump to S1) I have setup DATA and RS lines, I need to strobe EN
		exe_temp = 1;
	}	//End If: S3 -
	//Default case (impossibile)
	else
	{
		//ERROR: This is an impossibile state, something went REALLY wrong
		LCD_ERROR( LCD_WTF );
	}


	///**************************************************************************
	///	WRITE BACK
	///**************************************************************************

	//save updated status var
	status.exe 		= exe_temp;
	status.scan 	= scan_temp;

	///**************************************************************************
	///	RETURN
	///**************************************************************************

	return;
}	//end function: display_update

/****************************************************************************
**	LCD CONFIG
*****************************************************************************
**	This function configure certain behaviours of the print function
****************************************************************************/

void lcd_config( uint8_t cfg, uint8_t val )
{
	//***********************************************************************
	//	STATIC VARIABILE
	//***********************************************************************

	//***********************************************************************
	//	LOCAL VARIABILE
	//***********************************************************************

	//***********************************************************************
	//	BODY
	//***********************************************************************

	//Configure number alignment for the print functions
	if (cfg == LCD_ADJ)
	{
		//If the user wants left adjust (write MSB on pos and go right)
		if (val == LCD_ADJ_L)
		{
			SET_BIT( lcd_cfg_flags, LCD_ADJ_FLAG );
		}
		//If the user wants right adjust (write LSB on pos and go left) (Default)
		if (val == LCD_ADJ_R)
		{
			CLEAR_BIT( lcd_cfg_flags, LCD_ADJ_FLAG );
		}
	}

	//***********************************************************************
	//	RETURN
	//***********************************************************************

	return;
}   //End function:

/****************************************************************************
*****************************************************************************
**	PRINT GROUP
*****************************************************************************
****************************************************************************/

/****************************************************************************
**	LCD PRINT CHAR
*****************************************************************************
**	Write a char in the show vector, update the flag is required
**  Input:
**  >pos:       Vector position
**  >data:      Display data
****************************************************************************/

void lcd_print_char( uint8_t pos, uint8_t data )
{
	///**************************************************************************
	///	STATIC VARIABILE
	///**************************************************************************

	///**************************************************************************
	///	LOCAL VARIABILE
	///**************************************************************************

	///**************************************************************************
	///	PARAMETER CHECK
	///**************************************************************************

	if (pos > (LCD_ROW*LCD_COL))
	{
		//error, Out Of Bound write
		LCD_ERROR( LCD_OOB );
		//I'm done
		return;
	}

	///**************************************************************************
	///	BODY
	///**************************************************************************
	//	I only do something if I'm writing something different in lcd_show
	//	This already is a huge improvement from V2.0 which always sent commands

	//If: I'm writing a different byte to a location
	if (lcd_show[ pos ] != data )
	{
		//write the new data
		lcd_show[ pos ] = data;
		//Rise the update bit
		//	Location inside the vector is pos/8
		//	Bit position inside the Location is the 3LSB of pos (reminder of division by 8)
		SET_BIT( lcd_updt[ ((pos & 0x18) >> 3) ], (pos & 0x07) );
	}

	///**************************************************************************
	///	RETURN
	///**************************************************************************

	return;
}	//end Function: lcd_print_char

/****************************************************************************
**	PRINT STR
*****************************************************************************
**	Print a string starting from screen position
**	If reach EOL, it starts from the following one
****************************************************************************/

void lcd_print_str( uint8_t pos, const char *str )
{
	//***********************************************************************
	//	STATIC VARIABILE
	//***********************************************************************

	//***********************************************************************
	//	LOCAL VARIABILE
	//***********************************************************************

	uint8_t t;

	//***********************************************************************
	//	PARAMETER CHECK
	//***********************************************************************

	//If: NULL pointer
	if (str == nullptr)
	{
		return;
	}

	//***********************************************************************
	//	MAIN
	//***********************************************************************

	//Initialize
	t = 0;
	//While: I have char or I have space on screen
	while ( ((pos +t) <= (LCD_ROW *LCD_COL)) && (str[ t ] != '\0') )
	{
		//Print char on screen
		lcd_print_char( pos +t, str[ t ] );
		//next char
		t++;
	}

	//***********************************************************************
	//	RETURN
	//***********************************************************************

	return;
}	//End function: print_str

/****************************************************************************
**	LCD PRINT uint8_t
*****************************************************************************
**	Print a uint8_t number on screen
**	I print the spaces in the empty digit too otherwise if
**	I write a 1 digit number after a 3 digit one in the same position,
**	the old numbers will still be there
**	SETTINGS
**	ALIGNMENT
**	POS		|v					|v
**	RIGHT 	|123		LEFT	|123
**			|  1			 	|1
****************************************************************************/

void lcd_print_u8( uint8_t pos, uint8_t num )
{
	//***********************************************************************
	//	STATIC VARIABILE
	//***********************************************************************

	//***********************************************************************
	//	LOCAL VARIABILE
	//***********************************************************************

	//Temp string to accommodate the number
	char str[ User::String::Config::DIGIT8 +1 ];
	//return var of the number to string conversion
	uint8_t ret;
	//Temp vars
	uint8_t u8t, u8t1;

	//***********************************************************************
	//	BODY
	//***********************************************************************

	//convert the number
	ret = User::String::num_to_str( num, User::String::Config::DIGIT8 +1, str );
	//For: every digit
	for (u8t = 0;u8t < User::String::Config::DIGIT8; u8t++)
	{
		//If: left alignment
		if (IS_BIT_ONE( lcd_cfg_flags, LCD_ADJ_FLAG ))
		{
			//If: I'm outside the number
			if (u8t >= ret)
			{
				//I write spaces to blank older numbers
				u8t1 = ' ';
			}
			else
			{
				//Print number
				u8t1 = str[ u8t ];
			}
		}	//End If: left alignment
		//If: right alignment
		else
		{
			//If: I'm outside the number (yes, that's the condition,right adjustment is messy)
			if ((u8t +ret) < User::String::Config::DIGIT8)
			{
				//I write spaces to blank older numbers
				u8t1 = ' ';
			}
			//If: I'm writing a digit
			else
			{
				//print number, I have to write digit in a mesi order with this adjust
				u8t1 = str[ u8t +ret -User::String::Config::DIGIT8 ];
			}
		}	//End If: right alignment
		//I want to call a single print, I have calculated the argument based on the settings of the print
		lcd_print_char( pos +u8t, u8t1 );
	}	//End For: every digit

	//***********************************************************************
	//	RETURN
	//***********************************************************************

	return;
}   //End function: lcd_print_u8

/****************************************************************************
**	LCD PRINT int8_t
*****************************************************************************
**	Print a int8_t number on screen
**	I print the spaces in the empty digit too otherwise if
**	I write a 1 digit number after a 3 digit one in the same position,
**	the old numbers will still be there
**	SETTINGS
**	ALIGNMENT
**	POS		|v					|v
**	RIGHT 	|123		LEFT	|123
**			|  1			 	|1
****************************************************************************/

void lcd_print_s8( uint8_t pos, int8_t num )
{
	//***********************************************************************
	//	STATIC VARIABILE
	//***********************************************************************

	//***********************************************************************
	//	LOCAL VARIABILE
	//***********************************************************************

	//Temp string to accomodate the number
	char str[ User::String::Config::DIGIT8 +1 ];
	//return var of the number to string conversion
	uint8_t ret;
	//Temp vars
	uint8_t u8t, u8t1;

	//***********************************************************************
	//	BODY
	//***********************************************************************

	//convert the number
	ret = User::String::num_to_str( num, User::String::Config::DIGIT8 +1, str );
	//For: every digit
	for (u8t = 0;u8t < (User::String::Config::DIGIT8 +1); u8t++)
	{
		//If: sign handling
		if (u8t == 0)
		{
			//Print sign
			u8t1 = str[ u8t ];
		}	//End If: sign handling
		//If: left alignment
		else if (IS_BIT_ONE( lcd_cfg_flags, LCD_ADJ_FLAG ))
		{
			//If: I'm outside the number
			if (u8t >= ret)
			{
				//I write spaces to blank older numbers
				u8t1 = ' ';
			}
			else
			{
				//Print number
				u8t1 = str[ u8t ];
			}
		}	//End If: left alignment
		//If: right alignment
		else
		{
			//If: I'm outside the number (yes, that's the condition,right adjustment is messy)
			if ((u8t +ret ) <= (User::String::Config::DIGIT8 +1))
			{
				//I write spaces to blank older numbers
				u8t1 = ' ';
			}
			//If: I'm writing a digit
			else
			{
				//print number, I have to write digit in a mesi order with this adjust
				u8t1 = str[ u8t -1 +ret -User::String::Config::DIGIT8 ];
			}
		}	//End If: right alignment
		//I want to call a single print, I have calculated the argument based on the settings of the print
		lcd_print_char( pos +u8t, u8t1 );
	}	//End For: every digit

	//***********************************************************************
	//	RETURN
	//***********************************************************************

	return;
}   //End function: lcd_print_s8

/****************************************************************************
**	LCD PRINT uint16_t
*****************************************************************************
**	Print a uint8_t number on screen
**	I print the spaces in the empty digit too otherwise if
**	I write a 1 digit number after a 3 digit one in the same position,
**	the old numbers will still be there
**	SETTINGS
**	ALIGNMENT
**	POS		|v					|v
**	RIGHT 	|123		LEFT	|123
**			|  1			 	|1
****************************************************************************/

void lcd_print_u16( uint8_t pos, uint16_t num )
{
	//***********************************************************************
	//	STATIC VARIABILE
	//***********************************************************************

	//***********************************************************************
	//	LOCAL VARIABILE
	//***********************************************************************

	//Temp string to accommodate the number
	char str[ User::String::Config::DIGIT16 +1 ];
	//return var of the number to string conversion
	uint8_t ret;
	//Temp vars
	uint8_t u8t, u8t1;

	//***********************************************************************
	//	BODY
	//***********************************************************************

	//convert the number
	ret = User::String::num_to_str( num, User::String::Config::DIGIT16, str );
	//For: every digit
	for (u8t = 0;u8t < User::String::Config::DIGIT16; u8t++)
	{
		//If: left alignment
		if (IS_BIT_ONE( lcd_cfg_flags, LCD_ADJ_FLAG ))
		{
			//If: I'm outside the number
			if (u8t >= ret)
			{
				//I write spaces to blank older numbers
				u8t1 = ' ';
			}
			else
			{
				//Print number
				u8t1 = str[ u8t ];
			}
		}	//End If: left alignment
		//If: right alignment
		else
		{
			//If: I'm outside the number (yes, that's the condition,right adjustment is messy)
			if ((u8t +ret) < User::String::Config::DIGIT16)
			{
				//I write spaces to blank older numbers
				u8t1 = ' ';
			}
			//If: I'm writing a digit
			else
			{
				//print number, I have to write digit in order with this adjust
				u8t1 = str[ u8t +ret -User::String::Config::DIGIT16 ];
			}
		}	//End If: right alignment
		//I want to call a single print, I have calculated the argument based on the settings of the print
		lcd_print_char( pos +u8t, u8t1 );
	}	//End For: every digit

	//***********************************************************************
	//	RETURN
	//***********************************************************************

	return;
}   //End function: lcd_print_u16
