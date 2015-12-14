console.log("Connexion au script");

var socket = io(); //.connect('http://localhost:3000/view');

socket.on('queryUpdated', function (params) {
      document.getElementById("d").innerHTML = params['d'];
      document.getElementById("Bname").innerHTML = params['Bname'];
      document.getElementById("Baddress").innerHTML = params['Baddress'];
      document.getElementById("manufacturer").innerHTML = params['manufacturer'];
      document.getElementById("rssi").innerHTML = params['rssi'];
      document.getElementById("TxPower").innerHTML = params['TxPower'];
      document.getElementById("uuid").innerHTML = params['uuid'];
      document.getElementById("major").innerHTML = params['major'];
      document.getElementById("minor").innerHTML = params['minor'];
});