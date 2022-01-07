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
**	UNIVERSAL PARSER
*****************************************************************************
**	Author: 			Orso Eric
**	Creation Date:		2019-06-17
**	Last Edit Date:		2020-01-15
**	Revision:			2
**	Version:			5.0
****************************************************************************/

/****************************************************************************
**	DESCRIPTION
*****************************************************************************
**	Do a universal parser that does not require malloc/free and do not use explicit dictionary
**	I create a special register command function in which the message and an handler is provided
****************************************************************************/

/****************************************************************************
**	COMMAND RULES
*****************************************************************************
**		ADD COMMAND TO PARSER
**	A command is composed by fixed alphanumeric chars and argument descriptors
**	Each command is associated with a unique function callback
**	A command can only start with letters
**
**		EXAMPLE ADD COMMAND TO PARSER
**	myparser.add_cmd("P", (void *)&my_ping_handler );
**	Add a new command that is triggered when the string P\0 is received.
**	function my_ping_handler will be automatically executed when the \0 is processed
**	myparser.parse( 'P' );
**	myparser.parse( '\0' );
**	send manually bytes to the parser to test the system
*****************************************************************************
**	Command restriction:
**	>Can only start with a letter
**	>Valid arguments are %u %s %U %S %d %D
**	>There can be no commands with same start but two different argument type. Only the first one will be considered in case
**		PWM%d...
**		PWM%u...
**	The parser has no way of knowing if the incoming input (example 10) is meant for one command or the other
**	>After valid ID access parameters using Parser Macros
****************************************************************************/

/****************************************************************************
**	KNOWN BUG
*****************************************************************************
**
****************************************************************************/

/****************************************************************************
**	INCLUDES
****************************************************************************/

#include <stdint.h>
//#define ENABLE_DEBUG
#include "debug.h"
//Class Header
#include "uniparser.h"

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
//!	Uniparser | void
/***************************************************************************/
// @param
//! @return no return
//!	@details
//! Empty constructor
/***************************************************************************/

Uniparser::Uniparser( void )
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
	//! @details algorithm:

	//Initialize structure to safe values
	this -> init();
	//Pass a terminator to the parser to have it initialize itself
	this -> parse( '\0' );

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return;
}	//end constructor:

/****************************************************************************
*****************************************************************************
**	DESTRUCTORS
*****************************************************************************
****************************************************************************/

/***************************************************************************/
//!	@brief Empty Destructor
//!	Uniparser | void
/***************************************************************************/
// @param
//! @return no return
//!	@details
//! Empty destructor
/***************************************************************************/

Uniparser::~Uniparser( void )
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
	//! @details algorithm:

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return;
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

/***************************************************************************/
//!	@brief Public Getter
//!	get_error | void
/***************************************************************************/
//! @return Err_codes | error code
//!	@details
//! Get current error code of the parser
/***************************************************************************/

Err_codes Uniparser::get_error( void )
{
	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return this -> g_err;
}	//end destructor: get_error | void


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
//!	add_command | const char *, void *
/***************************************************************************/
//! @param cmd | const char * 		Text that will trigger the command
//!	@param handler | void * 		Pointer to callback function for this command
//! @return bool | false: OK | true: fail
//!	@details
//! Add a string and a function pointer to the parser
//!	@todo check that command is valid
/***************************************************************************/

