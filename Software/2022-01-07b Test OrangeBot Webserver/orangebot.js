//-----------------------------------------------------------------------------------
//	HISTORY ORANGEBOT.JS
//-----------------------------------------------------------------------------------
//		2020-01-01
//	Tested remote control application on OrangeBot via Wi-Wi
//		2020-01-04
//	Include native C++ module
//		2020-01-18
//	Added C++ library version 2020-01-18
//		2020-01-19
//	Added Encoder position and speed request and feedback data
//		2020-01-23
//	Moved to standard platform messages and firmware revision 2020-01-22
//		2020-01-27
//	Handle message of type PID to configure the Speed PID gains
//		2020-01-29
//	Added PID gain handling
//	Added handling for more position and speed modes. Fast/slow are selected automatically by the motor board.
//		2020-01-31
//	Refactored PID gain structure
//		2020-02-01
//	Bugfix PID gain structure
//		2020-02-02
//	Robot status now returns AT4809 error code, PID dual error and pid dual slew rates
//		2020-02-10
//	Rounding robot direction to nearest integer
//		2020-02-12
//	Position mode moves robot by a fixed amount in [mm]
//	Reworked process_direction to handle different control modes differently
//	control message now sends 3
//		2020-02-14
//	BUGFIX: switch of control mode no longer has any artefacts
//		2020-02-16
//	Twin loop position speed controller operational
//		2020-02-17
//	@todo: Add save/load button to save PID parameters on disk
//	@todo: Ask current PID parameters from robot
//	@todo: Send configuration message with correct PID parameters to robot on startup
//	@todo: Raspivid streaming Page
//		2020-02-22
//	Constructed orangebot-raspicam clase and included live video player
//	Added raspicam video streaming to browser
//	Save and load of PID gains


//-----------------------------------------------------------------------------------
//	HISTORY INDEX.HTML
//-----------------------------------------------------------------------------------
//		2020-01-29
//	Sliders for Speed PID Gains
//	Added PID gains to Socket.IO "pid" message
//	Added selection for fast and slow speed mode and for position mode
//		2020-02-02
//	Added Error code, PID error and PID slew rate
//		2020-02-12
//	Sliders and control mode now reset upon refresh of page
//		2020-02-17
//	Save PID parameters/Load PID parameters. Parameters are mode dependent.
//	Removed websocket clickbutton and associated handler, control, event



//-----------------------------------------------------------------------------------
//	BUGS
//-----------------------------------------------------------------------------------
//		2020-01-20-Zero-Speed
//	Symptoms: Sometimes, when key remains pressed, motors stop for a moment before resumnig
//	Bug Squashing: Problem has been insulated before the serial transmission. PWMR%SL%S message becomes zero
//	Bug Squashing: Problem has been insulated after key detection script. Up/down keys are correctly processed

//-----------------------------------------------------------------------------------
//	INCLUDE
//-----------------------------------------------------------------------------------

//operating system library. Used to get local IP address
const os = require("os");
//file system library. Used to load file stored inside back end server (https://nodejs.org/api/fs.html)
const fs = require("fs");
//http system library. Handles basic html requests
const http = require("http").createServer(http_handler);
//url library. Used to process html url requests
const url = require("url");
//Websocket
const io = require("socket.io")(http);
//Websocket used to stream video
const websocket = require("ws");
//Communicate with the RPI hardware serial port on the GPIO
const SerialPort = require("serialport");
//Include native C++ module
const orangebot_platform_cpp_module = require('./build/Release/OrangebotNodeCpp.node');
console.log('My custom c++ module',orangebot_platform_cpp_module);

//orangebot_platform_cpp_module.parse("PWM:+100:+100");
//console.log("pwm0:", orangebot_platform_cpp_module.getPwm(0));

module.exports = orangebot_platform_cpp_module;

//TEST: Get robot status class Orangebot::Panopticon from the C++ library
test_parser();

