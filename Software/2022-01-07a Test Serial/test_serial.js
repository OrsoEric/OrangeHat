



var SerialPort = require("serialport");

var num = 0;

// this is the openImmediately flag [default is true]
var my_uart = new SerialPort
(
	"/dev/ttyS0",
	{
		baudRate: 256000
	},
	false
);

function build_tx_string( num )
{
	return 'P' +num +'\0';
}

var num = 0;

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
				console.log("Sending str: ", build_tx_string(num) );
				my_uart.write
				(
					build_tx_string(num),
					function(err, res)
					{
						if (err)
						{
							console.log("err ", err);
						}
						else
						{
							if (res)
							{
								console.log("results ", results);
							}
							else
							{
								console.log("write success!");
							}
						}
					}
				);
				num++;
			},
			//Send every * [ms]
			500
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
