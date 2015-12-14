var express = require('express');
var app = express();

var http = require('http').Server(app);
var io = require('socket.io')(http);

var params = undefined;
//-------------------------------------------//

//app.use(express.static(__dirname + 'public'));
app.use('/is_sitting', express.static(__dirname + '/public'));

io.on('connection', function (socket) {
    console.log("Un utilisateur s'est connecté.");
});

app.get('/is_sitting', function (req, res) {
	res.sendfile('public/sitting.html');
});


http.listen(3001, null, function () {
    console.log('Serveur lancé sur le port 3001');
});


//------------- GESTION DES LED HUE ---------//
//--- Variables pour allumer la lampe Hue ---//
var lightHost = '10.134.15.60';
var lightUser = "/api/newdeveloper";
var targetedLight = "/lights/4/state";
var lightOn = true;
var postData;

//--- Url pour allumer/éteindre la lampe Hue ---//
app.get('/hue/test', function (req, res) {
	res.send(200, 'Test Hue');

	postData = '{"on": '+lightOn.toString()+'}';
	sendPutRequest(lightHost, lightUser+targetedLight, postData);

	if(lightOn) lightOn = false;
	else lightOn = true;
});

// Fonction appelée par l'Arduino pour allumer ou éteindre les LED
var lightUpThePath = function (putLightOn) {
	postData = '{"on": '+ putLightOn.toString() +'}';
	sendPutRequest(lightHost, lightUser+targetedLight, postData);
};


//--------- ENVOI DE LA REQUETE "PUT" HUE --------//
var sendPutRequest = function (myHost, myPath, myData, myPort) {
	
	var http = require('http');

	var options = {
	  hostname: myHost,
	  port: myPort,
	  path: myPath,
	  method: 'PUT',
	  headers: {
	    'Content-Type': 'application/x-www-form-urlencoded',
	    'Content-Length': postData.length
	  }
	};

	var req = http.request(options, function(res) {
	  res.on('data', function (chunk) {
	  	//récupérer la réponse si on est intéressé
	  });
	  res.on('end', function() {
	    //Fin du transfert des données de la réponse.
	  })
	});

	req.on('error', function(e) {
	  console.log('Requête échouée : ' + e.message);
	});

	// write data to request body
	req.write(postData);
	req.end();
};
//------------------------------------------------//


//--- VARIABLES ARDUINO ---//
var outOfBedCount = 0;
var inBed = true;
lightUpThePath(false); // On initialise le chemin lumineux à 'éteint'

//-------------- REQUETES ARDUINO ----------------//
// Requêtes appelées par l'Arduino pour permettre de les monitorer via '/is_sitting'
app.get('/calibration/:calibration', function (req, res) {
	res.send(200);
	console.log('Calibration : ' + req.params.calibration);
	io.emit('calibrationDone', req.params.calibration);
});

app.get('/sensor1/:sensor', function (req, res) {
	res.send(200);
	io.emit('sensor1', req.params.sensor);
});

app.get('/sensor2/:sensor', function (req, res) {
	res.send(200);
	io.emit('sensor2', req.params.sensor);
});

app.get('/is_sitting/:stepped', function (req, res) {
	res.send('Requete arduino emise');

	console.log('Stepped : ' + req.params.stepped);
	if(req.params.stepped == 1) {
		if(inBed) {
			inBed = false; // L'usager vient de sortir du lit.
			outOfBedCount++;
			// Allumer la lampe
			lightUpThePath(true);
		}
		else {
			inBed = true; // L'usager va se recoucher.
			// Eteindre la lampe
			lightUpThePath(false);
		}
		console.log('Dans le lit : ' + inBed.toString());
	}
	io.emit('currentlySitting', req.params.sitted);
});

//-------------- FIN REQUETES ARDUINO ------------//