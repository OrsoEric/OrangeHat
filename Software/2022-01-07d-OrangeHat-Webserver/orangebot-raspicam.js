//-----------------------------------------------------------------------------------
//	_Server
//	FatherOfMachines
//-----------------------------------------------------------------------------------
//	Original:
//	https://github.com/131/h264-live-player
//		2020-02-22
//	fix perMessageDeflate and add a websocket port
//	formatted code
//	merged redundant classes into a single cless
//	removed dependency from mout
//	resolved deprecated Buffer() warning

//Force declaration of vars
"use strict";

//-----------------------------------------------------------------------------------
//	INCLUDE
//-----------------------------------------------------------------------------------

const lib_websocket		= require("ws");
const lib_splitter		= require("stream-split");
const lib_util			= require("util");
const lib_spawn			= require("child_process").spawn;

//-----------------------------------------------------------------------------------
//	VARS
//-----------------------------------------------------------------------------------

//NAL break
const NALseparator		= new Buffer.from( [0,0,0,1] );

//-----------------------------------------------------------------------------------
//	Raspicam_browser
//-----------------------------------------------------------------------------------
//	This class encapsulates the server side websocket raspicam streamer

class Raspicam_browser
{
	//-----------------------------------------------------------------------------------
	//	Constructor
	//-----------------------------------------------------------------------------------
	//	Link canvas and raspicam configuration
	//	Link http server to attach websocket to
	
	constructor(server, width, height, framerate)
	{
		console.log("NODE Raspicam_browser | Construct | width: ", width, " | height : ", height, " | framerate : ", framerate );
		
		this.options =
		{
			width : width,
			height : height,
			fps : framerate
		};
		
		this.wss = new lib_websocket.Server({port: 8082, perMessageDeflate: false});

		this.new_client = this.new_client.bind( this );
		this.start_feed = this.start_feed.bind( this );
		this.broadcast  = this.broadcast.bind( this );

		this.wss.on('connection', this.new_client);
		
		return;
	}	//End: constructor

	//-----------------------------------------------------------------------------------
	//	start_feed
	//-----------------------------------------------------------------------------------

	start_feed()
	{
		var readStream = this.get_feed();
		this.readStream = readStream;

		readStream = readStream.pipe(new lib_splitter( NALseparator ));
		readStream.on("data", this.broadcast);
		console.log("NODE _Server : Start Feed");
		
		return;
	}	//End method: start_feed

	//-----------------------------------------------------------------------------------
	//	method: get_feed
	//-----------------------------------------------------------------------------------

	get_feed()
	{
		var msk = "raspivid -t 0 -o - -w %d -h %d -fps %d";
		var cmd = lib_util.format(msk, this.options.width, this.options.height, this.options.fps);
		
		console.log("NODE | Raspicam_browser : ", cmd);
		
		var streamer = lib_spawn('raspivid', ['-t', '0', '-o', '-', '-w', this.options.width, '-h', this.options.height, '-fps', this.options.fps, '-pf', 'baseline']);
		
		streamer.on
		(
			"exit",
			function(code)
			{
				console.log("Failure", code);
			}
		);

		return streamer.stdout;
	}	//End method: get_feed

	//-----------------------------------------------------------------------------------
	//	method: broadcast
	//-----------------------------------------------------------------------------------

	broadcast(data)
	{
		this.wss.clients.forEach
		(
			function(socket)
			{

				if(socket.buzy)
				{
					return;
				}

				socket.buzy = true;
				socket.buzy = false;

				socket.send
				(
					Buffer.concat( [NALseparator, data] ),
					{ binary: true },
					function ack( error )
					{
						socket.buzy = false;
					}
				);
			}
		);
		
		return;
	}	//End method: broadcast

	//-----------------------------------------------------------------------------------
	//	method: new_client
	//-----------------------------------------------------------------------------------

	new_client( socket )
	{
		var self = this;
		console.log("NODE Raspicam_browser: new_client");

		socket.send
		(
			JSON.stringify
			(
				{
					action : "init",
					width  : this.options.width,
					height : this.options.height,
				}
			)
		);

		socket.on
		(
			"message",
			function(data)
			{
				var cmd = "" + data, action = data.split(' ')[0];

				if(action == "REQUESTSTREAM")
				{
					self.start_feed();
				}
				if(action == "STOPSTREAM")
				{
					self.readStream.pause();
				}
				console.log("NODE Raspicam_browser: Message", data, action);
			}
		);

		socket.on
		(
			'close',
			function()
			{
				//BUG when refresh in special moments
				self.readStream.end();
				console.log('stopping client interval');
			}
		);
		
		return;
	}	//End method: new_client
};	//End class: Raspicam_browser

module.exports = Raspicam_browser;
