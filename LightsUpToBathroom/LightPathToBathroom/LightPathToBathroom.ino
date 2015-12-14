#include <SPI.h>
#include <Ethernet.h>
//---------------------------------------//
//---- Variables connexion ethernet -----//
//---------------------------------------//
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x8A, 0x28 };
//char server[] = "37.58.162.99";
char server[] = "10.134.15.2";

IPAddress ip(10, 134, 15, 82);
EthernetClient client;
//---------------------------------------//


//---------------------------------------//
//------- Variables calcul moyenne ------//
//---------------------------------------//
bool computingAverage = false;
bool currentlySitting = false;
unsigned int average = 0;
unsigned int nbValues = 0;
unsigned long checkpoint;
unsigned int laps = 750;
int calibration = 0;
//---------------------------------------//

void setup() {
  // initialize serial communication at 9600 bits per second:
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
  Serial.print("Test de la connexion... ");

  // On essaie de se connecter à mon serveur arduino au port 3011
  if (client.connect(server, 3011)) {
    Serial.println("OK");
    // Requete HTTP
    client.println("GET /test/3/-1 HTTP/1.1");
    client.println("Host: ArduinoServer");
    client.println("Connection: close");
    client.println();
    client.stop();
  } else {
    // Connexion échouée
    Serial.println("Echec de la connexion");
  }
  //---------------------------------------------//
  // Indiquer que l'on commence le calibrage :
  sendHttpRequest(true);
  delay(250);
  sendHttpRequest(false);
  delay(250);
  sendHttpRequest(true);
  delay(250);
  sendHttpRequest(false);
  delay(250);
  sendHttpRequest(true);
  delay(250);
  sendHttpRequest(false);
  delay(250);
  sendHttpRequest(true);
  delay(250);
  sendHttpRequest(false);
  delay(250);
  //--------------- Calibrage --------------------//
  Serial.println("Veuillez vous assoir, s'il vous plait");
  analogWrite(A5, 1024*2/5);
  delay(3000);
  
  for(int i=0; i<10; i++) {
    calibration = calibration + ((analogRead(A0)+analogRead(A1))/2);
    Serial.println(analogRead(A0));
    Serial.println(analogRead(A1));
    delay(500);
  }
  calibration = calibration / 10;
  Serial.print("Calibrage : ");
  Serial.println(calibration);
  sendHttpRequestTest(calibration);
  //---------------------------------------------//
  sendHttpRequest(true);
  delay(250);
  sendHttpRequest(false);
  delay(250);
  sendHttpRequest(true);
  delay(250);
  sendHttpRequest(false);
  delay(250);
  sendHttpRequest(true);
  delay(250);
  sendHttpRequest(false);
  delay(250);
  sendHttpRequest(true);
  delay(250);
  sendHttpRequest(false);
  delay(250);
}

// the loop routine runs over and over again forever:
void loop() {
  int sensor1 = analogRead(A0);
  int sensor2 = analogRead(A1);

  sendHttpSensor1(sensor1);
  sendHttpSensor2(sensor2);
  
  detectComputingAverage(sensor1, sensor2);

  if(computingAverage)
  {
    computeAverage(sensor1, sensor2);
  }

  if(!computingAverage)
  {
    // Intervalle de prise de valeurs lorsqu'on ne calcule pas de moyenne
    delay(100);
  }
  
  delay(50);        // delay in between reads for stability
}

// ------------------------------------------------------------------------------ //
//------------------- FONCTION Calcul de la moyenne ----------------------------- //
void computeAverage(int sensor1, int sensor2)
{
  if(millis()-checkpoint > laps)
    {
      average = average/nbValues;
      Serial.print("Moyenne : ");
      Serial.println(average);
      Serial.print("Etat precedent : ");
      Serial.println(currentlySitting);
      Serial.print("Nombre de valeurs : ");
      Serial.println(nbValues);
      // On fait le traitement selon la valeur de la moyenne
      if(average > calibration-3 && average <= calibration+3)
      {
        if(!currentlySitting)
        {
          currentlySitting = true;
          Serial.println("envoi requete 'assis'");
          sendHttpRequest(currentlySitting);
        }
      }
      else
      {
        if(currentlySitting)
        {
          currentlySitting = false;
          Serial.println("Envoi requete 'pas assis'");
          sendHttpRequest(currentlySitting);
        }
      }
      // et on arrête de faire la moyenne
      computingAverage = false;
      nbValues = 0;
      average = 0;
    }
    else
    {
      // On calcule la moyenne
      nbValues = nbValues + 1;
      average = (average+(sensor1+sensor2)/2);
    }
}

// ------------------------------------------------------------------------------ //
// ------------ Détection pour démarrer à faire la moyenne ou non ----------------//
void detectComputingAverage(int sensor1, int sensor2)
{
  if((sensor1<calibration+3 && sensor1>calibration-3) && (sensor2<calibration+3 && sensor2>calibration-3) && !computingAverage && !currentlySitting)
  {
    //Faire la moyenne sur 1 secondes
    computingAverage = true;
    checkpoint = millis();
    Serial.println("Calcul moyenne pour assis");
  }

  if(sensor1 > calibration+10 && sensor2 > calibration+10 && !computingAverage && currentlySitting)
  {
    delay(750);
    computingAverage = true;
    checkpoint = millis();
    Serial.println("Calcul moyenne pour debout");
  }
}

void sendHttpRequest(bool isSitting)
{
  if (client.connect(server, 3011)) {
    Serial.println("connected");
    // Requête HTTP envoyée
    char request[40];
    sprintf(request,"GET /is_sitting/%d HTTP/1.1",isSitting);
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

void sendHttpRequestTest(int calibration)
{
  if (client.connect(server, 3011)) {
    // Requête HTTP envoyée
    char request[40];
    sprintf(request,"GET /calibration/%i HTTP/1.1",calibration);
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

void sendHttpSensor1(int sensor)
{
  if (client.connect(server, 3011)) {
    // Requête HTTP envoyée
    char request[40];
    sprintf(request,"GET /sensor1/%i HTTP/1.1",sensor);
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

void sendHttpSensor2(int sensor)
{
  if (client.connect(server, 3011)) {
    // Requête HTTP envoyée
    char request[40];
    sprintf(request,"GET /sensor2/%i HTTP/1.1",sensor);
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
