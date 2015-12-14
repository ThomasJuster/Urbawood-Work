/**
 * Copyright (c) 2009 Andrew Rapp. All rights reserved.
 *
 * This file is part of XBee-Arduino.
 *
 * XBee-Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBee-Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <XBee.h>

/*
This example is for Series 2 XBee
 Sends a ZB TX request with the value of analogRead(pin5) and checks the status response for success
*/

// create the XBee object
XBee xbee = XBee();

uint8_t payload[] = { 1, 2, 3, 4 };

// SH + SL Address of receiving XBee
//XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x403e0f30);
XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x00000000);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

int pin5 = 0;

int statusLed = 13;
int errorLed = 13;

void flashLed(int pin, int times, int wait) {

  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);

    if (i + 1 < times) {
      delay(wait);
    }
  }
}

void setup() {
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);

  Serial.begin(9600);
  xbee.setSerial(Serial);
}

void loop() {   
  // break down 10-bit reading into two bytes and place in payload
  pin5 = analogRead(A0);
  Serial.print("Capteur : ");
  Serial.println(pin5);
  payload[0] = pin5 >> 8 & 0xff;
  payload[1] = pin5 & 0xff;
  Serial.print("0 : ");
  Serial.println(payload[0]);
  Serial.print("1 : ");
  Serial.println(payload[1]);

  xbee.send(zbTx);
  uint8_t* myPayload = zbTx.getPayload();
  Serial.println();
  Serial.println("My payload");
  for(int i = 0; i < zbTx.getPayloadLength(); i++) {
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(*(myPayload+i));
  }
  Serial.println();

  // flash TX indicator
  flashLed(statusLed, 1, 100);

  // after sending a tx request, we expect a status response
  // wait up to half second for the status response
  if (xbee.readPacket(500)) {
    // got a response!
    Serial.println("Got a response");
    // should be a znet tx status            	
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);

      // get the delivery status, the fifth byte
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        Serial.println("Success");
        Serial.print("Status : ");
        Serial.println(txStatus.getDeliveryStatus());
        // success.  time to celebrate
        flashLed(statusLed, 5, 50);
      } else {
        // the remote XBee did not receive our packet. is it powered on?
        flashLed(errorLed, 3, 500);
      }
    }
  } else if (xbee.getResponse().isError()) {
    //nss.print("Error reading packet.  Error code: ");  
    //nss.println(xbee.getResponse().getErrorCode());
    Serial.println("Reponse erronnee");
  } else {
    // local XBee did not provide a timely TX Status Response -- should not happen
    Serial.println("Should not happen");
    flashLed(errorLed, 2, 50);
  }
  
  delay(2000);
}

