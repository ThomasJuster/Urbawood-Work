console.log("Connexion au script");

var socket = io(); //.connect('http://localhost:3000/view');

document.getElementById('request').innerHTML = 'En attente...';

var count = 0;

socket.on('currentlySitting', function (sitting) {
	count++;
    document.getElementById('request').innerHTML = 'Reçue : ' + count + ' fois';
    document.getElementById('sitting').innerHTML = sitting;
});

socket.on('calibrationDone', function (calibration) {
	document.getElementById('calibration').innerHTML = calibration;
});

socket.on('sensor1', function (sensor) {
	document.getElementById('sensor1').innerHTML = sensor;
});

socket.on('sensor2', function (sensor) {
	document.getElementById('sensor2').innerHTML = sensor;
});