function test_parser()
{
	const test_message = "F4\0ERR\0";
	console.log( "Parse ERR signature, ", test_message );
	orangebot_platform_cpp_module.parse( test_message );
	var robot_status = orangebot_platform_cpp_module.get_status();
	console.log("Parsed Signature: ",robot_status.signature );
	if (robot_status.signature != "ERR")
	{
		console.log("Parser test: SUCCESS");
	}
	else
	{
		console.log("NODE ERR: Failed to parse test signature\n");
	}
	
	return;
}

//-----------------------------------------------------------------------------------
//	CONFIGURATION
//-----------------------------------------------------------------------------------

//Port the server will listen to
var server_port = 8080;
var websocket_stream_port = 8082;
//Path of the http and css files for the http server
var file_index_name = "index.html";
var file_script_key_name = "orangebot_key.js";
var g_file_player_name = "http-live-player.js";
//Http and css files loaded into memory for fast access
var file_index;
var file_script_key;
var g_file_player;
//Name of the local video stream
var stream_name = "mystream";
//Time between emission of data to browser
const time_send_robot_data_to_browser = 333
//Time between emission of serial messages to the robot electronics
const time_send_serial_messages = 500

//-----------------------------------------------------------------------------------
//	DEBUG
//-----------------------------------------------------------------------------------

//Enable runtime pedantic log messages
var g_enable_pedantic_log = false;
//Enable runtime log messages
var g_enable_log = true;

//-----------------------------------------------------------------------------------
//	MOTOR VARS
//-----------------------------------------------------------------------------------

//Speed of the right and left wheels. Arbitrary unit. range from -max_velocity to +max_velocity integer
var g_right = 0;
var g_left = 0;
//Minimum and maximum speed allowed
var min_velocity = 0;
var max_velocity = 30;
//Default speed factor in PWM mode
var g_pwm_factor = 70;
//Default speed factor in speed mode
var g_spd_factor = 1;
//Default for the position mode
var g_pos_factor = 10;
//Ratio of forward to sideways during turn. 0 full turn, 1 full forward
var steering_ratio = 0.7;
//Current direction of the platform
var direction = { forward : 0, right : 0 };
//Control System parameters
var g_pid =
{
	ctrl_mode : "off",
	spd_change : false,
	pos_change : false,
	spd_kp : 0,
	spd_ki : 0,
	spd_kd : 0,
	pos_kp : 0,
	pos_ki : 0,
	pos_kd : 0
}
//Store the robot status
var g_robot_status =
{
	signature : "NULL",
	AT4809ERR : 0,
	AT4809CPU : 0,
	AT4809RXI : 0,
	AT4809TXO : 0,
	pwm : [0, 0, 0, 0],
	enc_pos : [0, 0],
	enc_spd : [0, 0],
	pid_err : [0, 0],
	pid_slew_rate : [0, 0]
}
//Target for the position controller
var g_target_pos = [0, 0];

//-----------------------------------------------------------------------------------
//	ENCODER VARS
//-----------------------------------------------------------------------------------

//Number of encoder installed on the platform
const num_enc = 2;
//Ask one encoder at a time
var scan_encoder_index = 0

//-----------------------------------------------------------------------------------
//	PLATFORM VARS
//-----------------------------------------------------------------------------------

//Mechanical constants of the platform
const g_platform =
{	
	enc_res_cnt_mm : 15.79,
	enc_res : 6300,
	interaxis : 100,
	reducer : 5.25,
	wheel : 127
}

//-----------------------------------------------------------------------------------
//	DETECT SERVER OWN IP
//-----------------------------------------------------------------------------------

//If just one interface, store the server IP Here
var server_ip;
//Get local IP address of the server
//https://stackoverflow.com/questions/3653065/get-local-ip-address-in-node-js
var ifaces = os.networkInterfaces();

