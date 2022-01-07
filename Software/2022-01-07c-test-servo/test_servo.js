//		2020-02-25
//	Test NODE.JS application for the servo motors installed on board of OrangeBot
//		2021-01-07
//  OrangeHat electronics


var SerialPort = require("serialport");

// this is the openImmediately flag [default is true]
var my_uart = new SerialPort
(
	"/dev/ttyS0",
	{
		baudRate: 256000
	},
	false
);

//Test cycle settings
var num_motors = 2;
var max_speed = 100;
var speed_increment = 10;

//Cycle counters
var ramp = 0;
var motor = 0;
var speed = 0;

my_uart.on
(
	"open",
	function()
	{
		console.log("Port is open!");
		//Periodically send the current server time to the client in string form
		setInterval
		(
			function()
			{
				//----------------------------------------------
				//	Motor Ramp Generator
				//----------------------------------------------

				//if: accelerate
				if (ramp == 0)
				{
					//increase speed
					speed += speed_increment;
					//if maximum speed
					if (speed >= max_speed)
					{
						//clip speed
						speed = max_speed;
						//decelerate
						ramp = ramp +1;
					}
				}
				//if: decelerate
				else if (ramp == 1)
				{
					//decrease speed
					speed -= speed_increment;
					//if minimum
					if (speed <= -max_speed)
					{
						//clip speed
						speed = -max_speed;
						//accelerate
						ramp = ramp +1;
					}
                }
                else if (ramp == 2)
                {
                    //increase speed
					speed += speed_increment;
					//if maximum speed
					if (speed >= 0)
					{
						//clip speed
						speed = 0;
						//next motor
						ramp = ramp +1;
					}
                }
                else
                {
                    //Reset Scan
                    ramp = 0;
                    //Reset speed
                    speed = 0;
                    //Scan motors
                    motor++;
                    //clip to maximum number of motors
                    if (motor >= num_motors)
                    {
                        motor = 0;
                    }
                }

				send_servo_msg( motor, speed );

			},
			//Send every * [ms]
			300
		);
	}
);

my_uart.on
(
	'data',
	function(data)
	{
		console.log('data received: ' + data);
	}
);

//Send ping message to keep the connection alive
function send_ping( )
{
	my_uart.write
	(
		"P\0",
		function(err, res)
		{
			if (err)
			{
				console.log("err ", err);
			}
		}
	);
}

function send_servo_msg( servo_index, pos )
{
	
	var msg;
    //  PPM%u:%S:%S
    //  index
    //  position in us
    //  speed in us/s
	msg = "PPM" + servo_index + ":" + pos + ":" + 1000 + "\0";
	
	my_uart.write
	(
		msg,
		function(err, res)
		{
			if (err)
			{
				console.log("err ", err);
			}
			else
			{
				console.log("Sent: ", msg);
			}
		}
	);
	
	
}
