var express = require('express');
var Player = require('player');

var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var port = 3010;


// Player
var player = new Player('/home/isen/ArduinoServer/public/welcomeMessage.mp3'); // Placer ici le message d'accueil

player.on('error', function (err) {
	console.log('Player error : '+err);
});


// Socket.io
io.on('connection', function (socket) {
	console.log('Socket : ' + socket);
});


// App
app.use('/arduino', express.static(__dirname + '/public'));

app.get('/welcomeMessage', function (req, res) {
	res.send('Envoi de la requête pour déclencher le player');
	// play again
	player.play(function(err, player){
		 console.log(err, 'playend!');	
	});
	// Permet de vérifier la connexion :
	io.emit('arduinoSentSomething');
});



http.listen(port, null, function () {
    console.log('Serveur initialisé sur le port ' + port);
});