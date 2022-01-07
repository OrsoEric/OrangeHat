//-----------------------------------------------------------------------------------
//	ORANGEBOT KEY
//-----------------------------------------------------------------------------------
//	@author Orso Eric
//	Keystroke event handler 

//Map of keys that are down
var key_forward		= 0;
var key_backward	= 0;
var key_left		= 0;
var key_right		= 0;

//Current direction described by the keys
var direction = { forward : 0, right : 0};

//-----------------------------------------------------------------------------------
//	@brief 
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//	@brief process_key
//-----------------------------------------------------------------------------------
//	Process key event
//	data 			| structure holding key event
//	is_down == 1	| key stroked
//	is_down == 0	| key released

function process_key( key, is_down )
{
	//is_down can only be 0 or 1
	if (is_down < 0)
	{
		is_down = 0;
	}
	else if (is_down > 1)
	{
		is_down = 1;
	}
	
	//Switch: key
	switch(key)
	{
		//Increase Speed
		case "+":

			break;
		case "-":

			break;
		case "w":
		case "W":
			key_forward = is_down;
			break;
		case "s":
		case "S":
			key_backward = is_down;
			break;
		case "a":
		case "A":
			key_left = is_down;
			break;
		case "d":
		case "D":
			key_right = is_down;
			break;
		default:
			break;
	}	//End Switch: key
	
	//console.log( key, is_down );
	//console.log( key_forward, key_backward, key_left, key_right );
	
	//Process 2D direction in an object
	var new_direction = { forward : key_forward -key_backward, right : key_right -key_left };
	//Return true if direction has changed
	var f_direction_change = ((direction.forward != new_direction.forward) || (direction.right != new_direction.right))
	//Save current direction
	direction = new_direction;
	//Return current 2D direction as object with two fields
	return f_direction_change;
}	//End function: process_key


function get_direction()
{
	return direction;
}

	