Object.keys(ifaces).forEach
(
	function (ifname)
	{
		var alias = 0;

		ifaces[ifname].forEach
		(
			function (iface)
			{
				if ('IPv4' !== iface.family || iface.internal !== false)
				{
				  // skip over internal (i.e. 127.0.0.1) and non-ipv4 addresses
				  return;
				}

				if (alias >= 1)
				{
					// this single interface has multiple ipv4 addresses
					console.log('INFO: Server interface ' +alias +' - ' + ifname + ':' + alias, iface.address);
				}
				else
				{
					server_ip = iface.address;
					// this interface has only one ipv4 adress
					console.log('INFO: Server interface - ' +ifname, iface.address);
				}
				++alias;
			}
		);
	}
);

//-----------------------------------------------------------------------------------
//	HTTP SERVER
//-----------------------------------------------------------------------------------
//	Fetch and serves local files to client

//Create http server and listen to the given port
http.listen
(
	server_port,
	function( )
	{
		console.log('INFO: ' +server_ip +' listening to html requests on port ' +server_port);
		//Pre-load http, css and js files into memory to improve http request latency
		file_index = load_file( file_index_name );
		file_script_key = load_file( file_script_key_name );
		g_file_player = load_file( g_file_player_name );
	}
);

//-----------------------------------------------------------------------------------
//	HTTP REQUESTS HANDLER
//-----------------------------------------------------------------------------------
//	Answer to client http requests. Serve http, css and js files

function http_handler(req, res)
{
	//If client asks for root
	if (req.url == '/')
	{
		//Request main page
		res.writeHead( 200, {"Content-Type": detect_content(file_index_name),"Content-Length":file_index.length} );
		res.write(file_index);
		res.end();

		console.log("INFO: Serving file: " +req.url);
	}
	//If client asks for css file
	else if (req.url == ("/" +file_script_key_name))
	{
		//Request main page
		res.writeHead( 200, {"Content-Type": detect_content(file_script_key_name),"Content-Length" :file_script_key.length} );
		res.write(file_script_key);
		res.end();

		console.log("INFO: Serving file: " +req.url);
	}
	//If client asks for the livestream player
	else if (req.url == "/" +g_file_player_name)
	{
		//Request main page
		res.writeHead( 200, {"Content-Type": detect_content(g_file_player_name),"Content-Length":g_file_player.length} );
		res.write(g_file_player);
		res.end();

		console.log("INFO: Serving file: " +req.url);
	}
	//If client asks for an unhandled path
	else
	{
		res.end();
		console.log("NODE ERR: Invalid file request" +req.url);
	}
}

//-----------------------------------------------------------------------------------
//	VIDEO STREAMING
//-----------------------------------------------------------------------------------

const WebStreamerServer = require( "./orangebot-raspicam" );
const silence = new WebStreamerServer( http, 640, 480, 30 );

//-----------------------------------------------------------------------------------
//	WEBSOCKET SERVER: CONTROL/FEEDBACK REQUESTS
//-----------------------------------------------------------------------------------
//	Handle websocket connection to the client

