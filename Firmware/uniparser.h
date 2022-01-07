/**********************************************************************************
**	LOGS
***********************************************************************************
**		2020-01-08
**	V5 of the uniparser library
**		2020-01-13
**	Done test bench for callback mechanism for any argument combination
**		2020-01-15
**	Complete huge porting of new callback mechanism
**	now two separate vectors for arguments and argument types
**	@test S16 S16: Bugfix and PASS
**	call mechanism tested for large selection of calls from one to four arguments
**	updated set_arg and get_arg to be template based instead of encoding the type in the name
**	@test S32 S32: Bugfix and PASS
**	@test U8 U8 U8 U8 and recovery from miss: PASS
**	@test S8 S8 S8 S8 and recovery from miss: PASS
**	@test S16 S16 S16 S16 and recovery from miss: PASS
**	@todo Add replay system to feed back older character after miss
**	@test all remaining test cases
**	@todo check command based on byte width and argument number
**	@todo detect command with same signature
**	@test two mixed arguments: PASS
**	@test three mixed arguments: PASS | chk_command needs to be reworked
**	@test four mixed arguments: PASS
**	merge error handling mechanism.
**	@test syntax errors tested
**		2019-01-16
**	Combed the test cases to add more tests and use an unified methodology
**	@bug using 0 for both match and match of command ID 0 causes problems (SOLVED)
**  BUGFIX: Now, -1 means partial match of command ID 0. Negative numbers are shifted by 1
**********************************************************************************/

/**********************************************************************************
**	ENVIROMENT VARIABILE
**********************************************************************************/

#ifndef UNIPARSER_H_
	#define UNIPARSER_H_

/**********************************************************************************
**	GLOBAL INCLUDES
**********************************************************************************/

/**********************************************************************************
**	DEFINES
**********************************************************************************/

//!redudant checks meant for debug only
#define UNIPARSER_PENDANTIC_CHECKS	false
//!Maximum number of commands that can be registered
#define UNIPARSER_MAX_CMD			20
//!Commands can have at most two arguments
#define UNIPARSER_MAX_ARGS			4
//!Size of argument vector. one byte for each identifier plus bytes for the raw data
#define UNIPARSER_ARG_VECTOR_SIZE	8
//!maximum value the argument index can have. arg_index has limited bit allocated to it inside struct _Arg_fsm_status
#define UNIPARSER_MAX_ARG_INDEX		15
//! @todo Upon miss, the FSM will relunch execution of the past # characters allowing partial matches
//#define UNIPARSER_FSM_RETRY			4
//! @todo maximum command length
#define UNIPARSER_MAX_CMD_LENGTH	32

/**********************************************************************************
**	MACROS
**********************************************************************************/

	//--------------------------------------------------------------------------
	//	PARSER
	//--------------------------------------------------------------------------

#define IS_SIGN( x )	\
	(((x) == '+') || ((x) == '-'))
//x is a number?
#define IS_NUMBER( x )	\
	(((x) >= '0') && ((x) <= '9'))

#define IS_LOWCASE_LETTER( x )	\
	(((x) >= 'a') && ((x) <= 'z'))

#define IS_HIGHCASE_LETTER( x )	\
	(((x) >= 'A') && ((x) <= 'Z'))

#define IS_LETTER( x )	\
	(IS_LOWCASE_LETTER( x ) || IS_HIGHCASE_LETTER( x ))

//Enumerate possible argument descriptors
#define IS_ARG_DESCRIPTOR( x )	\
	( ((x) == Arg_type::ARG_U8) || ((x) == Arg_type::ARG_S8) || ((x) == Arg_type::ARG_U16) || ((x) == Arg_type::ARG_S16) || ((x) == Arg_type::ARG_U32) || ((x) == Arg_type::ARG_S32) )

	//--------------------------------------------------------------------------
	//	FUNCTION POINTERS
	//--------------------------------------------------------------------------
	//	Handles declaration, assignment and execution of (void *) as functions with arguments

//Declare pointer to function
#define FUNCTION_PTR_VAR( function_name, ... )	\
	void (*function_name)( __VA_ARGS__ )

//Cast a function to fit the pointer
#define FUNCTION_PTR_CAST(...)	\
	(void (*)( __VA_ARGS__ ))

//Launch a function pointer in execution
#define FUNCTION_PTR_EXE_VOID( function_name )	\
	(*function_name)()