bool Uniparser::add_cmd( const char *cmd, void *handler )
{
	DENTER_ARG("cmd: %p >%s<\n", (void *)cmd, cmd );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//index
	uint8_t t;

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//if: input is invalid
	if ((cmd == nullptr) || (handler == nullptr))
	{
		this -> g_err = ERR_INVALID_CMD;
		DRETURN_ARG("ERR%d: ERR_INVALID_CMD\n", this -> g_err);
		return true;	//fail
	}
	//If: num command is invalid
	if ((UNIPARSER_PENDANTIC_CHECKS) && ((this -> g_num_cmd < 0) || (this->g_num_cmd >= UNIPARSER_MAX_CMD)) )
	{
		this -> g_err = ERR_GENERIC;
		DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
		return true;	//fail
	}
	//if: maximum number of command has been reached
	if (this -> g_num_cmd >= (UNIPARSER_MAX_CMD-1))
	{
		this -> g_err = ERR_ADD_MAX_CMD;
		DRETURN_ARG("ERR%d: ERR_ADD_MAX_CMD in line: %d\n", this -> g_err, __LINE__ );
		return true;	//fail
	}
	// check the validity of the string
	bool f_ret = this -> chk_cmd((const uint8_t *)cmd);
	//If: command had a syntax error
	if (f_ret == true)
	{
		DRETURN_ARG("command didnt get past argument descriptor check\n");
		return true;	//FAIL
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Fetch currently used command
	t = this -> g_num_cmd;
	//Link command handler and command text
	this -> g_cmd_txt[t] = (uint8_t *)cmd;
	this -> g_cmd_handler[t] = handler;
	DPRINT("Command >%s< with handler >%p< has been added with index: %d\n", cmd, (void *)handler, t);
	//A command has been added
	this -> g_num_cmd = t +1;
	DPRINT("Total number of commands: %d\n", this -> g_num_cmd);

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN();
	return false;
}	//end method: add_command | const char *, void *

/***************************************************************************/
//!	@brief Public Method
//!	parse | char
/***************************************************************************/
//! @param data | uint8_t | incoming character to be processed through the universal parser
//! @return bool | false = OK | true = A runtime error occurred
//!	@details
//! Intricate FSM. The objective is to check the incoming character against
//!	charaters inside the various commands and decode a command when a \0 is detected
/***************************************************************************/

bool Uniparser::parse( uint8_t data )
{
	if ((data < '0') || (data > 'z'))
	{
		DENTER_ARG("exe: >0x%x<\n", data );
	}
	else
	{
		DENTER_ARG("exe: >0x%x< >%c<\n", data, data );
	}

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//when true, reset the FMS
	bool f_rst_fsm = false;
	//Index of the handler to be executer
	int8_t exe_index = -1;

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

		//----------------------------------------------------------------
		//	CLEAR ERROR
		//----------------------------------------------------------------
		//	User was notified of the error during previous call
		//	Errors should have triggered an FSM reset to return in working condition
		//	I can clear the error and keep churning bytes

	//If: an error was reported
	if (this -> g_err != Err_codes::NO_ERR)
	{
		//Clear the error
		DPRINT("%d | Clear Error: %d\n", __LINE__, this -> g_err);
		this -> g_err = Err_codes::NO_ERR;
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

		//----------------------------------------------------------------
		//	TERMINATOR
		//----------------------------------------------------------------
		//! @details when terminator is detected, close the command.
		//!	I either have a full match, or the command is invalid and I have to reset the FSM

	//If: input terminator from user
	if (data == '\0')
	{
		DPRINT("%d | Terminator detected | Number of partial matches: %d\n", __LINE__, this -> g_num_match);
		//counter
		uint8_t t;
		//If: i was decoding an argument
		if (this -> g_status == Orangebot::Parser_status::PARSER_ARG)
		{
			//I'm done decoding
			DPRINT("%d | Terminator after ARG\n", __LINE__);
			//Close current argument and update the argument detector FSM.
			bool f_ret = this -> close_arg();
			//if could not close argument
			if (f_ret == true)
			{
				//I can recover from this.
				//no matches and reset the FSM.
				this -> g_num_match = 0;
			}
		}
		//Index inside the command
		uint8_t cmd_index;
		//if: I have at least one parser_tmp.id_index entry
		if (this -> g_num_match > 0)
		{
			DPRINT("%d | Scanning partially matched commands for terminators\n", __LINE__);
			//For: scan all commands
			for (t = 0;t<this -> g_num_cmd;t++)
			{
				//Fetch command index
				cmd_index = this -> g_cmd_index[t];
				//If: this command is a partial match
				if (cmd_index > 0)
				{
					//If: i was decoding an argument
					if (this -> g_status == Parser_status::PARSER_ARG)
					{
						//If: index is not pointing to an argument descriptor inside the command
						if ((UNIPARSER_PENDANTIC_CHECKS) && (this -> g_cmd_txt[t][cmd_index] != '%'))
						{
							DPRINT("ERR: This command should have an argument descriptor in this position. | cmd: %d | cmd_index: %d | actual content: >0x%x<\n", t, cmd_index, this -> g_cmd_txt[t][cmd_index]);
							this -> g_err = Err_codes::ERR_GENERIC;
							DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
							return true;	//fail
						}
						//The argument has been closed. I need to skip the argument descriptor "%?"
						cmd_index += 2;
						//With argument closed I'm now doing an ID matching for terminator. Can be skipped since it's the last char
						this -> g_status = Parser_status::PARSER_ID;
						//I should write back the index. Can be skipped since it's the last char
						this -> g_cmd_index[t] = cmd_index;
					}
					//Check match against the character after the one already matched.
					//If: the next command data would be the terminator '\0'
					if ( this -> g_cmd_txt[t][ cmd_index ] == '\0')
					{
						//If there is a pending execution already
						if ((UNIPARSER_PENDANTIC_CHECKS) && (exe_index != -1))
						{
							//this should never happen
							this -> g_err = Err_codes::ERR_GENERIC;
							DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
							return true;	//fail
						}
						//Issue execution of the callback function linked
						exe_index = t;
						DPRINT("Valid command ID%d decoded\n", t);
						//I can stop the search
						t = this -> g_num_cmd;
					}
					//If: command would keep on going
					else
					{
						DPRINT("%d | no match in %x cmd %x | ", __LINE__ ,data , this -> g_cmd_txt[t][ cmd_index ] );
					}
				}	//End If: this command is a partial match
			}	//End For: scan all commands
		}	//end if: I have at least one parser_tmp.id_index entry
		//If I have just one match.  g_num_match now holds the index of the match shifted by one
		else if (this -> g_num_match < 0)
		{
			//decode command index | BUGFIX: -1 means only one match of command ID 0
			t = -this -> g_num_match -1;
			DPRINT("just one partial match: %d\n", t);
			//Fetch command index
			cmd_index = this -> g_cmd_index[t];
			//If: i was decoding an argument
			if (this -> g_status == Parser_status::PARSER_ARG)
			{
				//If: index is not pointing to an argument descriptor inside the command
				if ((UNIPARSER_PENDANTIC_CHECKS) && (this -> g_cmd_txt[t][cmd_index] != '%'))
				{
					DPRINT("ERR: This command should have an argument descriptor in this position. | cmd: %d | cmd_index: %d | actual content: >0x%x<\n", t, cmd_index, this -> g_cmd_txt[t][cmd_index]);
					this -> g_err = Err_codes::ERR_GENERIC;
					DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
					return true;	//fail
				}
				//The argument has been closed. I need to skip the argument descriptor "%?"
				cmd_index += 2;
				//With argument closed I'm now doing an ID matching for terminator. Can be skipped since it's the last char
				//this -> g_status == Parser_status::PARSER_ID;
			}
			//BUGFIX: this branch can now be executed with null pointer if dictionary is yet to be initialized. This is not a pedantic check
			//Check match  against the character after the one already matched.
			//If: the next command data would be the terminator '\0'
			if ((this -> g_cmd_txt[t] != nullptr) && (this -> g_cmd_txt[t][ cmd_index ] == '\0'))
			{
				//If there is a pending execution already
				if ((UNIPARSER_PENDANTIC_CHECKS) && (exe_index != -1))
				{
					//this should never happen
					this -> g_err = Err_codes::ERR_GENERIC;
					DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
					return true;	//fail
				}
				//Issue execution of the callback function linked
				exe_index = t;
				DPRINT("%d | Valid command ID%d decoded\n", __LINE__, t);
				//I can stop the search
				t = this -> g_num_cmd;
			}
			//BUGFIX: this branch can now be executed with null pointer if dictionary is yet to be initialized. This is not a pedantic check
			//if: I'm given a terminator but dictionary does not contain a terminator
			else
			{
				if (this -> g_cmd_txt[t] != nullptr)
				{
					//This happen if user gives a command that lack one char
					DPRINT("%d | no match. given: >0x%x< expected: >0x%x<\n" ,__LINE__, data , this -> g_cmd_txt[t][ cmd_index ] );
				}
				else
				{
					//This happen if user gives a command that lack one char
					DPRINT("%d | Dictionary entry is nullptr. Maybe dictionary is yet to be initialized? Dictionary entry: %d\n",__LINE__,  t  );
				}
				//Issue a FSM reset
				f_rst_fsm = true;
			}	//End if: I'm given a terminator but dictionary does not contain a terminator
		}	//End If: I have just one match g_num_match now holds the index of the match shifted by one
		//If: exactly zero matches.
		else
		{
			DPRINT("%d | No partial matches\n", __LINE__ );
		}
		//Issue a FSM reset
		f_rst_fsm = true;
	}	//End If: input terminator from user

		//--------------------------------------------------------------------------
		//	PARSER_IDLE
		//--------------------------------------------------------------------------
		//	Only letters can be used as first character in a command
		//	This section matches the first character in each command
		//	This section takes care of initializing g_cmd_index[] to valid partial match values

	//If: PARSER_IDLE
	else if (this -> g_status == Parser_status::PARSER_IDLE)
	{
		DPRINT("%d | PARSER_IDLE\n", __LINE__);
		//If: letter First character in a command can only be a letter
		if (IS_LETTER( data ))
		{
			//counter
			uint8_t t;
			//for each command
			for (t = 0; t < this -> g_num_cmd;t++)
			{
				//if: dictionary is unlinked
				if ((UNIPARSER_PENDANTIC_CHECKS) && (this -> g_cmd_txt[t] == nullptr))
				{
					this -> g_err = Err_codes::ERR_GENERIC;
					DRETURN_ARG("%d | ERR%d: ERR_GENERIC\n", __LINE__, this -> g_err );
					return true;	//fail
				}
				//If: partial match
				if (this -> g_cmd_txt[t][0] == data)
				{
					//A partial match has been found
					this -> g_num_match++;
					//Match has been found up to first character. Point to the next unmatched char
					this -> g_cmd_index[t] = 1;
					//Next, I'm matching ID entries
					this -> g_status = Parser_status::PARSER_ID;
					//TIP: I can't speculatively detect % here because two commands may have the same first section and diverge later.
					DPRINT("%d | Match command %d, total partial matches: %d\n", __LINE__, t, this -> g_num_match);
				}
				//if: no match
				else
				{
					//special code for no match found
					this -> g_cmd_index[t] = 0;
				}
			}	//end for: each command
		}	//End If: letter
		//Non letter can never be a first character
		else
		{
			//Issue a FSM reset
			f_rst_fsm = true;
		}
	}	//End If: PARSER_IDLE

		//--------------------------------------------------------------------------
		//	ID matching
		//--------------------------------------------------------------------------

	//if: I'm ID matching
	else if (this -> g_status == Parser_status::PARSER_ID)
	{
		DPRINT("%d | PARSER_ID ", __LINE__);
		//! @todo only go ARG if a command has an arg descriptor if there are number or sign. allow number or sign to be used as command ID
		//if: I'm being fed an argument
		if (IS_NUMBER( data ) || IS_SIGN( data ))
		{
			DPRINT_NOTAB("- ARG | num_match %d\n", this -> g_num_match);
			//if: I have at least one partial match
			if (this -> g_num_match > 0)
			{
				//for each dictionary command
				for (uint8_t t = 0;t < this -> g_num_cmd;t++)
				{
					//if: the command is a partial match
					if (this -> g_cmd_index[t] > 0)
					{
						//Search in the dictionary for a % entry. An argument descriptor
						if (this -> g_cmd_txt[t][ this -> g_cmd_index[t] ] == '%')
						{
							//Do not increment index but go in ARG parsing mode
							this -> g_status = Parser_status::PARSER_ARG;
							//Do not advance index until argument decoding is complete
							DPRINT("%d | ARG begins | command: %d\n", __LINE__, t);
							//Add an argument using current partial match as template
							bool f_ret = this -> add_arg( t );
							//if: adding argument failed
							if ((UNIPARSER_PENDANTIC_CHECKS) && (f_ret == true))
							{
								this -> error_handler( Err_codes::ERR_ADD_ARG );
								DPRINT("%d | ERR Failed to add an argument\n", __LINE__ );
								//I can recover from this by resetting the FSM
								f_rst_fsm = true;
							}
							//initialize agrument
							this -> accumulate_arg( data );
							//argument detection make the detection unique. Remove all other partial matches | BUGFIX: now negative index is shifted by one
							this -> g_num_match = -t -1;
							//Single match has been found. Break the cycle
							t = UNIPARSER_MAX_CMD;
						}
						//If: this dictionary entry does not contain an argument descriptor
						else
						{
							//Prune away the partial match
							this -> g_cmd_index[t] = 0;
							DPRINT("%d | Prune away partial match: %d | ", __LINE__, t);
							//if: I still have partial matches after pruning
							if (this -> g_num_match >= 2)
							{
								//One fewer partial match
								this -> g_num_match--;
								DPRINT_NOTAB(" num_match: %d\n", this -> g_num_match );
							}
							//if: all partial matches have been pruned away
							else
							{
								//No more commands. Reset the machine
								f_rst_fsm = true;
								//No more matches. Can breack cycle early
								t = UNIPARSER_MAX_CMD;
								DPRINT_NOTAB("%d | Last partial match has been pruned away... RESET\n", __LINE__);
								//! @todo command refeed function. Safe and refeed last char to detect other partial commands
							}
						}	//End If: this dictionary entry does not contain an argument descriptor
					}	//End if: the command is a partial match
					//If: this commnd did not match to begin with
					else
					{
						//do nothing
					}
				} //end for each dictionary command
			}	//end if: I have at least one partial match
			//If I only have one match | BUGFIX: single match is now shifted by one
			else if (this -> g_num_match < 0)
			{
				//decode command index
				uint8_t t = -this -> g_num_match -1;
				//Search in the dictionary for a % entry. An argument descriptor
				if (this -> g_cmd_txt[t][ this -> g_cmd_index[t] ] == '%')
				{
						//! This is the first char of an argument
					//Do not increment index but go in ARG parsing mode
					this -> g_status = Parser_status::PARSER_ARG;
					//Do not advance index until argument decoding is complete
					DPRINT("%d | ARG begins | command: %d\n", __LINE__, t);
					//Add an argument using current partial match as template
					bool f_ret = this -> add_arg( t );
					//if: adding argument failed
					if ((UNIPARSER_PENDANTIC_CHECKS) && (f_ret == true))
					{
						this -> g_err = Err_codes::ERR_GENERIC;
						DPRINT("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
						//I can recover from this by resetting the FSM
						f_rst_fsm = true;
					}
					//!	accumulate argument character inside argument.
					f_ret = this -> accumulate_arg( data );
					//if: adding argument failed
					if ((UNIPARSER_PENDANTIC_CHECKS) && (f_ret == true))
					{
						this -> g_err = Err_codes::ERR_GENERIC;
						DPRINT("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
						//I can recover from this by resetting the FSM
						f_rst_fsm = true;
					}
				}
			}	//If I only have one match
			//If: I had no matches to begin with
			else
			{
				//DO nothing
			}
		}	//end if: I'm being fed an argument
		//if: I'm matching a non argument non terminator
		else
		{
			DPRINT_NOTAB("- ID\n");
			//if: I have at least one partial match
			if (this -> g_num_match > 0)
			{
				//If I have just one match, I can upgrade num_match to skip for next time
				bool f_match = (this -> g_num_match == 1);
				//for each dictionary command
				for (uint8_t t = 0;t < this -> g_num_cmd;t++)
				{
					//if: the command is a partial match
					if (this -> g_cmd_index[t] > 0)
					{
						//check that the dictionary holds the same value as data
						if (this -> g_cmd_txt[t][ this -> g_cmd_index[t] ] == data)
						{
							//Advance to the next dictionary entry for this command
							this -> g_cmd_index[t]++;
							//if: This is the sole partial match surviving
							if (f_match == true)
							{
								//Save the index inside num match to skip the for next time | BUGFIX: Now single match is shifted by one
								this -> g_num_match = -t -1;
								//Single match has been found. Break the cycle
								t = UNIPARSER_MAX_CMD;
								DPRINT("%d | Only one partial match remains. Num_match: %d\n", __LINE__, this -> g_num_match);
							}
						}
						//Else: this dictionary entry does not contain an argument descriptor
						else
						{
							//Prune away the partial match
							this -> g_cmd_index[t] = 0;
							DPRINT("%d | Prune away partial match: %d | ", __LINE__, t);
							//if: I still have partial matches after pruning
							if (this -> g_num_match >= 2)
							{
								//One fewer partial match
								this -> g_num_match--;
								DPRINT_NOTAB(" num_match: %d\n", this -> g_num_match );
							}
							//if: all partial matches have been pruned away
							else
							{
								//No more commands. Reset the machine
								f_rst_fsm = true;
								//No more matches. Can breack cycle early
								t = UNIPARSER_MAX_CMD;
								DPRINT_NOTAB("Last partial match has been pruned away... RESET\n");
								//! @todo replay system. Safe and refeed last char to detect other partial commands
							}
						}
					}	//End if: the command is a partial match
					//if: the command was pruned away long ago
					else
					{
						//do nothing
					}
				} //end for each dictionary command
			}	//end if: I have at least one partial match
			//BUGFIX: If the only match is also entry 0
			//If I only have one match
			else //if (this -> g_num_match <= 0)
			{
				//decode command index | BUGFIX: Now single match is shifted by one
				uint8_t t = -this -> g_num_match -1;
				//check that the dictionary holds the same value as data
				if (this -> g_cmd_txt[t][ this -> g_cmd_index[t] ] == data)
				{
					//Match! Scan next entry
					this -> g_cmd_index[t]++;
					DPRINT("%d | Match still valid...\n", __LINE__);
				}
				//no match
				else
				{
					//Last partial match has been pruned away. Issue a FSM reset
					f_rst_fsm = true;
					DPRINT("%d | Last match was pruned away. Expected >0x%x< got >0x%x< instead\n", __LINE__, this -> g_cmd_txt[t][ this -> g_cmd_index[t] ], data);
				}
			}	//If I only have one match
		}	//end if: I'm matching a non argument non terminator
	}	//end if: I'm ID matching

		//--------------------------------------------------------------------------
		//	ARG decoder
		//--------------------------------------------------------------------------

	//if: I'm decoding arguments
	else if (this -> g_status == Parser_status::PARSER_ARG)
	{
		DPRINT("%d | PARSER_ARG\n", __LINE__);
		//If: I'm fed a number
		if (IS_NUMBER(data))
		{
			//!	accumulate argument character inside argument.
			bool f_ret = this -> accumulate_arg( data );
			//if: adding argument failed
			if ((UNIPARSER_PENDANTIC_CHECKS) && (f_ret == true))
			{
				this -> g_err = Err_codes::ERR_GENERIC;
				DPRINT("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
				//I can recover from this by resetting the FSM
				f_rst_fsm = true;
			}
			//Do not advance to next dictionary entry
		}	//If: I'm fed a number
		//if: I'm fed a non number
		else
		{
			DPRINT("Closing argument\n");
			//! Exit argument mode
			//Close current argument and update argument FSM
			this -> close_arg();
			//BUGFIX: If the only match is also entry 0
			//if: I have multiple matches. I should have just one match by this point.
			if ((UNIPARSER_PENDANTIC_CHECKS) && (this -> g_num_match > 0))
			{
				this -> g_err = Err_codes::ERR_GENERIC;
				DPRINT("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
				//I can recover from this by resetting the FSM
				f_rst_fsm = true;
			}
			else
			{
				//After argument, I can only have one match. | BUGFIX: Now single match is shifted by one
				uint8_t cmd_id = -this -> g_num_match -1;
				//Update the parser index by skipping % and the argument descriptor
				this -> g_cmd_index[ cmd_id ] += 2;
				//check that the dictionary holds the same value as data
				if (this -> g_cmd_txt[ cmd_id ][ this -> g_cmd_index[ cmd_id ] ] == data)
				{
					//Advance to the next dictionary entry for this command
					this -> g_cmd_index[ cmd_id ]++;
				}
				//No match
				else
				{
					DPRINT("%d | Pruning away last match\n", __LINE__);
					//I can recover from this by resetting the FSM
					f_rst_fsm = true;
				}
				this -> g_status = Parser_status::PARSER_ID;
			}
		}	//if: I'm fed a non number
	}	//if: I'm decoding arguments

		//----------------------------------------------------------------
		//	FSM RESET
		//----------------------------------------------------------------
		//!	@detail
		//! FSM reset. Clear up the machine for the next clean execution

	//If: a reset was issued
	if (f_rst_fsm == true)
	{
		DPRINT("%d | FSM RESET\n", __LINE__);
		//Clear reset flag
		f_rst_fsm = false;
		//Status becomes IDLE
		this -> g_status = Orangebot::Parser_status::PARSER_IDLE;
		//I have no partial matches anymore
		this -> g_num_match = 0;
		//If I don't have a pending execution
		if (exe_index == -1)
		{
			//Reset the argument decoder and prepare for a new command
			this -> init_arg_decoder();
		}
	}	//If: a reset was issued

		//----------------------------------------------------------------
		//	HANDLER EXECUTION
		//----------------------------------------------------------------
		//!	@detail
		//! FSM reset. Clear up the machine for the next clean execution

	//If: an execution event has been launched
	if (exe_index > -1)
	{
		//if execution index is out of range.
		if ((UNIPARSER_PENDANTIC_CHECKS) && (exe_index == this -> g_num_cmd))
		{
			this -> g_err = Err_codes::ERR_GENERIC;
			DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
			return true;	//fail
		}
		DPRINT("%d | Executing handler of command %d | num arguments: %d\n", __LINE__, exe_index, this -> g_arg_fsm_status.num_arg);
		//Execute handler of given function. Automatically deduce arguments from argument vector
        this -> execute_callback( this ->g_cmd_handler[exe_index] );
        //Reset the argument decoder and prepare for a new command
		this -> init_arg_decoder();
	}	//If: a reset was issued

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Trace Return from main
	DRETURN_ARG("Err_code: %d\n", this -> g_err);
	//Inform the caller that an error occurred
	return (this -> g_err != Err_codes::NO_ERR);
}	//end method: parse | char

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
//! initialize structure
/***************************************************************************/

inline void Uniparser::init( void )
{
	DENTER();

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//counter
	uint8_t t;

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------
	//! @details initialize structure

	//No commands are currently loaded inside the parser
	this -> g_num_cmd = 0;
	//for: each possible command
	for (t = 0;t < UNIPARSER_MAX_CMD;t++)
	{
		//command has no txt identifier linked
		this -> g_cmd_txt[t] = nullptr;
		//command has no function handler linked
		this -> g_cmd_handler[t] = nullptr;
	}
	//I have no partial matches
	this -> g_num_match = 0;
	//FSM begins in idle
	this -> g_status = Orangebot::Parser_status::PARSER_IDLE;
	//No error
	this -> g_err = Orangebot::Err_codes::NO_ERR;

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Trace Return from main
	DRETURN();

	return;	//OK
}	//end method: init | void

/***************************************************************************/
//!	@brief Private Method
//!	init_arg_decoder | void
/***************************************************************************/
//!	@details
//! initialize argument decoder for a new command
/***************************************************************************/

inline void Uniparser::init_arg_decoder( void )
{
	//Trace Enter with arguments
	DENTER();

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------
	//! @details algorithm:

	//Prepare the argument descriptor. zero arguments are in store
	this -> g_arg_fsm_status.num_arg = 0;
	//First free slot in the argument vector is the first byte
	this -> g_arg_fsm_status.arg_index = 0;
	//if sign is not specified, default argument sign is plus
	this -> g_arg_fsm_status.arg_sign = false;

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Trace Return vith return value
	DRETURN();

	return; //OK
}	//end method: init_arg_decoder | void

/***************************************************************************/
//!	@brief Private Method
//!	chk_cmd | const uint8_t *
/***************************************************************************/
//! @param cmd |
//! @return syntax error | bool | false: OK | true: error detected. Code handled by the error handler
//!	@details
//! Check command syntax to prevent bad commands from being loaded in the dictionary
//! Saves runtime check for bad dictionary entries
/***************************************************************************/

bool Uniparser::chk_cmd( const uint8_t *cmd )
{
	//Trace Enter with arguments
	DENTER_ARG("cmd: %p\n", (void *)cmd);

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//Counter
	uint8_t t;
	//Total argument size used
	uint8_t arg_cnt = 0;
	//Number of arguments
	uint8_t arg_type_cnt = 0;

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//If a null pointer function handler was given
	if (cmd == nullptr)
	{
		this -> error_handler( Err_codes::SYNTAX_BAD_POINTER );
		DRETURN_ARG("ERR%d: | Bad handler function pointer\n", Err_codes::SYNTAX_BAD_POINTER);
		return true; //FAIL
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//if: first char is not a letter
	if (!IS_LETTER( cmd[0] ))
	{
		this -> error_handler( Err_codes::SYNTAX_FIRST_NOLETTER );
		DRETURN_ARG("ERR%d | First character of a command must be a letter\n", Err_codes::SYNTAX_FIRST_NOLETTER);
		return true; //FAIL
	}

	//Initialize counter
	t = 1;
	//While: parsing is not done
	while ((cmd[t] != '\0') && (t < UNIPARSER_MAX_CMD_LENGTH))
	{
		//if: argument descriptor
		if (cmd[t] == '%')
		{
				//!Check number of arguments
			//i have an argument descriptor
			arg_type_cnt++;
			//if: exceed number of arguments
			if (arg_type_cnt > UNIPARSER_MAX_ARGS)
			{
				this -> error_handler( Err_codes::SYNTAX_TOO_MANY_ARGS );
				DRETURN_ARG("ERR%d in line %d | too many arguments in this command %d\n", Err_codes::SYNTAX_TOO_MANY_ARGS, __LINE__, arg_type_cnt);
				return true; //FAIL
			}
				//!Check back to back arguments
			//if: two arguments back to back
			if ( (t >= 2) && (cmd[t -2] == '%') )
			{
				this -> error_handler( Err_codes::SYNTAX_ARG_BACKTOBACK );
				DRETURN_ARG("ERR%d in line %d | Two back to back arguments were given. Add a character in between them\n", Err_codes::SYNTAX_ARG_BACKTOBACK, __LINE__);
				return true;	//FAIL
			}

				//!Check total size of arguments
			//Skip the argument descriptor
			t++;
			//Switch: argument type
			switch( cmd[t] )
			{
				//Decode type
				case Arg_type::ARG_U8:
				{
                    //Accumulate
                    arg_cnt += Arg_size::ARG_U8_SIZE;
					break;
				}
				//Decode type
				case Arg_type::ARG_S8:
				{
                    //Accumulate
                    arg_cnt += Arg_size::ARG_S8_SIZE;
					break;
				}
				//Decode type
				case Arg_type::ARG_U16:
				{
                    //Accumulate
                    arg_cnt += Arg_size::ARG_U16_SIZE;
					break;
				}
				//Decode type
				case Arg_type::ARG_S16:
				{
                    //Accumulate
                    arg_cnt += Arg_size::ARG_S16_SIZE;
					break;
				}
				//Decode type
				case Arg_type::ARG_U32:
				{
                    //Accumulate
                    arg_cnt += Arg_size::ARG_U32_SIZE;
					break;
				}
				//Decode type
				case Arg_type::ARG_S32:
				{
                    //Accumulate
                    arg_cnt += Arg_size::ARG_S32_SIZE;
					break;
				}
				default:
				{
					this -> error_handler( Err_codes::SYNTAX_ARG_TYPE_INVALID );
					DRETURN_ARG("ERR%d in line %d | Invalid argument descriptor %x\n", Err_codes::SYNTAX_ARG_TYPE_INVALID , __LINE__, cmd[t]);
					return true;	//FAIL
				}
			}	//End Switch: argument type
			//Check size
			if (arg_cnt > UNIPARSER_ARG_VECTOR_SIZE)
			{
				this -> error_handler( Err_codes::SYNTAX_ARG_LENGTH );
				DRETURN_ARG("ERR%d in line %d | Invalid argument descriptor %x\n", Err_codes::SYNTAX_ARG_LENGTH , __LINE__, cmd[t]);
				return true;	//FAIL
			}
		}	//End if: argument descriptor
		//Parse next byte
		t++;
	}	//End While: parsing is not done
	//If command size has been exceeded
	if (t >= UNIPARSER_MAX_CMD_LENGTH)
	{
		this -> error_handler( Err_codes::SYNTAX_LENGTH );
		DRETURN_ARG("ERR%d in line %d | Given command was too long %d\n", Err_codes::SYNTAX_LENGTH , __LINE__, t);
		return true;	//FAIL
	}

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN();
	return false; //OK
}	//end method: chk_cmd | const uint8_t *

/***************************************************************************/
//!	@brief Private Method
//!	set_arg<uint8_t> | uint8_t
/***************************************************************************/
//! @param arg | T | numeric value of the argument to be recorded inside the general argument vector
//! @return bool | false: OK | true: argument exceed the alloted arg vector space
//!	@details
//! Template based add_arg method to add the numeric value of the argument to be recorded inside the general argument vector \n
//! Each method is specialized for each supported type, while leaving space for standard or custom types to be added \n
//! It is much cleaner than encoding the type inside the name of the function and takes advantage of C++ constructs
/***************************************************************************/

template <>
inline bool Uniparser::set_arg<uint8_t>( uint8_t arg_index, uint8_t arg )
{
	//Trace Enter
	DENTER_ARG( "argument: %d\n", arg );

	///--------------------------------------------------------------------------
	///	VARS
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	//If: add_arg would exceed the maximum allowed argument vector size
	if ((UNIPARSER_PENDANTIC_CHECKS) && (arg_index > UNIPARSER_ARG_VECTOR_SIZE -ARG_U8_SIZE))
	{
		//Trace Return
		DRETURN_ARG("ERR: add_arg would exceed the maximum allowed argument vector size | max: %d | current: %d\n", UNIPARSER_ARG_VECTOR_SIZE, arg_index);
		this -> error_handler( Err_codes::ERR_ARG_SIZE );
		return true;
	}

	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Save the numeric value of the argument
	this -> g_arg[ arg_index ] = (uint8_t)arg;

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	//Trace Return
	DRETURN_ARG("Index: %d | Binary: %3x\n", arg_index, (uint8_t)this -> g_arg[arg_index +0] );

	return false;	//OK
}	//end method: set_arg<uint8_t> | uint8_t

/***************************************************************************/
//!	@brief Public Method
//!	set_arg<uint8_t> | uint8_t
/***************************************************************************/
//! @param arg | T | numeric value of the argument to be recorded inside the general argument vector
//! @return bool | false: OK | true: argument exceed the alloted arg vector space
//!	@details
//! Template based set_arg method to add the numeric value of the argument to be recorded inside the general argument vector \n
//! Each method is specialized for each supported type, while leaving space for standard or custom types to be added \n
//! It is much cleaner than encoding the type inside the name of the function and takes advantage of C++ constructs
/***************************************************************************/

template <>
inline bool Uniparser::set_arg<int8_t>( uint8_t arg_index, int8_t arg )
{
	//Trace Enter
	DENTER_ARG( "argument: %d\n", arg );

	///--------------------------------------------------------------------------
	///	VARS
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	//If: add_arg would exceed the maximum allowed argument vector size
	if ((UNIPARSER_PENDANTIC_CHECKS) && (arg_index > UNIPARSER_ARG_VECTOR_SIZE -ARG_S8_SIZE))
	{
		//Trace Return
		DRETURN_ARG("ERR: add_arg would exceed the maximum allowed argument vector size | max: %d | current: %d\n", UNIPARSER_ARG_VECTOR_SIZE, arg_index);
		this -> error_handler( Err_codes::ERR_ARG_SIZE );
		return true;
	}

	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Link pointer to the correct data type
	int8_t *arg_ptr = ARG_CAST( arg_index, int8_t );
	//Save the numeric value of the argument
	*arg_ptr = (int8_t)arg;

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	//Trace Return
	DRETURN_ARG("Index: %d | Binary: %3x\n", arg_index, (uint8_t)this -> g_arg[arg_index +0] );

	return false;	//OK
}	//end method: set_arg<int8_t> | int8_t

/***************************************************************************/
//!	@brief Public Method
//!	set_arg<uint16_t> | uint16_t
/***************************************************************************/
//! @param arg | T | numeric value of the argument to be recorded inside the general argument vector
//! @return bool | false: OK | true: argument exceed the alloted arg vector space
//!	@details
//! Template based add_arg method to add the numeric value of the argument to be recorded inside the general argument vector \n
//! Each method is specialized for each supported type, while leaving space for standard or custom types to be added \n
//! It is much cleaner than encoding the type inside the name of the function and takes advantage of C++ constructs
/***************************************************************************/

template <>
inline bool Uniparser::set_arg<uint16_t>( uint8_t arg_index, uint16_t arg )
{
	//Trace Enter
	DENTER_ARG( "argument: %d\n", arg );

	///--------------------------------------------------------------------------
	///	VARS
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	//If: add_arg would exceed the maximum allowed argument vector size
	if ((UNIPARSER_PENDANTIC_CHECKS) && (arg_index > UNIPARSER_ARG_VECTOR_SIZE -ARG_U16_SIZE))
	{
		//Trace Return
		DRETURN_ARG("ERR: add_arg would exceed the maximum allowed argument vector size | max: %d | current: %d\n", UNIPARSER_ARG_VECTOR_SIZE, arg_index);
		this -> error_handler( Err_codes::ERR_ARG_SIZE );
		return true;
	}

	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Link pointer to the correct data type
	uint16_t *arg_ptr = ARG_CAST( arg_index, uint16_t );
	//Save the numeric value of the argument
	*arg_ptr = (uint16_t)arg;

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	//Trace Return
	DRETURN_ARG("Index: %d | Binary: %3x %3x\n", arg_index, (uint8_t)this -> g_arg[arg_index +0], (uint8_t)this -> g_arg[arg_index +1] );

	return false;	//OK
}	//end method: set_arg<uint16_t> | uint16_t

/***************************************************************************/
//!	@brief Public Method
//!	set_arg<int16_t> | int16_t
/***************************************************************************/
//! @param arg | T | numeric value of the argument to be recorded inside the general argument vector
//! @return bool | false: OK | true: argument exceed the alloted arg vector space
//!	@details
//! Template based add_arg method to add the numeric value of the argument to be recorded inside the general argument vector \n
//! Each method is specialized for each supported type, while leaving space for standard or custom types to be added \n
//! It is much cleaner than encoding the type inside the name of the function and takes advantage of C++ constructs
/***************************************************************************/

template <>
inline bool Uniparser::set_arg<int16_t>( uint8_t arg_index, int16_t arg )
{
	//Trace Enter
	DENTER_ARG( "argument: %d\n", arg );

	///--------------------------------------------------------------------------
	///	VARS
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	//If: set_arg would exceed the maximum allowed argument vector size
	if ((UNIPARSER_PENDANTIC_CHECKS) && (arg_index > UNIPARSER_ARG_VECTOR_SIZE -ARG_S16_SIZE))
	{
		//Trace Return
		DRETURN_ARG("ERR: set_arg would exceed the maximum allowed argument vector size | max: %d | current: %d\n", UNIPARSER_ARG_VECTOR_SIZE, arg_index);
		this -> error_handler( Err_codes::ERR_ARG_SIZE );
		return true;
	}

	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Link pointer to the correct data type
	int16_t *arg_ptr = ARG_CAST( arg_index, int16_t );
	//Save the numeric value of the argument
	*arg_ptr = (int16_t)arg;

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	//Trace Return
	DRETURN_ARG("Index: %d | Binary: %3x %3x\n", arg_index, (uint8_t)this -> g_arg[arg_index +0], (uint8_t)this -> g_arg[arg_index +1] );

	return false;	//OK
}	//end method: set_arg<int16_t> | int16_t

/***************************************************************************/
//!	@brief Public Method
//!	set_arg<uint32_t> | uint32_t
/***************************************************************************/
//! @param arg | T | numeric value of the argument to be recorded inside the general argument vector
//! @return bool | false: OK | true: argument exceed the alloted arg vector space
//!	@details
//! Template based set_arg method to add the numeric value of the argument to be recorded inside the general argument vector \n
//! Each method is specialized for each supported type, while leaving space for standard or custom types to be added \n
//! It is much cleaner than encoding the type inside the name of the function and takes advantage of C++ constructs
/***************************************************************************/

template <>
inline bool Uniparser::set_arg<uint32_t>( uint8_t arg_index, uint32_t arg )
{
	//Trace Enter
	DENTER_ARG( "argument: %d\n", arg );

	///--------------------------------------------------------------------------
	///	VARS
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	//If: add_arg would exceed the maximum allowed argument vector size
	if ((UNIPARSER_PENDANTIC_CHECKS) && (arg_index > UNIPARSER_ARG_VECTOR_SIZE -ARG_U32_SIZE))
	{
		//Trace Return
		DRETURN_ARG("ERR: set_arg would exceed the maximum allowed argument vector size | max: %d | current: %d\n", UNIPARSER_ARG_VECTOR_SIZE, arg_index);
		this -> error_handler( Err_codes::ERR_ARG_SIZE );
		return true;
	}

	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Link pointer to the correct data type
	uint32_t *arg_ptr = ARG_CAST( arg_index, uint32_t );
	//Save the numeric value of the argument
	*arg_ptr = (uint32_t)arg;

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	//Trace Return
	DRETURN_ARG("Index: %d | Binary: %3x %3x %3x %3x\n", arg_index, (uint8_t)this -> g_arg[arg_index +0], (uint8_t)this -> g_arg[arg_index +1], (uint8_t)this -> g_arg[arg_index +2], (uint8_t)this -> g_arg[arg_index +3] );

	return false;	//OK
}	//end method: set_arg<int32_t> | int32_t

/***************************************************************************/
//!	@brief Public Method
//!	set_arg<int32_t> | int32_t
/***************************************************************************/
//! @param arg | T | numeric value of the argument to be recorded inside the general argument vector
//! @return bool | false: OK | true: argument exceed the alloted arg vector space
//!	@details
//! Template based set_arg method to add the numeric value of the argument to be recorded inside the general argument vector \n
//! Each method is specialized for each supported type, while leaving space for standard or custom types to be added \n
//! It is much cleaner than encoding the type inside the name of the function and takes advantage of C++ constructs
/***************************************************************************/

template <>
inline bool Uniparser::set_arg<int32_t>( uint8_t arg_index, int32_t arg )
{
	//Trace Enter
	DENTER_ARG( "argument: %d\n", arg );

	///--------------------------------------------------------------------------
	///	VARS
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	//If: add_arg would exceed the maximum allowed argument vector size
	if ((UNIPARSER_PENDANTIC_CHECKS) && (arg_index > UNIPARSER_ARG_VECTOR_SIZE -ARG_S32_SIZE))
	{
		//Trace Return
		DRETURN_ARG("ERR: set_arg would exceed the maximum allowed argument vector size | max: %d | current: %d\n", UNIPARSER_ARG_VECTOR_SIZE, arg_index);
		this -> error_handler( Err_codes::ERR_ARG_SIZE );
		return true;
	}

	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//Link pointer to the correct data type
	int32_t *arg_ptr = ARG_CAST( arg_index, int32_t );
	//Save the numeric value of the argument
	*arg_ptr = (int32_t)arg;

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	//Trace Return
	DRETURN_ARG("Index: %d | Binary: %3x %3x %3x %3x\n", arg_index, (uint8_t)this -> g_arg[arg_index +0], (uint8_t)this -> g_arg[arg_index +1], (uint8_t)this -> g_arg[arg_index +2], (uint8_t)this -> g_arg[arg_index +3] );

	return false;	//OK
}	//end method: set_arg<int32_t> | int32_t

/***************************************************************************/
//!	@brief Private Method
//!	get_arg<uint8_t> | uint8_t
/***************************************************************************/
//! @param arg_index | point to an argument inside the argument vector
//! @return decoded content of argument vector at given position
//!	@details
//! fetch the numeric value of an argument from the argument vector
/***************************************************************************/

template <>
inline uint8_t Uniparser::get_arg( uint8_t arg_index )
{
	DENTER_ARG("arg_index: %d\n", arg_index );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//If: add_arg would exceed the maximum allowed argument vector size
	if ((UNIPARSER_PENDANTIC_CHECKS) && (arg_index > UNIPARSER_ARG_VECTOR_SIZE -ARG_U8_SIZE))
	{
		//Trace Return
		DRETURN_ARG("ERR: add_arg would exceed the maximum allowed argument vector size | max: %d | current: %d\n", UNIPARSER_ARG_VECTOR_SIZE, arg_index);
		this -> error_handler( Err_codes::ERR_ARG_SIZE );
		return (uint8_t)0xff;
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Link the pointer to the actual content
	uint8_t *arg_ptr = &this -> g_arg[arg_index];

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN_ARG("Decoded: %d\n", *arg_ptr);
	return *arg_ptr; //OK
}	//end method: get_arg<uint8_t> | uint8_t

/***************************************************************************/
//!	@brief Private Method
//!	get_arg<int8_t> | uint8_t
/***************************************************************************/
//! @param arg_index | point to an argument inside the argument vector
//! @return decoded content of argument vector at given position
//!	@details
//! fetch the numeric value of an argument from the argument vector
/***************************************************************************/

template <>
inline int8_t Uniparser::get_arg( uint8_t arg_index )
{
	DENTER_ARG("arg_index: %d\n", arg_index );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//If: add_arg would exceed the maximum allowed argument vector size
	if ((UNIPARSER_PENDANTIC_CHECKS) && (arg_index > UNIPARSER_ARG_VECTOR_SIZE -ARG_S8_SIZE))
	{
		//Trace Return
		DRETURN_ARG("ERR: add_arg would exceed the maximum allowed argument vector size | max: %d | current: %d\n", UNIPARSER_ARG_VECTOR_SIZE, arg_index);
		this -> error_handler( Err_codes::ERR_ARG_SIZE );
		return (uint8_t)0xff;
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Link the pointer to the actual content
	int8_t *arg_ptr = ARG_CAST( arg_index, int8_t);

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN_ARG("Decoded: %d\n", *arg_ptr);
	return *arg_ptr; //OK
}	//end method: get_arg<int8_t> | uint8_t

/***************************************************************************/
//!	@brief Private Method
//!	get_arg<uint16_t> | uint8_t
/***************************************************************************/
//! @param arg_index | point to an argument inside the argument vector
//! @return decoded content of argument vector at given position
//!	@details
//! fetch the numeric value of an argument from the argument vector
/***************************************************************************/

template <>
inline uint16_t Uniparser::get_arg( uint8_t arg_index )
{
	DENTER_ARG("arg_index: %d\n", arg_index );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//If: add_arg would exceed the maximum allowed argument vector size
	if ((UNIPARSER_PENDANTIC_CHECKS) && (arg_index > UNIPARSER_ARG_VECTOR_SIZE -ARG_U16_SIZE))
	{
		//Trace Return
		DRETURN_ARG("ERR: add_arg would exceed the maximum allowed argument vector size | max: %d | current: %d\n", UNIPARSER_ARG_VECTOR_SIZE, arg_index);
		this -> error_handler( Err_codes::ERR_ARG_SIZE );
		return (uint8_t)0xff;
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Link the pointer to the actual content
	uint16_t *arg_ptr = ARG_CAST( arg_index, uint16_t);

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN_ARG("Decoded: %d\n", *arg_ptr);
	return *arg_ptr; //OK
}	//end method: get_arg<uint16_t> | uint8_t

/***************************************************************************/
//!	@brief Private Method
//!	get_arg<int16_t> | uint8_t
/***************************************************************************/
//! @param arg_index | point to an argument inside the argument vector
//! @return decoded content of argument vector at given position
//!	@details
//! fetch the numeric value of an argument from the argument vector
/***************************************************************************/

template <>
inline int16_t Uniparser::get_arg( uint8_t arg_index )
{
	DENTER_ARG("arg_index: %d\n", arg_index );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//If: add_arg would exceed the maximum allowed argument vector size
	if ((UNIPARSER_PENDANTIC_CHECKS) && (arg_index > UNIPARSER_ARG_VECTOR_SIZE -ARG_S16_SIZE))
	{
		//Trace Return
		DRETURN_ARG("ERR: add_arg would exceed the maximum allowed argument vector size | max: %d | current: %d\n", UNIPARSER_ARG_VECTOR_SIZE, arg_index);
		this -> error_handler( Err_codes::ERR_ARG_SIZE );
		return (uint8_t)0xff;
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Link the pointer to the actual content
	int16_t *arg_ptr = ARG_CAST( arg_index, int16_t);

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN_ARG("Decoded: %d\n", *arg_ptr);
	return *arg_ptr; //OK
}	//end method: get_arg<int16_t> | uint8_t

/***************************************************************************/
//!	@brief Private Method
//!	get_arg<uint32_t> | uint8_t
/***************************************************************************/
//! @param arg_index | point to an argument inside the argument vector
//! @return decoded content of argument vector at given position
//!	@details
//! fetch the numeric value of an argument from the argument vector
/***************************************************************************/

template <>
inline uint32_t Uniparser::get_arg( uint8_t arg_index )
{
	DENTER_ARG("arg_index: %d\n", arg_index );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//If: add_arg would exceed the maximum allowed argument vector size
	if ((UNIPARSER_PENDANTIC_CHECKS) && (arg_index > UNIPARSER_ARG_VECTOR_SIZE -ARG_U32_SIZE))
	{
		//Trace Return
		DRETURN_ARG("ERR: add_arg would exceed the maximum allowed argument vector size | max: %d | current: %d\n", UNIPARSER_ARG_VECTOR_SIZE, arg_index);
		this -> error_handler( Err_codes::ERR_ARG_SIZE );
		return (uint8_t)0xff;
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Link the pointer to the actual content
	uint32_t *arg_ptr = ARG_CAST( arg_index, uint32_t);

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN_ARG("Decoded: %d\n", *arg_ptr);
	return *arg_ptr; //OK
}	//end method: get_arg<uint32_t> | uint8_t

/***************************************************************************/
//!	@brief Private Method
//!	get_arg<int32_t> | uint8_t
/***************************************************************************/
//! @param arg_index | point to an argument inside the argument vector
//! @return decoded content of argument vector at given position
//!	@details
//! fetch the numeric value of an argument from the argument vector
/***************************************************************************/

template <>
inline int32_t Uniparser::get_arg( uint8_t arg_index )
{
	DENTER_ARG("arg_index: %d\n", arg_index );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//If: add_arg would exceed the maximum allowed argument vector size
	if ((UNIPARSER_PENDANTIC_CHECKS) && (arg_index > UNIPARSER_ARG_VECTOR_SIZE -ARG_S32_SIZE))
	{
		//Trace Return
		DRETURN_ARG("ERR: add_arg would exceed the maximum allowed argument vector size | max: %d | current: %d\n", UNIPARSER_ARG_VECTOR_SIZE, arg_index);
		this -> error_handler( Err_codes::ERR_ARG_SIZE );
		return (uint8_t)0xff;
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Link the pointer to the actual content
	int32_t *arg_ptr = ARG_CAST( arg_index, int32_t);

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	DRETURN_ARG("Decoded: %d\n", *arg_ptr);
	return *arg_ptr; //OK
}	//end method: get_arg<int32_t> | uint8_t

/***************************************************************************/
//!	@brief Private Method
//!	add_arg | uint8_t
/***************************************************************************/
//! @param cmd_id | index of the command the argument is inferred from
//! @return false: ok | true: fail
//!	@details
//! Add an argument to the parser argument storage.
//!	It expect the command to be a partial match and the index to be pointing to '%'
//! The command is added to the class argument storage string in the format
//!	'u' data0 ... data 1
/***************************************************************************/

bool Uniparser::add_arg( uint8_t cmd_id )
{
	//Trace Enter with arguments
	DENTER_ARG("command index: %d\n", cmd_id);

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//return
	bool f_ret = false;

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//If input index is out of range
	if ((UNIPARSER_PENDANTIC_CHECKS) && (cmd_id > this -> g_num_cmd) )
	{
		this -> g_err = Err_codes::ERR_GENERIC;
		DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
		return true;	//fail
	}
	//Fetch index inside the command
	uint8_t cmd_index = this -> g_cmd_index[ cmd_id ];
	//if: the command is not an argument
	if ((UNIPARSER_PENDANTIC_CHECKS) && (this -> g_cmd_txt[cmd_id][ cmd_index ] != '%'))
	{
		this -> g_err = Err_codes::ERR_GENERIC;
		DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
		return true;	//fail
	}
	//Point to the argument type
    cmd_index++;
    //if: the command is not an argument descriptor. PEDANTIC because dictionary should have been checked before hand
	if ((UNIPARSER_PENDANTIC_CHECKS) && (!IS_ARG_DESCRIPTOR(this -> g_cmd_txt[cmd_id][ cmd_index ])) )
	{
		this -> g_err = Err_codes::ERR_GENERIC;
		DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
		return true;	//fail
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------
	//! @details algorithm:
	//! Compute index to argument vector and number of arguments
	//!	Compute
	//!	ARG Index points to first free slot in the argument vector
	//! I save argument type and initialize the content

		//! Initialize argument identifier
	//argument descriptor is held in the dictionary
	Arg_type arg_type = (Arg_type)this -> g_cmd_txt[cmd_id][cmd_index];
	//Fetch index to the next free argument slot
	uint8_t arg_type_index = this -> g_arg_fsm_status.num_arg;
	//Store argument type in the argument type vector
	this -> g_arg_type[ arg_type_index ] = arg_type;
	//A new argument is being added. Reset the argument sign (@TODO: do this earlier)
	this -> g_arg_fsm_status.arg_sign = false;
	//fetch argument index
	uint8_t arg_index = this -> g_arg_fsm_status.arg_index;
	DPRINT("argument of type >%c< added | Num arguments: %d | Arg index: %d | Arg sign %c \n", arg_type, this -> g_arg_fsm_status.num_arg, arg_index, (this -> g_arg_fsm_status.arg_sign)?('-'):('+') );

		//! Initialize argument content
	//switch: decode argument desriptor
	switch (arg_type)
	{
		case Arg_type::ARG_U8:
		{
			//Write zero inside the argument
			f_ret = this -> set_arg<uint8_t>( arg_index, (uint8_t)0 );
			break;
		}
		case Arg_type::ARG_S8:
		{
			//Write zero inside the argument
			f_ret = this -> set_arg<int8_t>( arg_index, (int8_t)0 );
			break;
		}
		case Arg_type::ARG_U16:
		{
			//Write zero inside the argument
			f_ret = this -> set_arg<uint16_t>( arg_index, (uint16_t)0 );
			break;
		}
		case Arg_type::ARG_S16:
		{
			//Write zero inside the argument
			f_ret = this -> set_arg<int16_t>( arg_index, (int16_t)0 );
			break;
		}
		case Arg_type::ARG_U32:
		{
			//Write zero inside the argument
			f_ret = this -> set_arg<uint32_t>( arg_index, (uint32_t)0 );
			break;
		}
		case Arg_type::ARG_S32:
		{
			//Write zero inside the argument
			f_ret = this -> set_arg<int32_t>( arg_index, (int32_t)0 );
			break;
		}
		//if: type is not handled
		default:
		{
			this -> error_handler( Err_codes::ERR_UNHANDLED_ARG_TYPE );
			DRETURN_ARG("ERR_UNHANDLED_ARG_TYPE in line: %d | num arg: %d\n", __LINE__, arg_type_index);
			return true; //FAIL
		}
	}	//end switch: decode argument desriptor

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Trace Return vith return value
	DRETURN_ARG("Success: %x\n", f_ret);

	return f_ret; //OK
}	//end method: add_arg | uint8_t

/***************************************************************************/
//!	@brief Private Method
//!	accumulate_arg | uint8_t
/***************************************************************************/
//! @param data | input character to be decoded. It should be a number or a sign
//! @return false: ok | true: fail
//!	@details
//! decode an input character and accumulate it into the argument vector
/***************************************************************************/

bool Uniparser::accumulate_arg( uint8_t data )
{
	//Trace Enter with arguments
	DENTER_ARG("data >%c<\n", data);

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//detect failure state
	bool f_ret;

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//Fetch argument index
	uint8_t arg_index = this -> g_arg_fsm_status.arg_index;
	//Fetch argument type index
	uint8_t arg_type_index = this -> g_arg_fsm_status.num_arg;
	//If argument descriptor is bad
	if ((UNIPARSER_PENDANTIC_CHECKS) && (arg_type_index >= UNIPARSER_MAX_ARGS))
	{
		this -> g_err = Err_codes::ERR_GENERIC;
		DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d | Num args: %x\n", this -> g_err, __LINE__, arg_type_index );
		return true;	//fail
	}
	//Fetch argument type
	Arg_type arg_type = (Arg_type)this->g_arg_type[arg_type_index];
	//If argument descriptor is bad
	if ((UNIPARSER_PENDANTIC_CHECKS) && (!IS_ARG_DESCRIPTOR(arg_type)))
	{
		this -> g_err = Err_codes::ERR_GENERIC;
		DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d | Arg_type: %x\n", this -> g_err, __LINE__, arg_type );
		return true;	//fail
	}
	//If: input char is bad
	if ((UNIPARSER_PENDANTIC_CHECKS) && (!IS_SIGN(data)) && (!IS_NUMBER(data)))
	{
		DPRINT("ERR: bad input char. Expecting number or sign and got >0x%x< instead\n", data);
		this -> g_err = Err_codes::ERR_GENERIC;
		DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
		return true;	//fail
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

		//!Detect sign
	//if sign
	if (IS_SIGN(data))
	{
		//If sign is minus, argument sign is true which means minus
		this -> g_arg_fsm_status.arg_sign = (data == '-');
		DRETURN();
		//I'm done updating argument for now. Next round will come actual numbers.
		return false;
	}
		//!Decode numeric value


	//decode the argument descriptor
	switch (arg_type)
	{
		//if: the command expects a uint8_t data
		case (Arg_type::ARG_U8):
		{
			//Fetch old argument
			uint8_t old = this -> get_arg<uint8_t>( arg_index );
			//Check runtime overflow
			if (old > 25)
			{
				return true;
			}
			//Shift by one digit left
			old *= 10;
			//If number is positive
			if (this -> g_arg_fsm_status.arg_sign == false)
			{
				//Accumulate new digit
				old += data -'0';
			}
			//if: number is negative
			else
			{
				//Accumulate new digit
				old -= data -'0';
			}
			//Write back argument inside argument vector
			f_ret = this -> set_arg<uint8_t>( arg_index, old );
			//If set arg failed
			if (f_ret == true)
			{
				return true;
			}
			DPRINT("Updated argument with value: %d | index: %d\n", old, arg_index);
			break;
		}
		//if: the command expects a uint8_t data
		case (Arg_type::ARG_S8):
		{
			//Fetch old argument
			int8_t old = this -> get_arg<int8_t>( arg_index );
			//Check runtime overflow
			if ((old > 12) || (old < -12))
			{
				return true;
			}
			//Shift by one digit left
			old *= 10;
			//If number is positive
			if (this -> g_arg_fsm_status.arg_sign == false)
			{
				//Accumulate new digit
				old += data -'0';
			}
			//if: number is negative
			else
			{
				//Accumulate new digit
				old -= data -'0';
			}
			//Write back argument inside argument vector
			f_ret = this -> set_arg<int8_t>( arg_index, old );
			//If set arg failed
			if (f_ret == true)
			{
				return true;
			}
			DPRINT("Updated argument with value: %d | index: %d\n", old, arg_index);
			break;
		}
		//if: the command expects a uint8_t data
		case (Arg_type::ARG_U16):
		{
			//Fetch old argument
			uint16_t old = this -> get_arg<uint16_t>( arg_index );
			//Check runtime overflow
			if (old > 6553)
			{
				return true;
			}
			//Shift by one digit left
			old *= 10;
			//If number is positive
			if (this -> g_arg_fsm_status.arg_sign == false)
			{
				//Accumulate new digit
				old += data -'0';
			}
			//if: number is negative
			else
			{

				//Accumulate new digit
				old -= data -'0';
			}
			//Write back argument inside argument vector
			f_ret = this -> set_arg<uint16_t>( arg_index, old );
			//If set arg failed
			if (f_ret == true)
			{
				return true;
			}
			DPRINT("Updated argument with value: %d | index: %d\n", old, arg_index);
			break;
		}
		//if: the command expects a uint8_t data
		case (Arg_type::ARG_S16):
		{
			//Fetch old argument
			int16_t old = this -> get_arg<int16_t>( arg_index );
			//Check runtime overflow
			if ((old > 3276) || (old < -3276))
			{
				return true;
			}
			//Shift by one digit left
			old *= 10;
			//If number is positive
			if (this -> g_arg_fsm_status.arg_sign == false)
			{
				//Accumulate new digit
				old += data -'0';
			}
			//if: number is negative
			else
			{

				//Accumulate new digit
				old -= data -'0';
			}
			//Write back argument inside argument vector
			f_ret = this -> set_arg<int16_t>( arg_index, old );
			//If set arg failed
			if (f_ret == true)
			{
				return true;
			}
			DPRINT("Updated argument with value: %d | index: %d\n", old, arg_index);
			break;
		}
		//if: the command expects a uint8_t data
		case (Arg_type::ARG_U32):
		{
			//Fetch old argument
			uint32_t old = this -> get_arg<uint32_t>( arg_index );
			//Check runtime overflow
			if (old > 429496729)
			{
				return true;
			}
			//Shift by one digit left
			old *= 10;
			//If number is positive
			if (this -> g_arg_fsm_status.arg_sign == false)
			{
				//Accumulate new digit
				old += data -'0';
			}
			//if: number is negative
			else
			{

				//Accumulate new digit
				old -= data -'0';
			}
			//Write back argument inside argument vector
			f_ret = this -> set_arg<uint32_t>( arg_index, old );
			//If set arg failed
			if (f_ret == true)
			{
				return true;
			}
			DPRINT("Updated argument with value: %d | index: %d\n", old, arg_index);
			break;
		}
		//if: the command expects a uint8_t data
		case (Arg_type::ARG_S32):
		{
			//Fetch old argument
			int32_t old = this -> get_arg<int32_t>( arg_index );
			//Check runtime overflow
			if ((old > 214748364) || (old < -214748364))
			{
				return true;
			}
			//Shift by one digit left
			old *= 10;
			//If number is positive
			if (this -> g_arg_fsm_status.arg_sign == false)
			{
				//Accumulate new digit
				old += data -'0';
			}
			//if: number is negative
			else
			{

				//Accumulate new digit
				old -= data -'0';
			}
			//Write back argument inside argument vector
			f_ret = this -> set_arg<int32_t>( arg_index, old );
			//If set arg failed
			if (f_ret == true)
			{
				return true;
			}
			DPRINT("Updated argument with value: %d | index: %d\n", old, arg_index);
			break;
		}
		//Unrecognized argument descriptor in the dictionary
		default:
		{
			this -> g_err = Err_codes::ERR_GENERIC;
			DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
			return true;	//fail
		}
	}

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Trace Return vith return value
	DRETURN();

	return false; //OK
}	//end method: accumulate_arg | uint8_t

/***************************************************************************/
//!	@brief Private Method
//!	close_arg | void
/***************************************************************************/
//! @return false: OK | true: fail
//!	@details
//! Argument has been fully decoded into argument string. Update argument descriptor FSM.
/***************************************************************************/

bool Uniparser::close_arg( void )
{
	//Trace Enter with arguments
	DENTER();

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//Fetch argument index
	uint8_t arg_index = this -> g_arg_fsm_status.arg_index;
	//Fetch argument type index
	uint8_t arg_type_index = this -> g_arg_fsm_status.num_arg;
	//If argument descriptor is bad
	if ((UNIPARSER_PENDANTIC_CHECKS) && (arg_type_index >= UNIPARSER_MAX_ARGS))
	{
		this -> g_err = Err_codes::ERR_GENERIC;
		DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d | Num args: %x\n", this -> g_err, __LINE__, arg_type_index );
		return true;	//fail
	}
	//Fetch argument type
	Arg_type arg_type = (Arg_type)this->g_arg_type[arg_type_index];
	//If argument descriptor is bad
	if ((UNIPARSER_PENDANTIC_CHECKS) && (!IS_ARG_DESCRIPTOR(arg_type)))
	{
		this -> g_err = Err_codes::ERR_GENERIC;
		DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d | Arg_type: %x\n", this -> g_err, __LINE__, arg_type );
		return true;	//fail
	}

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

		//! Advance the argument index to the first free byte in the argument vector
	//If: one byte argument
	if ((arg_type == Arg_type::ARG_U8) || (arg_type == Arg_type::ARG_S8))
	{
		//Skip the argument descriptor and the argument itself
		arg_index += Arg_size::ARG_S8_SIZE;
	}
	//If: two bytes argument
	else if ((arg_type == Arg_type::ARG_U16) || (arg_type == Arg_type::ARG_S16))
	{
		//Skip the argument descriptor and the argument itself
		arg_index += Arg_size::ARG_S16_SIZE;
	}
	//If: four bytes argument
	else if ((arg_type == Arg_type::ARG_U32) || (arg_type == Arg_type::ARG_S32))
	{
		//Skip the argument descriptor and the argument itself
		arg_index += Arg_size::ARG_S32_SIZE;
	}
	//pedantic because the ditionary should have been checked before hand for bad argument descriptors
	else if (UNIPARSER_PENDANTIC_CHECKS)
	{
		//This error means the argument descriptor was unrecognized
		this -> g_err = Err_codes::ERR_GENERIC;
		DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
		return true;
	}
	else
	{
		//failure
		return true;
	}
		//! Check that index is valid
	//if: index is above bit width or exceed the argument vector size
	if ((arg_index > UNIPARSER_MAX_ARG_INDEX) || (arg_index > UNIPARSER_ARG_VECTOR_SIZE))
	{
		//Restart the argument decoder
		this -> init_arg_decoder();
		//
		DPRINT("ERR: Exceeded alloted argument vector size with index: %d\n", arg_index);
		this -> g_err = Err_codes::ERR_GENERIC;
		DRETURN_ARG("ERR%d: ERR_GENERIC in line: %d\n", this -> g_err, __LINE__ );
		return true;	//fail
	}
	//Write back index
	this -> g_arg_fsm_status.arg_index = arg_index;
	//Update number of decoded arguments
	this -> g_arg_fsm_status.num_arg++;
	DPRINT("Argument closed | num arg: %d | arg index: %d\n", this -> g_arg_fsm_status.num_arg, this -> g_arg_fsm_status.arg_index);

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Trace Return vith return value
	DRETURN();

	return false; //OK
}	//end method: close_arg | void

/***************************************************************************/
//!	@brief Public Method
//!	execute_callback | void
/***************************************************************************/
// @param
//! @return no return
//!	@details
//! Execute callback by passing a vector of U8 inside which are codified the bytes of the arguments
//! @algorithm
//! It's an execution tree that execute a branch based on the argument types
//! Tree is limited by the maximum number of types and the maximum number of bytes
//!	ARG0	ARG0,ARG1	ARG0,ARG1,ARG2	ARG0,ARG1,ARG2,ARG3
//! void
//! 8		8 8			8 8 8			8 8 8 8 (arg number limit)
//!						...				...
//! 		8 16
//!			8 32
//!	16		16 8
//!			16 16
//!			...
//! 32		32 8						32 8 8 8 (arg number limit)
//!										32 16 8 8 (arg number and size limit)
//!						32 16 16 (arg number limit)
//!			32 32 (arg size limit)
//!	(arg type limit)
//! Calls work equally with signed or unsiged arguments
/***************************************************************************/

bool Uniparser::execute_callback( void *callback_ptr )
{
	//Trace Enter
	DENTER();

	///--------------------------------------------------------------------------
	///	VARS
	///--------------------------------------------------------------------------

	//Argument index counter
	uint8_t arg_cnt = 0;
	//Number of arguments
	uint8_t arg_num_types = this -> g_arg_fsm_status.num_arg;
	//Count the index of the current argument type being decoded
	uint8_t arg_type_cnt = 0;
	//Link argument type vector
	Arg_type *arg_type = this -> g_arg_type;

	///--------------------------------------------------------------------------
	///	INIT
	///--------------------------------------------------------------------------

	///--------------------------------------------------------------------------
	///	BODY
	///--------------------------------------------------------------------------

	//if: no arguments
	if (arg_num_types == 0)
	{
		///--------------------------------------------------------------------------
		///	void
		///--------------------------------------------------------------------------

		//Declare specialized function pointer and link it to the general registered function callback address
		FUNCTION_PTR_VAR( f_ptr, void ) = FUNCTION_PTR_CAST(void)callback_ptr;
		//Execute function from its specialized pointer
		FUNCTION_PTR_EXE_VOID( f_ptr );
		DPRINT("Executing void -> void: %p | Args: NONE \n", (void *)f_ptr );
	}
	//if: first argument is U8 or S8
	else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
	{
		//Decode first argument
		uint8_t *arg_a_ptr = ARG_CAST( arg_cnt, uint8_t );
		//Accumulate argument size
		arg_cnt += Arg_size::ARG_U8_SIZE;
		//I decoded an argument
		arg_type_cnt++;
		//If: it's the only argument
		if (arg_num_types == arg_type_cnt)
		{
			///--------------------------------------------------------------------------
			///	U8 - 1
			///--------------------------------------------------------------------------

			//Declare specialized function pointer and link it to the general registered function callback address
			FUNCTION_PTR_VAR( f_ptr, uint8_t ) = FUNCTION_PTR_CAST( uint8_t )callback_ptr;
			//Execute function from its specialized pointer
			FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr) );
			DPRINT("Executing uint8_t -> void: %p | Args: %d \n", (void *)f_ptr, (*arg_a_ptr) );
		}	//End If: it's the only argument
		//if: second argument is U8 or S8
		else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
		{
			//Decode second argument
			uint8_t *arg_b_ptr = ARG_CAST( arg_cnt, uint8_t );
			//Accumulate argument size
			arg_cnt += Arg_size::ARG_U8_SIZE;
			//I decoded an argument
			arg_type_cnt++;
			//If: it's the only argument
			if (arg_num_types == arg_type_cnt)
			{
				///--------------------------------------------------------------------------
				///	U8 U8 - 2
				///--------------------------------------------------------------------------

				//Declare specialized function pointer and link it to the general registered function callback address
				FUNCTION_PTR_VAR( f_ptr, uint8_t, uint8_t ) = FUNCTION_PTR_CAST( uint8_t, uint8_t )callback_ptr;
				//Execute function from its specialized pointer
				FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr));
				DPRINT("Executing uint8_t, uint8_t -> void: %p | Args: %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr) );
			}	//End If: it's the only argument
			//if third argument is U8 or S8
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
			{
				//Decode second argument
				uint8_t *arg_c_ptr = ARG_CAST( arg_cnt, uint8_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U8_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U8 U8 U8 - 3
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint8_t, uint8_t, uint8_t ) = FUNCTION_PTR_CAST( uint8_t, uint8_t, uint8_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint8_t, uint8_t, uint8_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode second argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U8 U8 U8 - 4
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint8_t, uint8_t, uint8_t ) = FUNCTION_PTR_CAST( uint8_t, uint8_t, uint8_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint8_t, uint8_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
				{
					//Decode second argument
					uint16_t *arg_d_ptr = ARG_CAST( arg_cnt, uint16_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U16_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U8 U8 U16 - 5
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint8_t, uint8_t, uint16_t ) = FUNCTION_PTR_CAST( uint8_t, uint8_t, uint8_t, uint16_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint8_t, uint8_t, uint16_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U16 or S16
				//if fourth argument is U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					//Decode second argument
					uint32_t *arg_d_ptr = ARG_CAST( arg_cnt, uint32_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U32_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U8 U8 U32
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint8_t, uint8_t, uint32_t ) = FUNCTION_PTR_CAST( uint8_t, uint8_t, uint8_t, uint32_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint8_t, uint8_t, uint32_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U16 or S16
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U8 or S8
			//if third argument is U16 or S16
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
			{
				//Decode second argument
				uint16_t *arg_c_ptr = ARG_CAST( arg_cnt, uint16_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U16_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U8 U8 U16
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint8_t, uint8_t, uint16_t ) = FUNCTION_PTR_CAST( uint8_t, uint8_t, uint16_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint8_t, uint8_t, uint16_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode second argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U8 U16 U8 - 5
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint8_t, uint16_t, uint8_t ) = FUNCTION_PTR_CAST( uint8_t, uint8_t, uint16_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint8_t, uint16_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
				{
					//Decode second argument
					uint16_t *arg_d_ptr = ARG_CAST( arg_cnt, uint16_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U16_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U8 U16 U16 - 6
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint8_t, uint16_t, uint16_t ) = FUNCTION_PTR_CAST( uint8_t, uint8_t, uint16_t, uint16_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint8_t, uint16_t, uint16_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U16 or S16
				//if fourth argument is U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					//Decode second argument
					uint32_t *arg_d_ptr = ARG_CAST( arg_cnt, uint32_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U32_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U8 U16 U32 - 8
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint8_t, uint16_t, uint32_t ) = FUNCTION_PTR_CAST( uint8_t, uint8_t, uint16_t, uint32_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint8_t, uint16_t, uint32_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U16 or S16
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U16 or S16
			//if third argument is U32 or S32
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
			{
				//Decode second argument
				uint32_t *arg_c_ptr = ARG_CAST( arg_cnt, uint32_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U32_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U8 U8 U32 - 6
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint8_t, uint8_t, uint32_t ) = FUNCTION_PTR_CAST( uint8_t, uint8_t, uint32_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint8_t, uint8_t, uint32_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode second argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U8 U32 U8 - 7
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint8_t, uint32_t, uint8_t ) = FUNCTION_PTR_CAST( uint8_t, uint8_t, uint32_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint8_t, uint32_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
				{
					//Decode second argument
					uint16_t *arg_d_ptr = ARG_CAST( arg_cnt, uint16_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U16_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U8 U32 U16 - 8
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint8_t, uint32_t, uint16_t ) = FUNCTION_PTR_CAST( uint8_t, uint8_t, uint32_t, uint16_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint8_t, uint32_t, uint16_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U16 or S16
				//if fourth argument is U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument is U16 or S16
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U32 or S32
			//If: third argument is unhandled
			else
			{
				this -> error_handler( ERR_UNHANDLED_ARG_TYPE );
				DRETURN_ARG("ERR_UNHANDLED_ARG_TYPE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
				return true; //FAIL
			}	//End If: third argument is unhandled
		}	//if: second argument is U8 or S8
		//if: second argument is U16 or S16
		else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
		{
			//Decode second argument
			uint16_t *arg_b_ptr = ARG_CAST( arg_cnt, uint16_t );
			//Accumulate argument size
			arg_cnt += Arg_size::ARG_U16_SIZE;
			//I decoded an argument
			arg_type_cnt++;
			//If: it's the only argument
			if (arg_num_types == arg_type_cnt)
			{
				///--------------------------------------------------------------------------
				///	U8 U16 - 3
				///--------------------------------------------------------------------------

				//Declare specialized function pointer and link it to the general registered function callback address
				FUNCTION_PTR_VAR( f_ptr, uint8_t, uint16_t ) = FUNCTION_PTR_CAST( uint8_t, uint16_t )callback_ptr;
				//Execute function from its specialized pointer
				FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr));
				DPRINT("Executing uint8_t, uint16_t -> void: %p | Args: %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr) );
			}	//End If: it's the only argument
			//if third argument is U8 or S8
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
			{
				//Decode second argument
				uint8_t *arg_c_ptr = ARG_CAST( arg_cnt, uint8_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U8_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U8 U16 U8 - 4
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint8_t, uint16_t, uint8_t ) = FUNCTION_PTR_CAST( uint8_t, uint16_t, uint8_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint8_t, uint16_t, uint8_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode fourth argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U16 U8 U8 - 5
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint16_t, uint8_t, uint8_t ) = FUNCTION_PTR_CAST( uint8_t, uint16_t, uint8_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint16_t, uint8_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
				{
					//Decode fourth argument
					uint16_t *arg_d_ptr = ARG_CAST( arg_cnt, uint16_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U16_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U16 U8 U16 - 6
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint16_t, uint8_t, uint16_t ) = FUNCTION_PTR_CAST( uint8_t, uint16_t, uint8_t, uint16_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint16_t, uint8_t, uint16_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U16 or S16
				//if fourth argument is U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					//Decode fourth argument
					uint32_t *arg_d_ptr = ARG_CAST( arg_cnt, uint32_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U32_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U16 U8 U32 - 8
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint16_t, uint8_t, uint32_t ) = FUNCTION_PTR_CAST( uint8_t, uint16_t, uint8_t, uint32_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint16_t, uint8_t, uint32_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U16 or S16
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U8 or S8
			//if third argument is U16 or S16
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
			{
				//Decode second argument
				uint16_t *arg_c_ptr = ARG_CAST( arg_cnt, uint16_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U16_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U8 U16 U16
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint8_t, uint16_t, uint16_t ) = FUNCTION_PTR_CAST( uint8_t, uint16_t, uint16_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint8_t, uint16_t, uint16_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode second argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U16 U16 U8 - 6
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint16_t, uint16_t, uint8_t ) = FUNCTION_PTR_CAST( uint8_t, uint16_t, uint16_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint16_t, uint16_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
				{
					//Decode second argument
					uint16_t *arg_d_ptr = ARG_CAST( arg_cnt, uint16_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U16_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U16 U16 U16 - 7
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint16_t, uint16_t, uint16_t ) = FUNCTION_PTR_CAST( uint8_t, uint16_t, uint16_t, uint16_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint16_t, uint16_t, uint16_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U16 or S16
				//if fourth argument is U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument is U16 or S16
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U16 or S16
			//if third argument is U32 or S32
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
			{
				//Decode second argument
				uint32_t *arg_c_ptr = ARG_CAST( arg_cnt, uint32_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U32_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U8 U16 U32
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint8_t, uint16_t, uint32_t ) = FUNCTION_PTR_CAST( uint8_t, uint16_t, uint32_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint8_t, uint16_t, uint32_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode second argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U16 U32 U8 - 8
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint16_t, uint32_t, uint8_t ) = FUNCTION_PTR_CAST( uint8_t, uint16_t, uint32_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint16_t, uint32_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument is U16 or S16
				//if fourth argument is U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument is U16 or S16
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!

			}	//if: third argument is U32 or S32
			//If: third argument is unhandled
			else
			{
				this -> error_handler( ERR_UNHANDLED_ARG_TYPE );
				DRETURN_ARG("ERR_UNHANDLED_ARG_TYPE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
				return true; //FAIL
			}	//End If: third argument is unhandled
		}	//if: second argument is U16 or S16
		//if: second argument is U32 or S32
		else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
		{
			//Decode second argument
			uint32_t *arg_b_ptr = ARG_CAST( arg_cnt, uint32_t );
			//Accumulate argument size
			arg_cnt += Arg_size::ARG_U32_SIZE;
			//I decoded an argument
			arg_type_cnt++;
			//If: it's the only argument
			if (arg_num_types == arg_type_cnt)
			{
				///--------------------------------------------------------------------------
				///	U8 U32 - 5
				///--------------------------------------------------------------------------

				//Declare specialized function pointer and link it to the general registered function callback address
				FUNCTION_PTR_VAR( f_ptr, uint8_t, uint32_t ) = FUNCTION_PTR_CAST( uint8_t, uint32_t )callback_ptr;
				//Execute function from its specialized pointer
				FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr));
				DPRINT("Executing uint8_t, uint32_t -> void: %p | Args: %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr) );
			}	//End If: it's the only argument
			//if: third argument is U8 or S8
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
			{
				//Decode second argument
				uint8_t *arg_c_ptr = ARG_CAST( arg_cnt, uint8_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U8_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U8 U32 U8 - 6
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint8_t, uint32_t, uint8_t ) = FUNCTION_PTR_CAST( uint8_t, uint32_t, uint8_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint8_t, uint32_t, uint8_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode second argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U32 U8 U8 - 7
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint32_t, uint8_t, uint8_t ) = FUNCTION_PTR_CAST( uint8_t, uint32_t, uint8_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint32_t, uint8_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
				{
					//Decode second argument
					uint16_t *arg_d_ptr = ARG_CAST( arg_cnt, uint16_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U16_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U32 U8 U16 - 8
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint32_t, uint8_t, uint16_t ) = FUNCTION_PTR_CAST( uint8_t, uint32_t, uint8_t, uint16_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint32_t, uint8_t, uint16_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U16 or S16
				//if fourth argument is U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument is U16 or S16
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U8 or S8
			//if third argument is U16 or S16
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
			{
				//Decode second argument
				uint16_t *arg_c_ptr = ARG_CAST( arg_cnt, uint16_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U16_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U8 U32 U16 - 7
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint8_t, uint32_t, uint16_t ) = FUNCTION_PTR_CAST( uint8_t, uint32_t, uint16_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint8_t, uint32_t, uint16_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode fourth argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U8 U32 U16 U8 - 8
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint8_t, uint32_t, uint16_t, uint8_t ) = FUNCTION_PTR_CAST( uint8_t, uint32_t, uint16_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint8_t, uint32_t, uint16_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16 or U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16) || (arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument is U16 or S16 or U32 or S32
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U16 or S16
			//if third argument is U32 or S32
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
			{
				this -> error_handler( ERR_ARG_SIZE );
				DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
				return true; //FAIL
			}
			//If: third argument is unhandled
			else
			{
				this -> error_handler( ERR_UNHANDLED_ARG_TYPE );
				DRETURN_ARG("ERR_UNHANDLED_ARG_TYPE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
				return true; //FAIL
			}	//End If: third argument is unhandled		}
		}	//if: second argument is U32 or S32
		//If: second argument is unhandled
		else
		{
			this -> error_handler( ERR_UNHANDLED_ARG_TYPE );
			DRETURN_ARG("ERR_UNHANDLED_ARG_TYPE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
			return true; //FAIL
		} //End If: second argument is unhandled
	}	//end if: first argument is U8 or S8
	//if: first argument is U16 or S16
	else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
	{
		//Decode first argument
		uint16_t *arg_a_ptr = ARG_CAST( arg_cnt, uint16_t );
		//Accumulate argument size
		arg_cnt += Arg_size::ARG_U16_SIZE;
		//I decoded an argument
		arg_type_cnt++;
		//If: it's the only argument
		if (arg_num_types == arg_type_cnt)
		{
			///--------------------------------------------------------------------------
			///	U16 - 2
			///--------------------------------------------------------------------------

			//Declare specialized function pointer and link it to the general registered function callback address
			FUNCTION_PTR_VAR( f_ptr, uint16_t ) = FUNCTION_PTR_CAST( uint16_t )callback_ptr;
			//Execute function from its specialized pointer
			FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr) );
			DPRINT("Executing uint16_t -> void: %p | Args: %d \n", (void *)f_ptr, (*arg_a_ptr) );
		}
		//if: second argument is U8 or S8
		else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
		{
			//Decode second argument
			uint8_t *arg_b_ptr = ARG_CAST( arg_cnt, uint8_t );
			//Accumulate argument size
			arg_cnt += Arg_size::ARG_U8_SIZE;
			//I decoded an argument
			arg_type_cnt++;
			//If: it's the only argument
			if (arg_num_types == arg_type_cnt)
			{
				///--------------------------------------------------------------------------
				///	U16 U8 - 3
				///--------------------------------------------------------------------------

				//Declare specialized function pointer and link it to the general registered function callback address
				FUNCTION_PTR_VAR( f_ptr, uint16_t, uint8_t ) = FUNCTION_PTR_CAST( uint16_t, uint8_t )callback_ptr;
				//Execute function from its specialized pointer
				FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr));
				DPRINT("Executing uint16_t, uint8_t -> void: %p | Args: %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr) );
			}	//End If: it's the only argument
			//if third argument is U8 or S8
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
			{
				//Decode second argument
				uint8_t *arg_c_ptr = ARG_CAST( arg_cnt, uint8_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U8_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U16 U8 U8 - 4
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint16_t, uint8_t, uint8_t ) = FUNCTION_PTR_CAST( uint16_t, uint8_t, uint8_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint16_t, uint8_t, uint8_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode fourth argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U16 U8 U8 U8 - 5
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint16_t, uint8_t, uint8_t, uint8_t ) = FUNCTION_PTR_CAST( uint16_t, uint8_t, uint8_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint16_t, uint8_t, uint8_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
				{
					//Decode fourth argument
					uint16_t *arg_d_ptr = ARG_CAST( arg_cnt, uint16_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U16_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U16 U8 U8 U16 - 6
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint16_t, uint8_t, uint8_t, uint16_t ) = FUNCTION_PTR_CAST( uint16_t, uint8_t, uint8_t, uint16_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint16_t, uint8_t, uint8_t, uint16_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );					}	//End If: it's the only argument
				} //end if fourth argument is U16 or S16
				//if fourth argument is U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					//Decode fourth argument
					uint32_t *arg_d_ptr = ARG_CAST( arg_cnt, uint32_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U32_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U16 U8 U8 U32 - 8
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint16_t, uint8_t, uint8_t, uint32_t ) = FUNCTION_PTR_CAST( uint16_t, uint8_t, uint8_t, uint32_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint16_t, uint8_t, uint8_t, uint32_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );					}	//End If: it's the only argument
				} //end if fourth argument is U32 or S32
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U8 or S8
			//if third argument is U16 or S16
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
			{
				//Decode second argument
				uint16_t *arg_c_ptr = ARG_CAST( arg_cnt, uint16_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U16_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U16 U8 U16 - 5
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint16_t, uint8_t, uint16_t ) = FUNCTION_PTR_CAST( uint16_t, uint8_t, uint16_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint16_t, uint8_t, uint16_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode fourth argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U16 U8 U16 U8 - 6
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint16_t, uint8_t, uint16_t, uint8_t ) = FUNCTION_PTR_CAST( uint16_t, uint8_t, uint16_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint16_t, uint8_t, uint16_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
				{
					//Decode fourth argument
					uint16_t *arg_d_ptr = ARG_CAST( arg_cnt, uint16_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U16_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U16 U8 U16 U16 - 7
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint16_t, uint8_t, uint16_t, uint16_t ) = FUNCTION_PTR_CAST( uint16_t, uint8_t, uint16_t, uint16_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint16_t, uint8_t, uint16_t, uint16_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U16 or S16
				//if fourth argument is U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument is U16 or S16
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U16 or S16
			//if third argument is U32 or S32
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
			{
				//Decode second argument
				uint32_t *arg_c_ptr = ARG_CAST( arg_cnt, uint32_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U32_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U16 U8 U32 - 7
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint16_t, uint8_t, uint32_t ) = FUNCTION_PTR_CAST( uint16_t, uint8_t, uint32_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint16_t, uint8_t, uint32_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode fourth argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U16 U8 U32 U8 - 8
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint16_t, uint8_t, uint32_t, uint8_t ) = FUNCTION_PTR_CAST( uint16_t, uint8_t, uint32_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint16_t, uint8_t, uint32_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16 or U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16) || (arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument is U32 or S32
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U32 or S32
			//If: third argument is unhandled
			else
			{
				this -> error_handler( ERR_UNHANDLED_ARG_TYPE );
				DRETURN_ARG("ERR_UNHANDLED_ARG_TYPE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
				return true; //FAIL
			}	//End If: third argument is unhandled
		}	//if: second argument is U8 or S8
		//if: second argument is U16 or S16
		else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
		{
			//Decode second argument
			uint16_t *arg_b_ptr = ARG_CAST( arg_cnt, uint16_t );
			//Accumulate argument size
			arg_cnt += Arg_size::ARG_U16_SIZE;
			//I decoded an argument
			arg_type_cnt++;
			//If: it's the only argument
			if (arg_num_types == arg_type_cnt)
			{
				///--------------------------------------------------------------------------
				///	U16 U16 - 4
				///--------------------------------------------------------------------------

				//Declare specialized function pointer and link it to the general registered function callback address
				FUNCTION_PTR_VAR( f_ptr, uint16_t, uint16_t ) = FUNCTION_PTR_CAST( uint16_t, uint16_t )callback_ptr;
				//Execute function from its specialized pointer
				FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr));
				DPRINT("Executing uint16_t, uint16_t -> void: %p | Args: %d %d\n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr) );
			}	//End If: it's the only argument
			//if third argument is U8 or S8
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
			{
				//Decode second argument
				uint8_t *arg_c_ptr = ARG_CAST( arg_cnt, uint8_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U8_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U16 U16 U8 - 5
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint16_t, uint16_t, uint8_t ) = FUNCTION_PTR_CAST( uint16_t, uint16_t, uint8_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint16_t, uint16_t, uint8_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode fourth argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U16 U16 U8 U8 - 6
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint16_t, uint16_t, uint8_t, uint8_t ) = FUNCTION_PTR_CAST( uint16_t, uint16_t, uint8_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint16_t, uint16_t, uint8_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
				{
					//Decode fourth argument
					uint16_t *arg_d_ptr = ARG_CAST( arg_cnt, uint16_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U16_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U16 U16 U8 U16 - 7
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint16_t, uint16_t, uint8_t, uint16_t ) = FUNCTION_PTR_CAST( uint16_t, uint16_t, uint8_t, uint16_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint16_t, uint16_t, uint8_t, uint16_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );					}	//End If: it's the only argument
				} //end if fourth argument is U16 or S16
				//if fourth argument is U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument is U16 or S16
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U8 or S8
			//if third argument is U16 or S16
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
			{
				//Decode second argument
				uint16_t *arg_c_ptr = ARG_CAST( arg_cnt, uint16_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U16_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U16 U16 U16 - 6
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint16_t, uint16_t, uint16_t ) = FUNCTION_PTR_CAST( uint16_t, uint16_t, uint16_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint16_t, uint16_t, uint16_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode fourth argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U16 U16 U16 U8 - 7
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint16_t, uint16_t, uint16_t, uint8_t ) = FUNCTION_PTR_CAST( uint16_t, uint16_t, uint16_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint16_t, uint16_t, uint16_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
				{
					//Decode fourth argument
					uint16_t *arg_d_ptr = ARG_CAST( arg_cnt, uint16_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U16_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U16 U16 U16 U16 - 8
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint16_t, uint16_t, uint16_t, uint16_t ) = FUNCTION_PTR_CAST( uint16_t, uint16_t, uint16_t, uint16_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint16_t, uint16_t, uint16_t, uint16_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
					//if: fifth argument
					else
					{
						this -> error_handler( ERR_ARG_SIZE );
						DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
						return true; //FAIL
					} //end if fifth argument
				} //end if fourth argument is U16 or S16
				//if fourth argument is U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument is U16 or S16
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U16 or S16
			//if third argument is U32 or S32
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
			{
				//Decode second argument
				uint32_t *arg_c_ptr = ARG_CAST( arg_cnt, uint32_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U32_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U16 U16 U32 - 8
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint16_t, uint16_t, uint32_t ) = FUNCTION_PTR_CAST( uint16_t, uint16_t, uint32_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint16_t, uint16_t, uint32_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if: fourth argument
				else
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument
			}	//if: third argument is U32 or S32
			//If: third argument is unhandled
			else
			{
				this -> error_handler( ERR_UNHANDLED_ARG_TYPE );
				DRETURN_ARG("ERR_UNHANDLED_ARG_TYPE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
				return true; //FAIL
			}	//End If: third argument is unhandled
		}	//if: second argument is U16 or S16
		//if: second argument is U32 or S32
		else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
		{
			//Decode second argument
			uint32_t *arg_b_ptr = ARG_CAST( arg_cnt, uint32_t );
			//Accumulate argument size
			arg_cnt += Arg_size::ARG_U32_SIZE;
			//I decoded an argument
			arg_type_cnt++;
			//If: it's the only argument
			if (arg_num_types == arg_type_cnt)
			{
				///--------------------------------------------------------------------------
				///	U16 U32 - 6
				///--------------------------------------------------------------------------

				//Declare specialized function pointer and link it to the general registered function callback address
				FUNCTION_PTR_VAR( f_ptr, uint16_t, uint32_t ) = FUNCTION_PTR_CAST( uint16_t, uint32_t )callback_ptr;
				//Execute function from its specialized pointer
				FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr));
				DPRINT("Executing uint16_t, uint32_t -> void: %p | Args: %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr) );
			}	//End If: it's the only argument
			//if third argument is U8 or S8
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
			{
				//Decode second argument
				uint8_t *arg_c_ptr = ARG_CAST( arg_cnt, uint8_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U8_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U16 U32 U8 - 7
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint16_t, uint32_t, uint8_t ) = FUNCTION_PTR_CAST( uint16_t, uint32_t, uint8_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint16_t, uint32_t, uint8_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode fourth argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U16 U32 U8 U8 - 8
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint16_t, uint32_t, uint8_t, uint8_t ) = FUNCTION_PTR_CAST( uint16_t, uint32_t, uint8_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint16_t, uint32_t, uint8_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16 or U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16) || (arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument is U16 or S16 or U32 or S32
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U8 or S8
			//if third argument is U16 or S16
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
			{
				//Decode second argument
				uint16_t *arg_c_ptr = ARG_CAST( arg_cnt, uint16_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U16_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U16 U32 U16 - 8
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint16_t, uint32_t, uint16_t ) = FUNCTION_PTR_CAST( uint16_t, uint32_t, uint16_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint16_t, uint32_t, uint16_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if: fourth argument
				else
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument
			}	//if: third argument is U16 or S16
			//if third argument is U32 or S32
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
			{
				this -> error_handler( ERR_ARG_SIZE );
				DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
				return true; //FAIL
			}	//if: third argument is U32 or S32
			//If: third argument is unhandled
			else
			{
				this -> error_handler( ERR_UNHANDLED_ARG_TYPE );
				DRETURN_ARG("ERR_UNHANDLED_ARG_TYPE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
				return true; //FAIL
			}	//End If: third argument is unhandled

		}	//if: second argument is U32 or S32
		//If: second argument is unhandled
		else
		{
			this -> error_handler( ERR_UNHANDLED_ARG_TYPE );
			DRETURN_ARG("ERR_UNHANDLED_ARG_TYPE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
			return true; //FAIL
		} //End If: second argument is unhandled
	}	//end if: first argument is U16 or S16
	//if: first argument is S32
	else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
	{
		//Decode first argument
		uint32_t *arg_a_ptr = ARG_CAST( arg_cnt, uint32_t );
		//Accumulate argument size
		arg_cnt += Arg_size::ARG_S32_SIZE;
		//I decoded an argument
		arg_type_cnt++;
		DPRINT("Decoded argument | arg_type_cnt: %d | arg_cnt: %d | arg: %d\n", arg_type_cnt, arg_cnt, (*arg_a_ptr));
		//If: it's the only argument
		if (arg_num_types == arg_type_cnt)
		{
			///--------------------------------------------------------------------------
			///	U32 - 4
			///--------------------------------------------------------------------------

			//Passing an unsigned 32 bit pointer is decoded successfully as signed of same size
			FUNCTION_PTR_VAR( f_ptr, uint32_t ) = FUNCTION_PTR_CAST( uint32_t )callback_ptr;
			//Execute function from its specialized pointer
			FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr) );
			DPRINT("Executing int32_t -> void: %p | Args: %d\n", (void *)f_ptr, (*arg_a_ptr) );
		}
		//if: second argument is U8 or S8
		else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
		{
			//Decode second argument
			uint8_t *arg_b_ptr = ARG_CAST( arg_cnt, uint8_t );
			//Accumulate argument size
			arg_cnt += Arg_size::ARG_U8_SIZE;
			//I decoded an argument
			arg_type_cnt++;
			//If: it's the only argument
			if (arg_num_types == arg_type_cnt)
			{
				///--------------------------------------------------------------------------
				///	U32 U8 - 5
				///--------------------------------------------------------------------------

				//Declare specialized function pointer and link it to the general registered function callback address
				FUNCTION_PTR_VAR( f_ptr, uint32_t, uint8_t ) = FUNCTION_PTR_CAST( uint32_t, uint8_t )callback_ptr;
				//Execute function from its specialized pointer
				FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr));
				DPRINT("Executing uint16_t, uint8_t -> void: %p | Args: %d %d\n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr) );
			}	//End If: it's the only argument
			//if third argument is U8 or S8
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
			{
				//Decode second argument
				uint8_t *arg_c_ptr = ARG_CAST( arg_cnt, uint8_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U8_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U32 U8 U8 - 6
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint32_t, uint8_t, uint8_t ) = FUNCTION_PTR_CAST( uint32_t, uint8_t, uint8_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint32_t, uint8_t, uint8_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode fourth argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U32 U8 U8 U8 - 7
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint32_t, uint8_t, uint8_t, uint8_t ) = FUNCTION_PTR_CAST( uint32_t, uint8_t, uint8_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint32_t, uint8_t, uint8_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U16 or S16
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
				{
					//Decode fourth argument
					uint16_t *arg_d_ptr = ARG_CAST( arg_cnt, uint16_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U16_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U32 U8 U8 U16 - 8
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint32_t, uint8_t, uint8_t, uint16_t ) = FUNCTION_PTR_CAST( uint32_t, uint8_t, uint8_t, uint16_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint32_t, uint8_t, uint8_t, uint16_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );					}	//End If: it's the only argument
				} //end if fourth argument is U16 or S16
				//if fourth argument is U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument is U32 or S32
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U8 or S8
			//if third argument is U16 or S16
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
			{
				//Decode second argument
				uint16_t *arg_c_ptr = ARG_CAST( arg_cnt, uint16_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U16_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U32 U8 U16 - 7
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint32_t, uint8_t, uint16_t ) = FUNCTION_PTR_CAST( uint32_t, uint8_t, uint16_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint32_t, uint8_t, uint16_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode fourth argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U32 U8 U16 U8 - 8
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint32_t, uint8_t, uint16_t, uint8_t ) = FUNCTION_PTR_CAST( uint32_t, uint8_t, uint16_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint32_t, uint8_t, uint16_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16) || (arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument is U32 or S32
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U16 or S16
			//if third argument is U32 or S32
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
			{
				this -> error_handler( ERR_ARG_SIZE );
				DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
				return true; //FAIL
			}	//if: third argument is U32 or S32
			//If: third argument is unhandled
			else
			{
				this -> error_handler( ERR_UNHANDLED_ARG_TYPE );
				DRETURN_ARG("ERR_UNHANDLED_ARG_TYPE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
				return true; //FAIL
			}	//End If: third argument is unhandled
		}	//if: second argument is U8 or S8
		//if: second argument is U16 or S16
		else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
		{
			//Decode second argument
			uint16_t *arg_b_ptr = ARG_CAST( arg_cnt, uint16_t );
			//Accumulate argument size
			arg_cnt += Arg_size::ARG_U16_SIZE;
			//I decoded an argument
			arg_type_cnt++;
			//If: it's the only argument
			if (arg_num_types == arg_type_cnt)
			{
				///--------------------------------------------------------------------------
				///	U32 U16 - 6
				///--------------------------------------------------------------------------

				//Declare specialized function pointer and link it to the general registered function callback address
				FUNCTION_PTR_VAR( f_ptr, uint32_t, uint16_t ) = FUNCTION_PTR_CAST( uint32_t, uint16_t )callback_ptr;
				//Execute function from its specialized pointer
				FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr));
				DPRINT("Executing uint16_t, uint16_t -> void: %p | Args: %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr) );
			}	//End If: it's the only argument
			//if third argument is U8 or S8
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
			{
				//Decode second argument
				uint8_t *arg_c_ptr = ARG_CAST( arg_cnt, uint8_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U8_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U32 U16 U8 - 7
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint32_t, uint16_t, uint8_t ) = FUNCTION_PTR_CAST( uint32_t, uint16_t, uint8_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint32_t, uint16_t, uint8_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument is U8 or S8
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U8) || (arg_type[arg_type_cnt] == Arg_type::ARG_S8))
				{
					//Decode fourth argument
					uint8_t *arg_d_ptr = ARG_CAST( arg_cnt, uint8_t );
					//Accumulate argument size
					arg_cnt += Arg_size::ARG_U8_SIZE;
					//I decoded an argument
					arg_type_cnt++;
					//If: it's the only argument
					if (arg_num_types == arg_type_cnt)
					{
						///--------------------------------------------------------------------------
						///	U32 U16 U8 U8 - 8
						///--------------------------------------------------------------------------

						//Declare specialized function pointer and link it to the general registered function callback address
						FUNCTION_PTR_VAR( f_ptr, uint32_t, uint16_t, uint8_t, uint8_t ) = FUNCTION_PTR_CAST( uint32_t, uint16_t, uint8_t, uint8_t )callback_ptr;
						//Execute function from its specialized pointer
						FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr));
						DPRINT("Executing uint32_t, uint16_t, uint8_t, uint8_t -> void: %p | Args: %d %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr), (*arg_d_ptr) );
					}	//End If: it's the only argument
				} //end if fourth argument is U8 or S8
				//if fourth argument is U32 or S32
				else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16) || (arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument is U32 or S32
				//if: more than four arguments are forbidden!
				else
				{
					this -> error_handler( ERR_TOO_MANY_ARGS );
					DRETURN_ARG("ERR_TOO_MANY_ARGS in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if: more than four arguments are forbidden!
			}	//if: third argument is U8 or S8
			//if third argument is U16 or S16
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U16) || (arg_type[arg_type_cnt] == Arg_type::ARG_S16))
			{
				//Decode second argument
				uint16_t *arg_c_ptr = ARG_CAST( arg_cnt, uint16_t );
				//Accumulate argument size
				arg_cnt += Arg_size::ARG_U16_SIZE;
				//I decoded an argument
				arg_type_cnt++;
				//If: it's the only argument
				if (arg_num_types == arg_type_cnt)
				{
					///--------------------------------------------------------------------------
					///	U32 U16 U16 - 8
					///--------------------------------------------------------------------------

					//Declare specialized function pointer and link it to the general registered function callback address
					FUNCTION_PTR_VAR( f_ptr, uint32_t, uint16_t, uint16_t ) = FUNCTION_PTR_CAST( uint32_t, uint16_t, uint16_t )callback_ptr;
					//Execute function from its specialized pointer
					FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr));
					DPRINT("Executing uint32_t, uint16_t, uint16_t -> void: %p | Args: %d %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr), (*arg_c_ptr) );
				}	//End If: it's the only argument
				//if fourth argument
				else
				{
					this -> error_handler( ERR_ARG_SIZE );
					DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
					return true; //FAIL
				} //end if fourth argument
			}	//if: third argument is U16 or S16
			//if third argument is U32 or S32
			else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
			{
				this -> error_handler( ERR_ARG_SIZE );
				DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
				return true; //FAIL
			}	//if: third argument is U32 or S32
			//If: third argument is unhandled
			else
			{
				this -> error_handler( ERR_UNHANDLED_ARG_TYPE );
				DRETURN_ARG("ERR_UNHANDLED_ARG_TYPE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
				return true; //FAIL
			}	//End If: third argument is unhandled
		}	//if: second argument is U16 or S16
		//if: second argument is U32 or S32
		else if ((arg_type[arg_type_cnt] == Arg_type::ARG_U32) || (arg_type[arg_type_cnt] == Arg_type::ARG_S32))
		{
			//Decode second argument
			uint32_t *arg_b_ptr = ARG_CAST( arg_cnt, uint32_t );
			//Accumulate argument size
			arg_cnt += Arg_size::ARG_U32_SIZE;
			//I decoded an argument
			arg_type_cnt++;
			DPRINT("Decoded argument | arg_type_cnt: %d | arg_cnt: %d | arg: %d\n", arg_type_cnt, arg_cnt, (*arg_b_ptr));
			//If: it's the only argument
			if (arg_num_types == arg_type_cnt)
			{
				///--------------------------------------------------------------------------
				///	U32 U32 - 8
				///--------------------------------------------------------------------------

				//Declare specialized function pointer and link it to the general registered function callback address
				FUNCTION_PTR_VAR( f_ptr, uint32_t, uint32_t ) = FUNCTION_PTR_CAST( uint32_t, uint32_t )callback_ptr;
				//Execute function from its specialized pointer
				FUNCTION_PTR_EXE( f_ptr, (*arg_a_ptr), (*arg_b_ptr));
				DPRINT("Executing uint32_t, uint32_t -> void: %p | Args: %d %d \n", (void *)f_ptr, (*arg_a_ptr), (*arg_b_ptr) );
			}	//End If: it's the only argument
			//If: I CANNOT have a third argument after this because it would exceed the argument size limit
			else
			{
				this -> error_handler( ERR_ARG_SIZE );
				DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
				return true; //FAIL
			}	//End If: argument is unhandled
		}	//if: second argument is U32 or S32
		//If: second argument is unhandled
		else
		{
			this -> error_handler( ERR_UNHANDLED_ARG_TYPE );
			DRETURN_ARG("ERR_ARG_SIZE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
			return true; //FAIL
		} //End If: second argument is unhandled
	}	//end if: first argument is S32



	//If: argument is unhandled
	else
	{
		this -> error_handler( ERR_UNHANDLED_ARG_TYPE );
		DRETURN_ARG("ERR_UNHANDLED_ARG_TYPE in line: %d | num arg: %d | arg_index: %d\n", __LINE__, arg_type_cnt, arg_cnt);
		return true; //FAIL
	}	//End If: argument is unhandled

	///--------------------------------------------------------------------------
	///	RETURN
	///--------------------------------------------------------------------------

	//Trace Return
	DRETURN();

	return false;	//OK
}	//end method: execute_callback

/***************************************************************************/
//!	@brief Private Method
//!	error_handler | Err_codes
/***************************************************************************/
//! @param err_code | Err_codes | internal uniparser error code
//! @return void
//!	@details
//! Handles an internal parser error
/***************************************************************************/

void Uniparser::error_handler( Err_codes err_code )
{
	//Trace Enter with arguments
	DENTER_ARG( "Error code: %d\n", err_code);

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	INIT
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//Save the error code
	this -> g_err = err_code;

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Trace Return vith return value
	DRETURN();

	return;
}	//end method: error_handler | Err_codes

/****************************************************************************
**	NAMESPACES
****************************************************************************/

} //End Namespace: Orangebot