io.on
(
	"connection",
	function (socket)
	{
		console.log("connecting...");

		// @todo security keys negotiation
		socket.emit("welcome", { payload: "Server says hello" });

		//Load PID gains from file
		g_pid = file_load_pid();
		//Send PID gains to client
		socket.emit("set_pid_gain", g_pid );

		//Periodically send the current server time to the client in string form
		setInterval
		(
			function()
			{
				//Fetch the object containing the current robot status vars
				var robot_status = orangebot_platform_cpp_module.get_status();
				//Send the object directly to the browser
				socket.emit("robot_status", robot_status );
				
				g_robot_status = robot_status;
			},
			//Send periodically
			time_send_robot_data_to_browser
		);

		//Keys are processed by the html running on the browser
		socket.on
		(
			"direction",
			function (data)
			{
				timestamp_ms = get_timestamp_ms();
				//socket.emit("profile_ping", { timestamp: timestamp_ms });
				//Process the direction coming from the platform
				[g_right, g_left] = process_direction( g_pid.ctrl_mode, data );
				//Instantly forward the direction message to minimize latency
				send_robot_direction_message( g_pid.ctrl_mode, g_right, g_left );
			}
		);

		//PID Set parameter message
		socket.on
		(
			"pid",
			function (data)
			{
				console.log("NODE: pid message | control mode: ", data.ctrl_mode, "previous control mode: ", g_pid.ctrl_mode, data);
				//If: there was a control mode change
				if (data.ctrl_mode != g_pid.ctrl_mode)
				{
					console.log("NODE: reset references");
					
					//Position controller must be initialized to the current absolute position
					if (data.ctrl_mode == "pos")
					{
						if (typeof g_robot_status === "undefined")
						{
							g_target_pos[0] = 0;
							g_target_pos[1] = 0;
							console.log("NODE ERR: failed to load encoder position for position controller");
						}
						else
						{
							//Snap target for the position controller
							g_target_pos[0] = g_robot_status.enc_pos[0];
							g_target_pos[1] = g_robot_status.enc_pos[1];
							console.log("NODE: Reset target position", g_target_pos[0], g_target_pos[1]);
						}
					}
					//Reset references for good measure
					[g_right, g_left] = process_direction(data.ctrl_mode, {forward: 0, right : 0} );
				}
				//If gain for the speed PID has changed
				if (data.spd_change == true)
				{
					//Send Speed PID setting
					send_message_set_spd_pid( data );
				}
				//If gain for the position PID has changed
				if (data.pos_change == true)
				{
					//Send Speed PID setting
					send_message_set_pos_pid( data );
				}
				//Update the PID gain structure from the client
				g_pid = data;
				//Pedantic debug
				if (g_enable_pedantic_log == true)
				{
					console.log("PID message: PID:", g_pid );
				}
			}
		);
		
		socket.on
		(
			"pid_btn",
			function (data)
			{
				//Switch: Button action
				switch(data)
				{
					//Reset sliders to zero and send update
					case "reset":
						//NODE doesn't have to do anything here.
						break;
					case "save":
						//Save PID gains on file
						file_save_pid( g_pid );
					
						break;
					case "load":
						//Load PID gains from file
						g_pid = file_load_pid();
						//Send PID gains to client
						socket.emit("set_pid_gain", g_pid );
						
						break;
					default:
						console.log("HTML: ERR | bad buttonaction | ", data);
					
				}
				
				
				timestamp_ms = get_timestamp_ms();
				socket.emit("profile_ping", { timestamp: timestamp_ms });
				console.log("button event: " +" client says: " +data.payload);
			}
		);

		//profile packets from the client are answer that allows to compute roundway trip time
		socket.on
		(
			"profile_pong",
			function (data)
			{
				timestamp_ms_pong = get_timestamp_ms();
				timestamp_ms_ping = data.timestamp;
				console.log("Pong received. Round trip time[ms]: " +(timestamp_ms_pong -timestamp_ms_ping));
			}
		);
	}
);

//-----------------------------------------------------------------------------------
//	SERIAL PORT
//-----------------------------------------------------------------------------------
//	Communication with the HotBlack Hat through GPIO

//Connect to the serial port on th GPIO
var my_uart = new SerialPort
(
	"/dev/ttyS0",
	{
		baudRate: 256000,
		openImmediately: true
	},
	false
);

//-----------------------------------------------------------------------------------
//	SERIAL PORT HANDLER
//-----------------------------------------------------------------------------------

//Detect port open
my_uart.on
(
	"open",
	function()
	{
		console.log("Port is open!");
		//Initialize robot communication
		robot_communication_init();
	}
);

//Data from hat. Currently does not work neither in HW nor in SW.
my_uart.on
(
	"data",
	function(data)
	{
		console.log( "RX: ", data.toString() );
		//Feed data to C++ module that takes care of decoding the serial stream
		orangebot_platform_cpp_module.parse( data.toString() );
	}
);

