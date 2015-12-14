#include <SPI.h>
#include <Ethernet.h>

//---------------------------------------//
//---- Variables connexion ethernet -----//
//---------------------------------------//
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x8A, 0x28 };
char server[] = "10.134.15.101";
IPAddress ip(10, 134, 15, 82);
EthernetClient client;
//---------------------------------------//
int count = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //---------- Connexion ethernet ---------------//
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Echec de la configuration via DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  delay(1000);
  //---------------------------------------------//
  pinMode(2, INPUT);
  Serial.print("calibrating sensor ");
  for(int i = 0; i < 10; i++){
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" OK");
}

void loop() {
  // put your main code here, to run repeatedly:
  int sensor = digitalRead(2);

  Serial.print("sensor : ");
  Serial.println(sensor);
  if(sensor == 1 && count < 4) {
    count = count + 1;
  }
  else if(count >= 4) {
    count = 0;
    sendHttpRequest();
    delay(7000);
  }
  delay(500);
}

void sendHttpRequest()
{
  if (client.connect(server, 3010)) {
    Serial.println("connected");
    // Requête HTTP envoyée
    char request[40];
    sprintf(request,"GET /welcomeMessage HTTP/1.1");
    Serial.print("Requete : ");
    Serial.println(request);
    client.println(request);
    client.println("Host: Moi");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("connection failed");
  }
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  client.stop();
}
