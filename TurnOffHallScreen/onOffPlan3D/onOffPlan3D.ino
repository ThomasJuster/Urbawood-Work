#include <SPI.h>
#include <Ethernet.h>
#include <IRremote.h>

IRsend irsend;

// Adresse MAC et IP du serveur Arduino
byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0x8A, 0x28};
IPAddress ip(10, 134, 15, 200); // On contacte le serveur avec cette adresse

// Port du serveur pour l'appeler
EthernetServer server(80);

void setup() {
  // Ethernet & Serveur setup
  Serial.begin(9600);
  while (!Serial){;}

  // Démarrer la connexion internet en deux temps :
  Ethernet.begin(mac, ip); // 1) Initilisation de l'Ethernet
  server.begin(); // 2) Démarrage du serveur
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  // Fin Ethernet & Serveur setup
}


void loop() {
  // Ecouter si un client se connecte
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // Lecture de la requête http. Lorsqu'elle est finie (currentLineIsBlank), on renvoie une réponse.
        if (c == '\n' && currentLineIsBlank) {
          //----- Header -----//
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          //--- Fin header ---//
          
          //----- Contenu -----//
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<p>Ecran du plan 3D mis sur ON/OFF</p>");
          client.println("</html>");
          //--- Fin contenu ---//
          for (int i = 0; i < 3; i++) {
            irsend.sendNEC(0xee1101fe, 32);
            delay(40);
          }
          //----- Envoi du signal infrarouge -----//
          //--- Fin envoi du signal infrarouge ---//
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