//-----------------------------------------------------------------------------------
//	FUNCTIONS
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//	INIT RPI3B+ <-> Serial Robot Board Communication
//-----------------------------------------------------------------------------------

function robot_communication_init()
{
	/* @todo wait until a valid signature is found
	//While: The signature is invalid
	while (robot_status.signature == "ERR")
	{
		//Ask robot for firmware signature
		send_message_signature_request();
		//Wait before sending out a new request
	}
	*/

	//Ask robot for firmware signature
	send_message_signature_request();

	//Ask for periodic update about AT8090 board performance
	send_message_cpu_load_request();

	//Periodically send the current server time to the client in string form
	setInterval
	(
		function()
		{
			//Ask for the next encoder position
			send_message_abs_enc_request( scan_encoder_index );
			//Scan next encoder
			scan_encoder_index++;
			if (scan_encoder_index >= num_enc)
			{
				scan_encoder_index = 0;
			}
			//Ask for the next encoder speed
			send_message_enc_spd_request();
			//Show decoded
			console.log( orangebot_platform_cpp_module.get_status() );
			//Control the plaform
			send_robot_direction_message( g_pid.ctrl_mode, g_right, g_left );
		},
		//Send periodically speed to the motors
		time_send_serial_messages
	);
}

//-----------------------------------------------------------------------------------
//	SERVER DATE&TIME
//-----------------------------------------------------------------------------------
//	Get server time in string form

function get_server_time()
{
	my_date = new Date();

	return my_date.toUTCString();
}

//-----------------------------------------------------------------------------------
//	TIMESTAMP
//-----------------------------------------------------------------------------------
//	Profile performance in ms

function get_timestamp_ms()
{
	my_date = new Date();
	return 1000.0* my_date.getSeconds() +my_date.getMilliseconds()
}

//-----------------------------------------------------------------------------------
//	FILE LOADER
//-----------------------------------------------------------------------------------
//	Load files into memory for improved latency

function load_file( file_name )
{
	var file_tmp;
	var file_path =  __dirname +"/" +file_name;

	//HTML index file
	try
	{
		file_tmp = fs.readFileSync( file_path );
	}
	catch (err)
	{
		console.log("NODE ERR: " +err.code +" failed to load: " +file_path);
		throw err;
	}

	console.log("INFO: " +file_path +" has been loaded into memory");

	return file_tmp;
}

//-----------------------------------------------------------------------------------
//	CONTENT TYPE DETECTOR
//-----------------------------------------------------------------------------------
//	Return the right content type to give correct information to the client browser

function detect_content( file_name )
{
	if (file_name.includes(".html"))
	{
        return "text/html";
	}
	else if (file_name.includes(".css"))
	{
		return "text/css";
	}
	else if (file_name.includes(".js"))
	{
		return "application/javascript";
	}
	else
	{
		throw "invalid extension";

	}
}

//-----------------------------------------------------------------------------------
//	DIRECTION
//-----------------------------------------------------------------------------------

