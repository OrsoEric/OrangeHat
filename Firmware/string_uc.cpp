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
**	String_uC
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
**	This library is meant to provide basic string functionality to microcontrollers
**	The library uses well defined types often used in microcontrollers
**	This library provides conversion from a binary number to a string
**	THe library make use of templates
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
//Debug trace log
//#define ENABLE_DEBUG
#include "debug.h"
//Class Header
#include "string_uc.h"

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
//!	String_uc | void
/***************************************************************************/
// @param
//! @return no return
//!	@details
//! Empty constructor
/***************************************************************************/

String_uc::String_uc( void )
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
}	//end constructor:

/****************************************************************************
*****************************************************************************
**	DESTRUCTORS
*****************************************************************************
****************************************************************************/

/***************************************************************************/
//!	@brief Empty Destructor
//!	String_uc | void
/***************************************************************************/
// @param
//! @return no return
//!	@details
//! Empty destructor
/***************************************************************************/

String_uc::~String_uc( void )
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

/****************************************************************************
*****************************************************************************
**	GETTERS
*****************************************************************************
****************************************************************************/

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

/****************************************************************************
*****************************************************************************
**	PUBLIC STATIC METHODS
*****************************************************************************
****************************************************************************/

/***************************************************************************/
//!	function
//!	str_len | uint8_t, uint8_t *
/***************************************************************************/
//! @param str			| string reference
//! @param max_length	| maximum length of the string
//! @return uint8_t	| length of the string up to the terminator. max_length=error
//! @brief Convert an int32_t to string
//! @details
//!	Compute length of a string
/***************************************************************************/

uint8_t String_uc::str_length( uint8_t *str, uint8_t max_length )
{
	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//counter
	uint8_t t = 0;

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//While string is not over
	while ((t < max_length) && (str[t]!='\0'))
	{
		//Scan next character
		t++;
	}
	//Account for terminator character
	t++;

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	return t;
} //End function: str_len | uint8_t, uint8_t *

/***************************************************************************/
//!	@brief Public Static Method
//!	to_string<uint8_t> | uint8_t, uint8_t *
/***************************************************************************/
//! @param value | binary number to be converted to string
//! @param str | string in which to load the result. User must take care to allocate a string big enough. User::String_uc::Size::STRING_U8
//! @return
//!	@details
//! Convert number to string. String is provided by the user. Return the index of the terminator
//! enum User::String_uc::Size:: hold all relevant sizes user need
/***************************************************************************/

template <>
uint8_t String_uc::to_string( uint8_t value, uint8_t *str )
{
	//Trace Enter
	DENTER_ARG( "value: %d | string pointer: %p\n", value, str );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//decimal base
	uint8_t base[] =
	{
		100,
		10,
		1
	};
	//temp var
	uint8_t t;
	uint8_t value_tmp;

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//index to the string
	uint8_t index = 0;
	//flag used to blank non meaningful zeros
	bool flag = true;

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//For all bases
	for (t = 0;t < String_uc::Size::DIGIT_U8; t++)
	{
		//If the base is bigger or equal than the number (division is meaningful)
		if (base[t] <= value)
		{
			//Divide number by base, get the digit
			value_tmp = value/base[t];
			//Write the digit
			str[ index ] = '0' +value_tmp;
			//Update the number
			value = value - base[t] * value_tmp;
			//I have found a meaningful digit
			flag = false;
			//Jump to the next digit
			index++;
		}
		//If: The base is smaller then the number, and I have yet to find a non zero digit, and I'm not to the last digit
		else if ( (flag == true) && (t != (String_uc::Size::DIGIT_U8 -1)) )
		{
			//do nothing
		}
		//If: I have a meaningful zero
		else
		{
			//It's a zero
			str[ index ] = '0';
			//Jump to the next digit
			index++;
		}
	}	//End for: all bases

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Append the terminator
	str[ index ] = '\0';

	//Trace Return
	DRETURN_ARG( "length: %d | return string: >%s<\n", index, str );
	return index;	//OK
}	//end method: to_string<uint8_t> | uint8_t, uint8_t *

/***************************************************************************/
//!	@brief Public Static Method
//!	to_string<int8_t> | int8_t, uint8_t *
/***************************************************************************/
//! @param value | binary number to be converted to string
//! @param str | string in which to load the result. User must take care to allocate a string big enough. User::String_uc::Size::STRING_U8
//! @return
//!	@details
//! Convert number to string. String is provided by the user. Return the index of the terminator
//! enum User::String_uc::Size:: hold all relevant sizes user need
/***************************************************************************/