//Launch a function pointer in execution
#define FUNCTION_PTR_EXE( function_name, ... )	\
	(*function_name)( __VA_ARGS__ )

	//--------------------------------------------------------------------------
	//	ARGUMENT POINTER
	//--------------------------------------------------------------------------

//Promote a section of the argument vector starting from arg_index to the desired argument type
#define ARG_CAST( arg_index, type ) \
    ((type *)(&this -> g_arg[arg_index]))

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

//! @namespace Orangebot My custom namespace
namespace Orangebot
{

/**********************************************************************************
**	TYPEDEFS
**********************************************************************************/

//! List of error codes of the parser FSM
enum _Err_codes
{
	NO_ERR,					//FSM is Okay
	//Runtime errors
	ERR_GENERIC,			//Uncategorized error
	ERR_ARG_SIZE,			//Too many bytes in the argument vector
	ERR_TOO_MANY_ARGS,		//Too many arguments were given
	ERR_UNHANDLED_ARG_TYPE,	//Argument type is not registered and cannot be handled
	ERR_INVALID_CMD,		//An invalid command was given
	ERR_ADD_MAX_CMD,		//Parser already contains the maximum number of commands
	ERR_ADD_ARG,			//Failed to add an argument
	//Command Syntax errors
	SYNTAX_BAD_POINTER,			//nullptr
	SYNTAX_ARG_TYPE_INVALID,	//An invalid argument descriptor has been used
	SYNTAX_TOO_MANY_ARGS,		//Too many arguments have been specified for this command
    SYNTAX_ARG_BACKTOBACK,		//At least an ID byte required before an argument
    SYNTAX_ARG_LENGTH,			//Arguments are using too many bytes
	SYNTAX_LENGTH,				//Command is too long
	SYNTAX_FIRST_NOLETTER		//First byte must be a letter
};
typedef enum _Err_codes Err_codes;

//! Statuses of the FSM
enum _Parser_status
{
	PARSER_IDLE,			//IDLE, awaiting for ID
	PARSER_ID,				//ID successfully decoded, decoding command
	PARSER_ARG				//Process input sign or positive or argment number
};
typedef enum _Parser_status Parser_status;

//! Argument descriptors. used after % in a command to specify that the command has numeric arguments.
enum _Arg_type
{
	ARG_S8 = 's',
	ARG_U8 = 'u',
	ARG_S16 = 'S',
	ARG_U16 = 'U',
	ARG_U32 = 'D',
	ARG_S32 = 'd'
	//ARG_F32 = 'f'
};
typedef enum _Arg_type Arg_type;

//! argument stride. space inside the argument descriptor vector
enum _Arg_size
{
	ARG_S8_SIZE				= sizeof(int8_t),
	ARG_U8_SIZE				= sizeof(uint8_t),
	ARG_S16_SIZE			= sizeof(int16_t),
	ARG_U16_SIZE			= sizeof(uint16_t),
	ARG_U32_SIZE			= sizeof(uint32_t),
	ARG_S32_SIZE			= sizeof(int32_t)
	//ARG_F32_SIZE			= sizeof(float)
};
typedef enum _Arg_size Arg_size;

/**********************************************************************************
**	PROTOTYPE: STRUCTURES
**********************************************************************************/

//! status of the argument decoder FSM
struct _Arg_fsm_status
{
	//! Sign of the argument | false = positive | true = negative.
	uint8_t arg_sign	: 1;
	//! Number of arguments fully processed by the argument decoder
	uint8_t num_arg		: 3;
	//! Index of the argument currently being decoded in the argument vector
	uint8_t arg_index	: 4;
};
typedef struct _Arg_fsm_status Arg_fsm_status;

/**********************************************************************************
**	PROTOTYPE: GLOBAL VARIABILES
**********************************************************************************/

/**********************************************************************************
**	PROTOTYPE: CLASS
**********************************************************************************/

/************************************************************************************/
//! @class 		Uniparser
/************************************************************************************/
//!	@author		Orso Eric
//! @version	0.1 alpha
//! @date		2019/06
//! @brief		Uniparser V5. Universal command decoder with arguments and callback funcrion
//! @details
//! UNIPARSER V5 \n
//!	Adding a parser is a time consuming, bug prone and thankless task. \n
//! This library is the current iteration of a series of libraries meant to minimize \n
//! the workload required to add a command parser to a microcontroller application
//! @pre		No prerequisites
//! @bug		Sign bug: PWMR-127L127 is decoded as -127 | -127 (wrong) instead of -127 | +127 (right) \n
//!				SOLVED | Was caused by the sign of the argument decoder not being initialized if a sign was not specified
//!	@bug		sometime not match is confused with match of command ID 0 causing wrong an execution to happen
//! @warning	No warnings
//! @copyright	License ?
//!	@todo V6 Ability to call class methods
//! @todo Add ability to parse messages that differ only in number of parameters
/************************************************************************************/

class Uniparser
{
	//Visible to all
	public:
		//--------------------------------------------------------------------------
		//	CONSTRUCTORS
		//--------------------------------------------------------------------------