//Compute pwm from direction
function process_direction( ctrl_mode, direction )
{
	//console.log( "arguments: ", direction.forward, direction.right );
	var forward = direction.forward;
	var right = direction.right;
	//Vector speed
	var velocity = 0;

	var pwm_right = 0;
	var pwm_left = 0;

	console.log("NODE | mode: " , ctrl_mode, "process_direction : ", forward, right );

	//Processing depends on control mode
	switch(ctrl_mode)
	{
		case "off":
		{
			console.log("NODE: switch to off");
			//do nothing
			break;
		}
		case "pwm":
		{
			console.log("NODE: switch to pwm");
			//Fetch speed for this mode
			velocity = g_pwm_factor;
		}
		case "slow_spd":
		{
			console.log("NODE: switch to slow_spd");
		}
		case "fast_spd":
		{
			console.log("NODE: switch to fast_spd");
			//If velocity was not overwritten by other modes
			if (velocity == 0)
			{
				//Fetch speed for this mode
				velocity = g_spd_factor;
			}
			//compute PWM
			pwm_right = forward *velocity -right*velocity *steering_ratio;
			pwm_left = forward *velocity +right *velocity *steering_ratio;

			//If right wheel exceed the overal maximum speed
			if (pwm_right > velocity)
			{
				//Push the difference on the other wheel in opposite direction
				pwm_right = velocity;
				pwm_left = pwm_left -pwm_right +velocity;

			}
			else if (pwm_right < -velocity)
			{
				//Push the difference on the other wheel in opposite direction
				pwm_right = -velocity;
				pwm_left = pwm_left -pwm_right -velocity;

			}

			//If right wheel exceed the overal maximum speed
			if (pwm_left > velocity)
			{
				//Push the difference on the other wheel in opposite direction
				pwm_left = velocity;
				pwm_right = pwm_right -pwm_left +velocity;

			}
			else if (pwm_left < -velocity)
			{
				//Push the difference on the other wheel in opposite direction
				pwm_left = -velocity;
				pwm_right = pwm_right -pwm_left -velocity;

			}
			break;
		}
		case "pos":
		{
			console.log("NODE: switch to pos");
			//Fetch displacement for this mode
			var displacement = g_pos_factor;
			//Fetch current encoder position
			pwm_right = g_target_pos[0];
			pwm_left = g_target_pos[1];
			console.log("displacement: ", displacement, "Origin: ", pwm_right, pwm_left);
			//Create a target position
			pwm_right = pwm_right +forward *g_platform.enc_res_cnt_mm *displacement;
			pwm_left = pwm_left +forward *g_platform.enc_res_cnt_mm *displacement;
			pwm_right = pwm_right +right *steering_ratio *g_platform.enc_res_cnt_mm *displacement;
			pwm_left = pwm_left +right *steering_ratio *g_platform.enc_res_cnt_mm *displacement;
			//Write back new target position
			g_target_pos[0] = pwm_right;
			g_target_pos[1] = pwm_left;
			console.log("Destination: ", pwm_right, pwm_left);
			// code block
			break;
		}
		
		default:
		{
			console.log("NODE ERR: unknown control mode");
		}
	} 
	
	//Round the result
	pwm_left = Math.round( pwm_left );
	pwm_right = Math.round( pwm_right );

	//Return processed PWM
	return [pwm_right, pwm_left]
}

//-----------------------------------------------------------------------------------
//	SEND SERIAL PORT MESSAGES
//-----------------------------------------------------------------------------------

//Uart wrapper to handle failure in transmission
function uart_transmit( message )
{
	//Send message through UART
	my_uart.write
	(
		message,
		function(err, res)
		{
			if (err)
			{
				console.log("NODE ERR | UART ERR: ", err);
			}
			else
			{
				console.log("TX: ", message);
			}
		}
	);

	return;
}

//Send ping message to keep the connection alive
function message_ping( )
{
	//Send message through UART
	uart_transmit( "P\0" );

	return;
}

//Ask for signature
function send_message_signature_request( )
{
	//Send message through UART
	uart_transmit( "F\0" );

	return;
}

//Ask for microcontroller CPU Load
function send_message_cpu_load_request( )
{
	//Construct encoder absolute position request
	var message = "GET_PERFORMANCE" +"1" +"\0";
	//Send message through UART
	uart_transmit( message );

	return;
}

//Ask for encoder absolute position
function send_message_abs_enc_request( scan_encoder_index )
{
	//Construct encoder absolute position request
	var message = "ENC_ABS" + scan_encoder_index + "\0";
	//Send message through UART
	uart_transmit( message );
}

//Ask for encoder speed
function send_message_enc_spd_request()
{
	//Construct encoder absolute position request
	var message = "ENC_SPD\0";
	//Send message through UART
	uart_transmit( message );
}

