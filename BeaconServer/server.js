var express = require('express');
var http_request = require('http');

var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var url = require('url');
var querystring = require('querystring');

var params = undefined;

var currentRoom = "living_room";
var inBellScenario = false;
var tvUrl = "http://10.134.15.103:3000/bell/";
var bathMirrorUrl = "http://10.134.15.2:4000/bell/";


io.on('connection', function (socket) {
    console.log("Someone is connected dear.");
});

http.listen(3000, function () {
    console.log('Listening on port 3000');
})


app.use(express.static('public'));

//Stocker les informations envoyées
app.get("/", function(req, res) {
    params = querystring.parse(url.parse(req.url).query);
    io.emit('queryUpdated', params);

	console.log("rssi", params.rssi);
    
    if ((currentRoom == "living_room") && (params.rssi >= -70)) {
		currentRoom = "bath_room";
	}
	
	if ((currentRoom == "bath_room") && (params.rssi < -80)) {
		currentRoom = "living_room";
	}
	
	if (inBellScenario) {
		showCamera();
	}
	
	console.log(currentRoom, inBellScenario);
   
});

// Fonction d'envoi d'une requête HTTP
function sendRequest(req) {
		http_request.get(req, function(res) {
			res.on('data', function() { /* do nothing */ });
		  console.log("Got response: " + res.statusCode);
		}).on('error', function(e) {
		  console.log("Got error: " + e.message);
		});		
}

// Montrer le portier vidéo de la porte d'entrée selon l'emplacement de l'utilisateur
function showCamera() {
	if (currentRoom == "living_room") {
		// Portier affiché sur la TV
		sendRequest(bathMirrorUrl + "0");
		sendRequest(tvUrl + "1");
	}	
	if (currentRoom == "bath_room") {
		// Portier affiché sur le miroir
		sendRequest(bathMirrorUrl + "1");
		sendRequest(tvUrl + "0");
	}
}

function hideCamera() {
	sendRequest(bathMirrorUrl + "0");
	sendRequest(tvUrl + "0");
}

app.get('/getData', function (req, res) {
    res.send(params);
});

//---- Gestions des scénarios ----//
app.get('/bell_scenario/finished', function (req, res) {
    res.send("ok");
	console.log("bell scenario finished");
	inBellScenario = false;
	hideCamera();
});


app.get('/bell_scenario/started', function (req, res) {
    res.send("ok");
	console.log("bell scenario started");
	inBellScenario = true;
	showCamera();
});

app.get('/simulate_door_bell', function(req, res) {
	sendRequest("http://10.134.15.2:3000/bell_scenario/started");
	sendRequest("http://10.134.15.2:3001/doorbell");
	sendRequest('http://10.134.15.50/axis-cgi/com/ptz.cgi?camera=1&gotoserverpresetname=Porte');
	res.send("ok");
});
