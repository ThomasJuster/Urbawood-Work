var express = require('express');
var app = express();
var os = require('os');

var port = 3004;


// App
app.use(express.static('public'));

app.get('/setScreen/off', function (req, res) {
	res.send(200);
	if(os.platform() == 'win32') {
		launchBatchFile('setScreenOff.bat');
	}
});

app.get('/setScreen/on', function (req, res) {
	res.send(200);
	if(os.platform() == 'win32') {
		launchBatchFile('setScreenOn.bat');
	}
});

app.listen(port, function () {
	console.log('Serveur démarré au port ' + port);
});



var launchBatchFile = function (path) {
	var spawn = require('child_process').spawn,
	ls = spawn('cmd.exe', ['/c', path]);

	ls.stdout.on('data', function (data) {
	console.log('stdout: ' + data);
	});

	ls.stderr.on('data', function (data) {
		console.log('stderr: ' + data);
	});

	ls.on('exit', function (code) {
		console.log('child process exited with code ' + code);
	});
};