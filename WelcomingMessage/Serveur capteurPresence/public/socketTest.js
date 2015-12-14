var socket = io();

document.getElementById('request').innerHTML = 'En attente...';

var count = 0;

socket.on('arduinoSentSomething', function () {
	count++;
    document.getElementById('request').innerHTML = 'Reçue : ' + count + ' fois';
});