//Send the appropriate robot control target message
function send_robot_direction_message( ctrl_mode, target_right, target_left )
{
	//Debug
	if (g_enable_log == true)
	{
		console.log("NODE | send_robot_direction_message: ", ctrl_mode, target_right, target_left );
	}
	
	//switch: Processing depends on control mode
	switch(ctrl_mode)
	{
		case "off":
		{
			//Send PWM message. Automatically switch to PWM mode
			send_message_set_pwm_dual( 0, 0 );

			break;
		}
		case "pwm":
		{
			//Send PWM message. Automatically switch to PWM mode
			send_message_set_pwm_dual( target_right, target_left );
			break;
		}
		case "slow_spd":
		{
			//Send SPD message. Automatically switch to SPD mode
			send_message_set_spd_dual( target_right, target_left );
			break;
		}
		case "fast_spd":
		{
			//Send SPD message. Automatically switch to SPD mode
			send_message_set_slow_spd_dual( target_right, target_left );
			break;
		}
		case "pos":
		{
			//Send POS message. Automatically switch to SPD mode
			send_message_set_pos_dual( target_right, target_left );
			break;
		}
		default:
		{
			console.log("NODE ERR: unknown control mode");
		}
	} 	//end switch: Processing depends on control mode
	
	return;
}

//Compute the PWM set message to send the Robot Board
function send_message_set_pwm_dual( pwm_r, pwm_l )
{
	//Construct PWM Message
	var message = "ROBOT_PWM" + pwm_r + ":" + pwm_l + "\0";
	//Send message through UART
	uart_transmit( message );
}

//Compute the SPD set message to send the Robot Board
function send_message_set_spd_dual( spd_r, spd_l )
{
	//Add plus signs when needed
	var message = "ROBOT_SPD" + spd_r + ":" + spd_l + "\0";
	//Send message through UART
	uart_transmit( message );
}

//Compute the SPD set message to send the Robot Board
function send_message_set_slow_spd_dual( spd_r, spd_l )
{
	//Add plus signs when needed
	var message = "ROBOT_SSPD" + spd_r + ":" + spd_l + "\0";
	//Send message through UART
	uart_transmit( message );
}

//Compute the SPD set message to send the Robot Board
function send_message_set_pos_dual( pos_r, pos_l )
{
	//Add plus signs when needed
	var message = "ROBOT_POS" + pos_r + ":" + pos_l + "\0";
	//Send message through UART
	uart_transmit( message );
}

//The RPI wants to change the speed PID parameters
function send_message_set_spd_pid( pid )
{
	//Add plus signs when needed
	var message = "SET_SPD_PID" + pid.spd_kp + ":" + pid.spd_ki + ":" + pid.spd_kd + "\0";
	//Send message through UART
	uart_transmit( message );
}

//The RPI wants to change the speed PID parameters
function send_message_set_pos_pid( pid )
{
	//Add plus signs when needed
	var message = "SET_POS_PID" + pid.pos_kp + ":" + pid.pos_ki + ":" + pid.pos_kd + "\0";
	//Send message through UART
	uart_transmit( message );
}

//-----------------------------------------------------------------------------------
//	FILE OPERATION
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//	SAVE PID GAIN
//-----------------------------------------------------------------------------------
//	Save a PID structure on file

function file_save_pid( pid )
{	
	var data = JSON.stringify( pid );
	fs.writeFileSync("pid_gain.json", data);
	console.log("NODE file_save_pid | ", data );
	
	return;
}

//-----------------------------------------------------------------------------------
//	LOAD PID GAIN
//-----------------------------------------------------------------------------------
//	Load PID gains from file

function file_load_pid( pid )
{	
	
	var data  = fs.readFileSync("pid_gain.json");
	var pid = JSON.parse(data);
	console.log("NODE file_load_pid | ", data );
	
	return pid;
}