template <>
uint8_t String_uc::to_string( int8_t value, uint8_t *str )
{
	//Trace Enter
	DENTER_ARG( "value: %d | string pointer: %p\n", value, str );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//number of character written
	uint8_t length;
	//Temp conversion to unsigned
	uint8_t value_tmp;

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//If: negative
	if (value < 0)
	{
		//Write sign '-'
		str[ 0 ] = '-';
		//Correct sign
		value_tmp = -value;

	}
	//If: zero or positive
	else
	{
		//Write sign '+'
		str[ 0 ] = '+';
		value_tmp = value;
	}
	//launch the u8_to_str to the corrected num, but feed the vector shifted by 1 to make room for the sign. save the return value
	length = to_string<uint8_t>( value_tmp, &str[1] );
	//Take into account sign
	length++;

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Trace Return
	DRETURN_ARG( "length: %d | return string: >%s<\n", length, str );
	return length;
} //End function: to_string<int8_t> | int8_t, uint8_t *

/***************************************************************************/
//!	@brief Public Static Method
//!	to_string<uint16_t> | uint16_t, uint8_t *
/***************************************************************************/
//! @param value | binary number to be converted to string
//! @param str | string in which to load the result. User must take care to allocate a string big enough. User::String_uc::Size::STRING_U8
//! @return
//!	@details
//! Convert number to string. String is provided by the user. Return the index of the terminator
//! enum User::String_uc::Size:: hold all relevant sizes user need
/***************************************************************************/

template <>
uint8_t String_uc::to_string( uint16_t value, uint8_t *str )
{
	//Trace Enter
	DENTER_ARG( "value: %d | string pointer: %p\n", value, str );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//decimal base
	uint16_t base[] =
	{
		10000,
		1000,
		100,
		10,
		1
	};
	//temp var
	uint8_t t;
	uint8_t value_tmp;

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//index to the string
	uint8_t index = 0;
	//flag used to blank non meaningful zeros
	bool flag = true;

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//For all bases
	for (t = 0;t < String_uc::Size::DIGIT_U16; t++)
	{
		//If the base is bigger or equal than the number (division is meaningful)
		if (base[t] <= value)
		{
			//Divide number by base, get the digit
			value_tmp = value/base[t];
			//Write the digit
			str[ index ] = '0' +value_tmp;
			//Update the number
			value = value - base[t] * value_tmp;
			//I have found a meaningful digit
			flag = false;
			//Jump to the next digit
			index++;
		}
		//If: The base is smaller then the number, and I have yet to find a non zero digit, and I'm not to the last digit
		else if ( (flag == true) && (t != (String_uc::Size::DIGIT_U16 -1)) )
		{
			//do nothing
		}
		//If: I have a meaningful zero
		else
		{
			//It's a zero
			str[ index ] = '0';
			//Jump to the next digit
			index++;
		}
	}	//End for: all bases

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Append the terminator
	str[ index ] = '\0';

	//Trace Return
	DRETURN_ARG( "length: %d | return string: >%s<\n", index, str );
	return index;	//OK
}	//end method: to_string<uint16_t> | uint16_t, uint8_t *

/***************************************************************************/
//!	@brief Public Static Method
//!	to_string<int16_t> | int16_t, uint8_t *
/***************************************************************************/
//! @param value | binary number to be converted to string
//! @param str | string in which to load the result. User must take care to allocate a string big enough. User::String_uc::Size::STRING_U8
//! @return
//!	@details
//! Convert number to string. String is provided by the user. Return the index of the terminator
//! enum User::String_uc::Size:: hold all relevant sizes user need
/***************************************************************************/

template <>
uint8_t String_uc::to_string( int16_t value, uint8_t *str )
{
	//Trace Enter
	DENTER_ARG( "value: %d | string pointer: %p\n", value, str );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//number of character written
	uint8_t length;
	//Temp conversion to unsigned
	uint16_t value_tmp;

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//If: negative
	if (value < 0)
	{
		//Write sign '-'
		str[ 0 ] = '-';
		//Correct sign
		value_tmp = -value;

	}
	//If: zero or positive
	else
	{
		//Write sign '+'
		str[ 0 ] = '+';
		value_tmp = value;
	}
	//launch the u8_to_str to the corrected num, but feed the vector shifted by 1 to make room for the sign. save the return value
	length = to_string<uint16_t>( value_tmp, &str[1] );
	//Take into account sign
	length++;

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Trace Return
	DRETURN_ARG( "length: %d | return string: >%s<\n", length, str );
	return length;
} //End function: to_string<int16_t> | int16_t, uint8_t *

