/****************************************************************************
**	R@P PROJECT
*****************************************************************************
**	AT_UTILS
**	Useful Atmel configuration macros
*****************************************************************************
**	Author: 			Orso Eric
**	Creation Date:		21/05/09
**	Last Edit Date:
**	Revision:			4
**	Version:			1.3
****************************************************************************/

/****************************************************************************
**	HYSTORY VERSION
*****************************************************************************
**	R1V1.0:
**		>implemented generic macro
**		>implemented port configuration macro
**		>implemented UART module macro
**	R2V1.1:
**		>added NOP macro
**		>added MASK macro
**		>added TOGGLE_BIT macro
**		>removed SET_2BIT macro (useless)
**		>removed SET_PORT_VALUE macro (useless)
**		>renamed SET_PORT macro to SET_VAR
**		>renamed CLEAR_PORT macro to CLEAR_VAR
**		>updated PORT_CONFIG macro
**		>updated USART macro handling
**		>added USART enviroment variablile to avoid those annoying warning
**		>comment updated
**		>code reformatted
**	R3V1.2
**		>Removed Shift Macro
**		>Removed Clear and SEt variabile macros
**		>Added conditional creation of PORT MACROS
**		>Added Saturated Math MACROS
**		>Added Fixed point MUL macros
**		>Added 8 bit Fixed point mul macros
**		>Added 8 bit fixed point rounded toward even mul macro
**		>Added 16 bit Fixed point mul macros
**		X>Added 16 bit fixed point rounded toward even mul macro
**	R4V1.3
**		>added circular buffer structure and handling macros
**		>added circular buffer with semaphor, allow for safe buffer push and kick
**		use in ISR and concurrent code
**		>added 8 bit exact logarithm macros
**		>added IS_NUMBR macro
**	2019-10-04
**		remove NULL. use nullptr
**	2019-10-13
**		SET_MASKED_BIT: set bit with external mask
**		SET_BIT_VALUE: set a bit to a desired true/false value
**	2019-11-10
**		TODO: update debug macros
**		BUG: AT_FP_SAT_MUL16 is bugged
****************************************************************************/

