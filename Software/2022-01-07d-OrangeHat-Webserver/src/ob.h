/****************************************************************************
**	DEFINES
****************************************************************************/

//A NAPI string feeds an extra terminator that must be pruned away to prevent parsing errors
#define BOILERPLATE_NAPI_STRING 1

/****************************************************************************
**	NAMESPACE
****************************************************************************/

namespace Orangebot
{

/****************************************************************************
**	GLOBAL VARIABLE PROTOTYPES
****************************************************************************/

/****************************************************************************
**	FUNCTION PROTOTYPES
****************************************************************************/

//Initialize OrangeBot motor board processor
void orangebot_node_cpp_init( void );
//Parse string char by char
void orangebot_parse( std::string str );

} //End namespace: Orangebot