/***************************************************************************/
//!	@brief Public Static Method
//!	to_string<uint32_t> | uint32_t, uint8_t *
/***************************************************************************/
//! @param value | binary number to be converted to string
//! @param str | string in which to load the result. User must take care to allocate a string big enough. User::String_uc::Size::STRING_U8
//! @return
//!	@details
//! Convert number to string. String is provided by the user. Return the index of the terminator
//! enum User::String_uc::Size:: hold all relevant sizes user need
/***************************************************************************/

template <>
uint8_t String_uc::to_string( uint32_t value, uint8_t *str )
{
	//Trace Enter
	DENTER_ARG( "value: %d | string pointer: %p\n", value, str );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//decimal base
	uint32_t base[] =
	{
		1000000000,
		100000000,
		10000000,
		1000000,
		100000,
		10000,
		1000,
		100,
		10,
		1
	};
	//temp var
	uint8_t t;
	uint8_t value_tmp;

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//index to the string
	uint8_t index = 0;
	//flag used to blank non meaningful zeros
	bool flag = true;

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//For all bases
	for (t = 0;t < String_uc::Size::DIGIT_U32; t++)
	{
		//If the base is bigger or equal than the number (division is meaningful)
		if (base[t] <= value)
		{
			//Divide number by base, get the digit
			value_tmp = value/base[t];
			//Write the digit
			str[ index ] = '0' +value_tmp;
			//Update the number
			value = value - base[t] * value_tmp;
			//I have found a meaningful digit
			flag = false;
			//Jump to the next digit
			index++;
		}
		//If: The base is smaller then the number, and I have yet to find a non zero digit, and I'm not to the last digit
		else if ( (flag == true) && (t != (String_uc::Size::DIGIT_U32 -1)) )
		{
			//do nothing
		}
		//If: I have a meaningful zero
		else
		{
			//It's a zero
			str[ index ] = '0';
			//Jump to the next digit
			index++;
		}
	}	//End for: all bases

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Append the terminator
	str[ index ] = '\0';

	//Trace Return
	DRETURN_ARG( "length: %d | return string: >%s<\n", index, str );
	return index;	//OK
}	//end method: to_string<uint32_t> | uint32_t, uint8_t *

/***************************************************************************/
//!	@brief Public Static Method
//!	to_string<int32_t> | int32_t, uint8_t *
/***************************************************************************/
//! @param value | binary number to be converted to string
//! @param str | string in which to load the result. User must take care to allocate a string big enough. User::String_uc::Size::STRING_U8
//! @return
//!	@details
//! Convert number to string. String is provided by the user. Return the index of the terminator
//! enum User::String_uc::Size:: hold all relevant sizes user need
/***************************************************************************/

template <>
uint8_t String_uc::to_string( int32_t value, uint8_t *str )
{
	//Trace Enter
	DENTER_ARG( "value: %d | string pointer: %p\n", value, str );

	//----------------------------------------------------------------
	//	VARS
	//----------------------------------------------------------------

	//number of character written
	uint8_t length;
	//Temp conversion to unsigned
	uint32_t value_tmp;

	//----------------------------------------------------------------
	//	BODY
	//----------------------------------------------------------------

	//If: negative
	if (value < 0)
	{
		//Write sign '-'
		str[ 0 ] = '-';
		//Correct sign
		value_tmp = -value;

	}
	//If: zero or positive
	else
	{
		//Write sign '+'
		str[ 0 ] = '+';
		value_tmp = value;
	}
	//launch the u8_to_str to the corrected num, but feed the vector shifted by 1 to make room for the sign. save the return value
	length = to_string<uint32_t>( value_tmp, &str[1] );
	//Take into account sign
	length++;

	//----------------------------------------------------------------
	//	RETURN
	//----------------------------------------------------------------

	//Trace Return
	DRETURN_ARG( "length: %d | return string: >%s<\n", length, str );
	return length;
} //End function: to_string<int32_t> | int32_t, uint8_t *

/****************************************************************************
*****************************************************************************
**	PRIVATE METHODS
*****************************************************************************
****************************************************************************/

/***************************************************************************/
//!	@brief Public Method
//!	String_uc | void
/***************************************************************************/
// @param
//! @return no return
//!	@details
//! Method
/***************************************************************************/

bool String_uc::dummy( void )
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

	return false;	//OK
}	//end method:

/****************************************************************************
**	NAMESPACES
****************************************************************************/

} //End Namespace