#ifndef AT_UTILS_H_		//handle multiple inclusion
	#define AT_UTILS_H_	//library define

	/****************************************************************************
	** ENVROIMENT VARIABILE
	****************************************************************************/

	/****************************************************************************
	** USEFUL DEFINE
	****************************************************************************/

	///--------------------------------------------------------------------------
	///	System Defines
	///--------------------------------------------------------------------------

	#ifndef TRUE
		#define TRUE			1
	#endif

	#ifndef FALSE
		#define FALSE			0
	#endif

	/****************************************************************************
	** GENERAL PURPOSE MACRO
	****************************************************************************/

	//do nothing for one clock cycle
	#define NOP()	__asm__ __volatile__("nop")

	//generate a mask with '1' in position shift value
	#define MASK( shift_value ) \
		( 0x01 << (shift_value) )

	//generate inverted mask '0' in position shift value
	#define INV_MASK( shift_value )	\
		(~MASK(shift_value))

	//set a single bit inside a variable leaving the other bit untouched
	#define SET_BIT( var, shift_value ) \
		( (var) |= MASK( shift_value ) )

	//clear a single bit inside a variable leaving the other bit untouched
	#define CLEAR_BIT( var, shift_value ) \
		( (var) &= (INV_MASK( shift_value ) ) )

	//toggle a single bit inside a variable leaving the other bit untouched
	#define TOGGLE_BIT( var, shift_value ) \
		( (var) ^= MASK( shift_value ) )

	//set the bits whose mask is '1' to value's bits and leave the bit whose mask is '0' unaltered
	#define SET_MASKED_BIT( var, mask, bits )	\
		( (var) = ((var) & (~(mask))) | ((bits) & (mask)))

	//set a bit in a given position to a desired value
	#define SET_BIT_VALUE( var, shift_value, value )	\
		( (var) = ( ((var) & INV_MASK(shift_value)) | ((value)<<(shift_value)) ) )

	//TRUE if a bit in position shift value is '1'
	#define IS_BIT_ONE( var, shift_value )	\
		( ( (var) & MASK( shift_value ) ) == MASK( shift_value ) )

	//TRUE if a bit in position shift value is '0'
	#define IS_BIT_ZERO( var, shift_value )	\
		( ( (var) & MASK( shift_value ) ) == 0x00 )

	//Get bit [shift_value] and put it on position 0
	#define GET_BIT( var, shift_value )	\
		((var & MASK( shift_value )) >> shift_value)

	//Extract H part of uint16_t
	#define U16H( data )	\
		(uint8_t)((uint16_t)data >> 8)
	//Extract L part of uint16_t
	#define U16L( data )	\
		(uint8_t)((uint16_t)data & (uint16_t)0x00ff)
	//Extract the h part of a s16 data
	#define S16H( x )	\
		(int8_t)((uint16_t)((x)>>8))
	//Extract L part of int16_t. The result is an unsigned number
	#define S16L( x )	\
		(uint8_t)((uint16_t)(x) & (uint16_t)0x00ff)

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

	/****************************************************************************
	**	SHIFT MACROS
	*****************************************************************************
	**	All flavor of shift and rotation, signed and unsigned,
	**	with residual and rounding and sign extension
	****************************************************************************/

		///--------------------------------------------------------------------------------
		///	SHL - SHift Left, Multiply by 2^n
		///--------------------------------------------------------------------------------
		//	example
		//	in:		7	6	5	4	3	2	1	0 	shift: <<3
		//	shl:	4	3	2	1	0	/	/	/	shift
		//	res:	/	/	/	/	/	7	6	5	residual of the shift

	//SHift Left
	#define SHL( x, s )	\
		((x) << (s))
	//Signed Shift Left (Multiply by power of 2)
	#define SSHL( x, s )	\
		(((x)>=0)?(SHL(x,s)):(-(SHL(-x, s))))
	//RESidual from a SHift Left. Needs to know the width of the input
	#define SHL_RES( x, s, w )	\
		(SHR(x,(w-s)))


		///--------------------------------------------------------------------------------
		///	SHR - SHift Right, Divide by 2^n
		///--------------------------------------------------------------------------------
		//	example
		//	in:		7	6	5	4	3	2	1	0 	shift: >>3
		//	shr:	/	/	/	7	6	5	4	3
		//	res:	/	/	/	/	/	2	1	0	residual of the shift

	//SHift Right
	#define SHR( x, s )	\
		((x) >> (s))
	//Signed right shift
	#define SSHR( x, s )	\
		(((x)>=0)?(SHR(x,s)):(-(SHR(-x, s))))
	//Residual from a Right SHift
	#define SHR_RES( x, s ) \
		((x) & (MASK(s)-1))
	//right shift with RTO rounding
	#define SHR_RTO(x, s)	\
		(((s >= 1)&&(IS_BIT_ONE(x,s-1)))?((((s>=2)&&(SHR_RES( x, s-1 ))>0))?(SHR(x,s)+1):(SHR(x,s)|1)):(SHR(x,s)))
	//Signed right shift with RTO rounding
	#define SSHR_RTO( x, s )	\
		(((x)>=0)?(SHR_RTO(x,s)):(-(SHR_RTO(-x, s))))

		///--------------------------------------------------------------------------------
		///	SH - Generic Shift
		///--------------------------------------------------------------------------------
		//	Positive shifts multiply by 2^n. Left Shift
		//	Negative shifts divide by 2^n. Right Shift

	//Generic SHift
	#define SH( x, s )	\
		( ((s) >= 0)?(SHL( x, s )):(SHR( x, -s)) )

	//Generic Signed SHift
	#define SSH( x, s )	\
		( ((s) >= 0)?(SSHL( x, s )):(SSHR( x, -s)) )

	//Generic Signed Shift with rounding toward ODD
	#define SSH_RTO( x, s )	\
		( ((s) >= 0)?(SSHL( x, s )):(SSHR_RTO( x, -s)) )

		///--------------------------------------------------------------------------------
		///	ROR - Right ROtation
		///--------------------------------------------------------------------------------
		//	example
		//	in:		7	6	5	4	3	2	1	0 	shift: >>3
		//	ror:	2	1	0	7	6	5	4	3

	#define ROR( x, s, w )	\
		( (SHR(x,s)) | (SHL(x, (w-s))) )

	#define ROR8( x, s )	\
		ROR( x, s, 8 )

	#define ROR16( x, s )	\
		ROR( x, s, 16 )

	#define ROR32( x, s )	\
		ROR( x, s, 32 )

		///--------------------------------------------------------------------------------
		///	ROL - Left ROtation
		///--------------------------------------------------------------------------------
		//	example
		//	in:		7	6	5	4	3	2	1	0 	shift: >>3
		//	ror:	4	3	2	1	0	7	6	5

	#define ROL( x, s, w )	\
		( (SHR(x,s)) | (SHL(x, (w-s))) )

	/****************************************************************************
	** CIRCULAR BUFFER MACROS
	*****************************************************************************
	**	Very useful set of macros to handle circular buffers
	**	i use them all the time, making it in macro form is very efficient
	**	Actually, the macro are very generic, i can easily make many types of buffer
	**	probably void * ones too
	****************************************************************************/

		///--------------------------------------------------------------------------
		///	BUFFER STRUCTURE PROTOTYPE
		///--------------------------------------------------------------------------
		// the data type can be anything as long as the field name stay the same

	//Status byte of an At buf structure
	typedef struct _At_buf_status At_buf_status;

	//Circular buffer structure (maximum size 255 element)
	typedef struct _At_buf8 At_buf8;

	//Circular buffer structure with semaphore (maximum size 255 element)
	typedef struct _At_buf8_safe At_buf8_safe;

		///--------------------------------------------------------------------------
		///	BUFFER STRUCTURE DEFINITIONS
		///--------------------------------------------------------------------------
		// the data type can be anything as long as the field name stay the same

	//Status byte of an At buf structure
	struct _At_buf_status
	{
		uint8_t push_flag	: 1;	//Is someone pushing into this buffer?
		uint8_t kick_flag	: 1;	//Is someone kicking from this buffer?
		uint8_t				: 6;	//Future use
	};

	//Circular buffer structure (maximum size 255 element)
	struct _At_buf8
	{
		//size of the buffer
		uint8_t size;
		//pointer to the top element
		uint8_t bot;
		//pointer to the bot element
		uint8_t top;
		//Pointer to the content (can be anything)
		uint8_t *ptr;
	};

	//Circular buffer structure with semaphore, handle exception caused by ISR
	struct _At_buf8_safe
	{
		//size of the buffer
		uint8_t size;
		//pointer to the top element
		uint8_t bot;
		//pointer to the bot element
		uint8_t top;
		//Pointer to the content (can be anything)
		uint8_t *ptr;
		//Status of the
		At_buf_status status;
	};

		///--------------------------------------------------------------------------
		///	AT_BUF_ATTACH
		///--------------------------------------------------------------------------
		///	Attach an existing vector to an existing At_buf structure

	#define AT_BUF_ATTACH( buf, ptr_in, size_in )	\
		( ((buf).ptr = ptr_in), ((buf).size = size_in) )

		///--------------------------------------------------------------------------
		///	AT_BUF_FLUSH
		///--------------------------------------------------------------------------
		/// Flush a buffer, return 0

	#define AT_BUF_FLUSH( buf )	\
		( ((buf).top = 0), ((buf).bot = 0) )

		///--------------------------------------------------------------------------
		///	AT_BUF_FLUSH_SAFER
		///--------------------------------------------------------------------------
		/// Flush a buffer, clear status flags,return 0
		///	Operate only on safe structure

	#define AT_BUF_FLUSH_SAFE( buf ) \
		( ((buf).top = 0), ((buf).bot = 0), (buf).status.push_flag = 0, (buf).status.kick_flag = 0, (0) )

		///--------------------------------------------------------------------------
		///	AT_BUF_EMPTY
		///--------------------------------------------------------------------------
		//	Often user need to know only if empty or not empty.

	#define AT_BUF_NOTEMPTY( buf )	\
		((buf).top != (buf).bot)

	#define AT_BUF_EMPTY( buf )	\
		((buf).top == (buf).bot)

		///--------------------------------------------------------------------------
		///	AT_BUF_NUMELEM
		///--------------------------------------------------------------------------
		///	Return the number of elements in a buffer

	#define AT_BUF_NUMELEM( buf )	\
		( ((buf).top >= (buf).bot) ? ((buf).top - (buf).bot) : ( (buf).size +(buf).top -(buf).bot  ) )

		///--------------------------------------------------------------------------
		///	AT_BUF_SIZE
		///--------------------------------------------------------------------------
		///	Return the size of the buffer

	#define AT_BUF_SIZE( buf )	\
		( (buf).size )

		///--------------------------------------------------------------------------
		///	AT_BUF_PUSH
		///--------------------------------------------------------------------------
		/// Push a data into a buffer
		///	Load data
		///	If: top < size
		///		top++
		///	Else:
		///		top = 0
		///	RETURN 0

	#define AT_BUF_PUSH( buf, data )	\
		( ( (buf).ptr[ (buf).top ] = data ), ( ( (buf).top < ((buf).size -1) ) ? ((buf).top++) : ((buf).top = 0) ) )

		///--------------------------------------------------------------------------
		///	AT_BUF_PUSH_SAFE
		///--------------------------------------------------------------------------
		/// Push a data into a buffer, checking that the buffer won't overflow,
		/// return 1 if th data could not be pushed
		///	If: current element < size
		///		AT_BUF_PUSH
		///		RETURN 0
		///	Else:
		///		RETURN 1

	#define AT_BUF_PUSH_SAFE( buf, data )	\
		( (AT_BUF_NUMELEM(buf) < ((buf).size-1)) ? (AT_BUF_PUSH(buf,data)) : (1) )

		///--------------------------------------------------------------------------
		///	AT_BUF_PUSH_SAFEER
		///--------------------------------------------------------------------------
		///	Operate on safe structure, return 1 if buf is full, return 2 if buf is busy
		///	because of other operations
		///	If: busy pushing?
		///		return 2
		///	Else:
		///		busy = 1
		///		ret = AT_BUF_PUSH_SAFE
		///		busy = 0
		///		return ret

	#define AT_BUF_PUSH_SAFER( buf, data ) \
		( ((buf).status.push_flag == 1) ? (2) : ( (((buf).status.push_flag = 1), (AT_BUF_NUMELEM(buf) < ((buf).size-1))) ? ((AT_BUF_PUSH(buf,data)), ((buf).status.push_flag = 0), (0)) : (((buf).status.push_flag = 0), (1)) ) )

		///--------------------------------------------------------------------------
		///	AT_BUF_PEEK
		///--------------------------------------------------------------------------
		///	Return the oldest element of the buffer, the user should check if there are
		///	element in the buffer

	#define AT_BUF_PEEK( buf )	\
		( (buf).ptr[ (buf).bot ]  )

		///--------------------------------------------------------------------------
		///	AT_BUF_KICK
		///--------------------------------------------------------------------------
		///	Kick out the oldest element of the buffer, the user should check that there is
		/// at least 1 element in the buffer, return bot
		///	If: bot < size
		///		bot++
		///	Else:
		///		bot = 0

	#define AT_BUF_KICK( buf )	\
		( (((buf).bot < ((buf).size -1)) ? ((buf).bot++) : ((buf).bot = 0)))

		///--------------------------------------------------------------------------
		///	AT_BUF_KICK_SAFE
		///--------------------------------------------------------------------------
		///	kick the oldest element if there is at least one element in the buffer
		/// return 1 if fail
		///	If: num_elem > 0
		///		AT_BUF_KICK
		///		RETURN 0
		///	Else
		///		RETURN 1

	#define AT_BUF_KICK_SAFE( buf )	\
		(( AT_BUF_NUMELEM( buf ) > 0 ) ? (AT_BUF_KICK( buf )) : (1))

		///--------------------------------------------------------------------------
		///	AT_BUF_KICK_SAFER
		///--------------------------------------------------------------------------
		///	Kick the oldest element, if another kick operation is rnning, return 2 instead
		///	Work only on safe structure
		///	If: busy  == 1
		///		RETURN 2
		///	Else:
		///		busy = 1
		///		ret = AT_BUF_KICK_SAFE
		///		busy = 0
		///		RETURN ret

	#define AT_BUF_KICK_SAFER( buf )	\
		( ((buf).status.kick_flag == 1) ? (2) : ( (((buf).status.kick_flag = 1), ( AT_BUF_NUMELEM( buf ) > 0 )) ? ((AT_BUF_KICK( buf )), ((buf).status.kick_flag = 0), (0)) : (((buf).status.kick_flag = 0), (1))        ) )


	/****************************************************************************
	** PRESCALER CALCULATION MACROS
	****************************************************************************/

	/*******************************************
	**	AT_OCR
	********************************************
	**	Setup a counter to obtain a given fout
	**
	**	fout [Hz]	= output frequency
	**	fin [Hz]	= input frequency
	**	N [1]		= prescaler
	**	K [1]		= 0 or 1, depend if use UP or UPDOWN mode
	**
	**	fout = fin / (2^N * (M+1) * 2^K)
	**
	**	M = fin / (fout * 2^(N+K)) -1
	*******************************************/

	#define AT_OCR( fin, fout, n, k)	\
		(( ((fin) / (fout)) >> ((n)+(k)) ) -1)

	/*******************************************
	**	AT_OCR_ROUND
	********************************************
	**	choose the correct M between M and M+1
	**	>M' = f(2*fout)
	**	>M' even?
	**		>M = f(fout)
	**	>else
	**		>M = f(fout) +1
	*******************************************/

	#define AT_OCR_ROUND( fin, fout, n, k)	\
		(( AT_OCR((fin),(fout)*2,(n),(k)) & 1 )?( 1 +AT_OCR((fin),(fout),(n),(k)) ):(AT_OCR((fin),(fout),(n),(k))))

		///********************************************************************************
		///	AT_ABS
		///********************************************************************************
		//	negative/positive -> positive

	//Absolute value
	#define AT_ABS( x )	\
		((x>=0)?(x):(-(x)))

	/****************************************************************************
	**	FIXED POINT ARITH MACROS
	*****************************************************************************
	**	Those macros handle operation with fixed point variabile
	**	The SUM macros are variabile type indipendent and handle saturation limit
	**	ex.
	**	0111.0001 +		// +7.0625
	**	0001.1000 =		// +1.5
	**	-----------
	**	0111.1111		// +7.9375
	**
	**	The MUL macros handle the renormalisation and the saturation
	**	The rounded toward even version of the macro has PROS and CONS:
	**	PROS:
	**	-1 Order of Magnitude reduction for the mean error
	**	CONS:
	**	-2 Sligth increase of the rms error
	**	-3 Increased computational cost (1 additionale heavy brench)
	**
	**	ex.
	**	0000.1000 *		// +0.5
	**	0000.1000 *		// +0.5
	**	-----------
	**	0000.0100		// +0.25
	**
	**	ex.	same number with different point position yeld to different result
	**	00001.000 *		// +1.0
	**	00001.000 *		// +1.0
	**	-----------
	**	00001.000		// +1.0
	****************************************************************************/

		///--------------------------------------------------------------------------
		///	SATURATION HANDLING
		///--------------------------------------------------------------------------
		///	Handle up and low bound of variabile

	#define AT_SAT( x, up_bound, low_bound )	\
		( ((x)>=(up_bound))?(up_bound):(((x)<=(low_bound))?(low_bound):(x)) )


		///--------------------------------------------------------------------------
		///	TWO WAY SATURATED SUM
		///--------------------------------------------------------------------------
		///	AT_FP_SAT_SUM( x, y, up_bound, low_bound )
		///	x, y				: Fixed Point Operand
		///	up_bound, low_bound	: Bound for the operation (can be lower than the variabile range)

	//This macro handle the sum between two number handling the saturation
	//Algorithm:
	//	(x > 0) && (y >= up_bound - x) ?			//X > 0 and Y > positive overflow treshold
	//	Yes
	//		up_bound								//Positive Overflow
	//	No
	//		(x < 0) && (y <= low_bound - x) ?		//X < 0 and Y < negative overflow treshold
	//		Yes
	//			low_bound							//Negative Overflow
	//		No
	//			x + y								//No overflow
	#define AT_SAT_SUM( x, y, up_bound, low_bound )	\
		( ( (x) > 0 ) && ( (y) >= ((up_bound) - (x)) ) )? (up_bound) : ( ( ( (x) < 0 ) && ( (y) <= ((low_bound) - (x)) ) )? (low_bound) : ((x) + (y)) )

		///--------------------------------------------------------------------------
		///	THREE WAY SATURATED SUM
		///--------------------------------------------------------------------------
		///	AT_FP_SAT_SUM( x, y, z, up_bound, low_bound )
		///	x, y, z				: Fixed Point Operand
		///	up_bound, low_bound	: Bound for the operation (can be lower than the variabile range)

	//This macro solve an issue when i have to sum three element in a saturated way
	//The problem happen when the first partial result saturate (discarding something) and
	//the next operation would subtract from the first one
	//es
	//	a = 127, b = 60, c = -128: 		(expected result = 59)
	//	127 + 60 = 127, 127 - 128 = -1 	(wrong result = -1)
	//Algorithm:
	//	(a + b == up_bound AND c < 0 ) OR (a + b == low_bound AND c > 0 )
	//		YES
	//			(a + c) + b
	//		NO
	//			(a + b) + c
	#define AT_SAT_3SUM( x, y, z, up_bound, low_bound )	\
		( ( (AT_SAT_SUM( (x), (y), (up_bound), (low_bound) ) == (up_bound)) && ((z) < 0) ) || ( (AT_SAT_SUM( (x), (y), (up_bound), (low_bound) ) == (low_bound)) && ((z) > 0) ) )?AT_SAT_SUM( AT_SAT_SUM( (x), (z), (up_bound), (low_bound) ), (y), (up_bound), (low_bound) ):AT_SAT_SUM( AT_SAT_SUM( (x), (y), (up_bound), (low_bound) ), (z), (up_bound), (low_bound) )

		///********************************************************************************
		///	AT_TOP_INC
		///********************************************************************************
		//	Increment. If increment would exceed top -> reset
		//	Ex. top = 7
		//	cnt = 0, 1, 2, 3, 4, 5, 6, 7, 0, ...

	#define AT_TOP_INC( cnt, top )	\
		(((cnt) == (top))?(0):((cnt)+1))

		///********************************************************************************
		///	CIRCULAR SUM
		///********************************************************************************
		//	This function allow sum to work like an 8bit var would start again from 0 once a sum overflow
		//	sometime this is useful
		//	TODO: I can handle the case when i go around max multiple times

	#define AT_CIRCULAR_SUM( x, y, max )	\
		( ((x +y) >= (max))?(x +y -max):(x +y) )
		///--------------------------------------------------------------------------
		///	DIVISION HANDLING
		///--------------------------------------------------------------------------
		///	This macro handle division by power of 2, no rounding at all
		///	Algorithm:
		///		out = x / (1<<fp)

	#define AT_DIVIDE( x, fp )	\
		( (x) / (1 << (fp) ) )

		///--------------------------------------------------------------------------
		///	DIVISION HANDLING (ROUND TOWARD NEGATIVE)
		///--------------------------------------------------------------------------
		///	This macro handle division by power of 2, no rounding at all
		///	The 0.5 case is always rounded down
		///	Algorithm:
		///	>The first bit masked away is '1'?
		///	Y:
		///		out+1
		///	N:
		///		out


	#define AT_DIVIDE_RTN( x, fp )	\
		( ((x)&(MASK(fp-1)))?(((x)/MASK(fp))+1):((x)/MASK(fp)) )

		///--------------------------------------------------------------------------
		///	DIVISION HANDLING (ROUND TOWARD ODD)
		///--------------------------------------------------------------------------
		///	This macro handle division by power of 2, the result is renormalised and rounded
		///	toward the nearest even number, allow for minimal error if the data have non 0 mean value
		///	I round toward odd because is simpler to handle the rounding, i just have to OR the
		///	result with '1'
		///
		///	Algorithm:
		///		The first bit masked away is '1'?
		///		Y:
		///			The other masked bit are not exactly 0?
		///			Y:
		///				out+1
		///			N:
		///				out | 1
		///		N:
		///			out

	#define AT_DIVIDE_RTO( x, fp )	\
		( ((x)&(MASK(fp-1)))?(((x)&(MASK(fp-1)-1))?(((x)/MASK(fp))+1):(((x)/MASK(fp))|1)):((x)/MASK(fp)) )

		///--------------------------------------------------------------------------
		///	GENERAL PORPOUSE SATURATED FIXED POINT MUL
		///--------------------------------------------------------------------------
		///	This macro handle the multiplication and renormalization between two number
		///	x, y				: Fixed Point Operand
		///	fp					: Fixed point position
		///	up_bound, low_bound	: Bound for the operation (can be lower than the variabile range)
		///	op_type				: Type of the operand
		///	mul_type			: Type of the mul result (es. if the operand is int16, this is int32
		///	mul mask			: Mask the useless bit of the result it es for int16 operand is 32 bit 0x0000ffff

	//	x, y	= Number
	//	fp = position of the point
	//	Algorithm:
	//	x * y  >= up bound * (1 << fp)?
	//	Yes
	//		up bound
	//	No
	//		x * y  <= low bound * (1 << fp)?
	//			Yes
	//				low_bound
	//			No
	//				x * y / (1 << fp)
	#define AT_FP_SAT_MUL( x, y, fp, up_bound, low_bound, op_type, mul_type, mul_mask )	\
		( ((mul_type)((x)*(y))>=((mul_type)1*(up_bound)*(1<<(fp))))?((op_type)(up_bound)):( ((mul_type)((x)*(y))<=((mul_type)1*(low_bound)*(1<<(fp))))?((op_type)(low_bound)):((op_type)((mul_type)(mul_mask)&(mul_type)((x)*(y)/(1<<(fp)))))) )

		///--------------------------------------------------------------------------
		///	SATURATED RENORMALISED FIXED POINT 8BIT MUL
		///--------------------------------------------------------------------------
		/// Wrapper for the 8 bit fixed point saturated multiplication

	//Setting the parameter for the 8 bit mul macro
	#define AT_FP_SAT_MUL8( x, y, fp )	\
		AT_FP_SAT_MUL( x, y, fp, (int8_t)+127, (int8_t)-127, int8_t, int16_t, 0x00ff )

		///--------------------------------------------------------------------------
		///	SATURATED, RENORMALISED FIXED POINT 16BIT MUL
		///--------------------------------------------------------------------------
		/// Wrapper for the 16 bit fixed point saturated multiplication

	//Setting the parameter for the 16 bit mul macro
	#define AT_FP_SAT_MUL16( x, y, fp )	\
		AT_FP_SAT_MUL( x, y, fp, (int16_t)+32767, (int16_t)-32767, int16_t, int32_t, (int32_t)0x0000ffff )
		//( ((int32_t)((x)*(y))>=(int32_t)(MAX_INT16*(1<<(fp))))?((int16_t)MAX_INT16):( ((int32_t)((x)*(y))<=(int32_t)(MIN_INT16*(1<<(fp))))?((int16_t)MIN_INT16):((int16_t)((int32_t)0x0000ffff&(int32_t)((x)*(y)/(1<<(fp)))))) )

		///--------------------------------------------------------------------------
		///	SATURATED, ROUNDED TOWARD EVEN, RENORMALISED FIXED POINT 8BIT MUL
		///--------------------------------------------------------------------------
		///	This macros handle the multiplication and renormalization and the rounding toward even
		///	es.
		///	3.0 * 0.6 = 1.8 -> = 2.0	//resto > 0.5 => arrotondato per eccesso
		///	3.0 * 0.5 = 1.5 -> = 2.0	//resto = 0.5 => arrotondato al pari piu' vicino
		///	5.0 * 0.5 = 2.5 -> = 2.0	//resto = 0.5 => arrotondato al pari piu' vicino
		///	3.0 * 0.4 = 1.2 -> = 1.0	//resto < 0.5 => arrotondato per difetto
		///	x * y  >= up bound * (1 << fp)?			//Result Higher than the upper bound?
		///	Yes
		///		up bound							//Return Up bound
		///	No
		///		x * y <= low bound  * (1 << fp)?	//Result lower than Low bound?
		///		Yes
		///			low_bound						//Return Low Bound
		///		No
		///											//Result would be exactly x.5 and x is even OR result is lower than x.5
		///			YES
		///											//X
		///			NO
		///											//X +1

	#define SAT_ROUND_MUL8( x, y, fp )	\
		((int16_t)((x) * (y) / (1 << (fp))) >= (int16_t)MAX_INT8)?(int8_t)MAX_INT8:( ((int16_t)((x) * (y) / (1 << (fp))) <= (int16_t)MIN_INT8)?(int8_t)MIN_INT8:(((fp>0) && ((x*y)&(1<<(fp-1))))?(((x*y)>0)?((int8_t)( (int16_t)0x00ff & (int16_t)((x) * (y) / (1 << (fp))+1))):((int8_t)( (int16_t)0x00ff & (int16_t)((x) * (y) / (1 << (fp))-1)))):(int8_t)( (int16_t)0x00ff & (int16_t)((x) * (y) / (1 << (fp))))) )

		///********************************************************************************
		///	AT_MUL_ONE Multiply a number by a frection m/2^n Rounded Toward Odd
		///********************************************************************************
		///	This function execute a*m/2^n where m/2^n must be <1. It's a very common operation
		///	I start by subtracting (a) with its remainder (a/2^n)*2^n
		///	Then I multiply that by m, and divide by 2^n with the RTO function
		///	Then I sum the unrounded division. If I do this part first, i would need 2a maximum (bad)

	#define AT_MUL_ONE( a, m, n ) \
		( AT_DIVIDE_RTO( ((a-((a/MASK(n))*MASK(n)))*m) , n ) +((a/MASK(n))*m) )

    #define AT_MUL_NEG_ONE( a, m, n) \
		((a<0)?(-AT_MUL_ONE( (-(a)), m, n )):AT_MUL_ONE( a, m, n ))

		///********************************************************************************
		///	RTE, RTO ROUND CONDITION
		///********************************************************************************
		//	true if round UP
		//Algorithm:
		//r > d/2		//>0.5?
		//	y++				//round UP
		//r == d/2		//==0.5?
		//	d odd		//d was odd, it's actually >0.5
		//		y++			//round UP
		//	y odd		//Spread rounding error. round to even
		//		y++			//round up
	#define ROUND_RTE( y, r, d ) \
		((r>(d/2))?(1):((r==(d/2))?(IS_BIT_ONE(d,0))?(1):((IS_BIT_ONE(y,0))?(1):(0)):(0)))

	#define ROUND_RTO( y, r, d ) \
		((r > d/2)?(1):((r == d/2)?((IS_BIT_ONE(d,0))?(1):((IS_BIT_ZERO(y,0))?(1):(0))):(0)))

	/****************************************************************************
	** OVERFLOW CONDITIONS
	****************************************************************************/

	//will an unsigned sum overflow?
	#define WILL_U_OVF( a, b , top )	\
		(a > (top -b))

	//will a signed sum overflow?
	//can be used for all cases U+S, S+S, S+U and ->S, -> U. just cast everything and use appropriate bounds
	#define WILL_S_OVF( a, b, bot, top ) \
		((b>=0)?(WILL_U_OVF(a,b,top)):(a < (bot-b)))

		///********************************************************************************
		///	LOGARITHM
		///********************************************************************************
		//	Basically count the weight+1 of the MSB '1'
		//	Implemented with a binary bisection tree. 4 r 3 comparisons needed for result.
		//	the case x=0,x=1 ar the only that needs 4. because 0 is a special case

	//4bit logarithm [0, 15]  -> [0, 4]
	//	x		|	0	1	2	3	4 ...	7	8 ...	15
	//	log2	|	0	1	2	2	3 ...	3	4 ...	4
	#define AT_LOG24( x )	\
		((x<4)?((x<2)?((x<1)?(0):(1)):(2)):((x<(8))?(3):(4)))

	//8bit logarithm [0, 255] -> [0, 8]
	//	x		|	0	1	2	3	4	..	7	8	..	15	16	..	31	32	..	63	64	..	127	128	..	255
	//	log2	|	0	1	2	2	3	..	3	4	..	4	5	..	5	6	..	6	7	..	7	8	..	8
	#define AT_LOG28( x )	\
		((x<16)?(AT_LOG24(x)):((x<64)?(((x<32)?(5):(6))):(((x<128)?(7):(8)))))

	#define AT_LOG216( x )	\
		((x<256)?(AT_LOG28(x)):(8+AT_LOG28(((x)>>8))))

	#define AT_LOG2S16( x )	\
		((x>=0)?(AT_LOG216(x)):(-AT_LOG216((-x))))

#else
	#warning "multiple inclusion of AT_UTILS_H_"
#endif