		//! Default constructor
		Uniparser( void );

		//--------------------------------------------------------------------------
		//	DESTRUCTORS
		//--------------------------------------------------------------------------

		//!Default destructor
		~Uniparser( void );

		//--------------------------------------------------------------------------
		//	OPERATORS
		//--------------------------------------------------------------------------

		//--------------------------------------------------------------------------
		//	SETTERS
		//--------------------------------------------------------------------------

		//! Add a command to the parser. Provide text that will trigger the call and function to be executed. false=command added successfully
		bool add_cmd( const char *cmd, void *handler );

		//--------------------------------------------------------------------------
		//	GETTERS
		//--------------------------------------------------------------------------

		//! Get current error code of the parser
		Err_codes get_error( void );

		//--------------------------------------------------------------------------
		//	TESTERS
		//--------------------------------------------------------------------------

		//--------------------------------------------------------------------------
		//	PUBLIC METHODS
		//--------------------------------------------------------------------------

		//! Process a byte through the parser. Handler function is automatically called when a full command is decoded
		bool parse( uint8_t data );

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

		//initialize class vars
		void init( void );

			//! Argument descriptor group
		//initialize argument decoder for a new command
		void init_arg_decoder( void );
		//add a command to the command string
		bool add_arg( uint8_t cmd_id );
		//decode an input character and accumulate it into the argument vector
		bool accumulate_arg( uint8_t data );
		//Argument has been fully decoded into argument string. Update argument descriptor FSM.
		bool close_arg( void );
		//set the value of an argument inside the argument vector
		template <typename T>
		bool set_arg( uint8_t arg_index, T arg );
		//get the value of an argument inside the argument vector
		template <typename T>
		T get_arg( uint8_t arg_index );

			//! Callback group
		//Execute the handler of function of index cmd_id. Arguments are to be axtracted from the argument vector.
		bool execute_callback( void *callback_ptr );

			//! Error group
		//Check command syntax
		bool chk_cmd( const uint8_t *cmd );
		//Handles internal parser errors
		void error_handler( Err_codes err_code );

		//--------------------------------------------------------------------------
		//	PRIVATE VARS
		//--------------------------------------------------------------------------

			/// Parser dictionary and handler functions
		// Number of commands currently registered inside the parser
		uint8_t g_num_cmd;
		//String that will trigger a command detetion and execute the callback
		const uint8_t *g_cmd_txt[UNIPARSER_MAX_CMD];
		//Partial hit status inside each command. 0 means no hit. Point to the next unmatched char in a command.
		uint8_t g_cmd_index[UNIPARSER_MAX_CMD];
		//Register the callback to be executed when the command is decoded
		void *g_cmd_handler[UNIPARSER_MAX_CMD];

			///	Argument Decoder
		//Structure that encode the status of the argument decoder FSM
		Arg_fsm_status g_arg_fsm_status;

		//Argument types for each argument are stored in the argument vector
		Arg_type g_arg_type[UNIPARSER_MAX_ARGS];
		//Arguments decoded are stored in this string
		uint8_t g_arg[UNIPARSER_ARG_VECTOR_SIZE];

			/// FSM working variables
		//number of partial matches
		//	0: no matches or parser IDLE
		//	1: just one match found. The number will become negative to skip search of commandin g_cmd_index[]
		//	>0: a number of partial matches have been found.
		//	<0: just one match found. g_num_match now holds the index of the partially matched command
		int8_t g_num_match;
		//State of the FSM
		Parser_status g_status;
		//Error status of the parser. NO_ERR means OK
		Err_codes g_err;

};	//End Class: Uniparser

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

} //End Namespace. orangebot

#else
    #warning "Multiple inclusion of hader file"
#endif
