/*
  Asignatura: Sistemas Informáticos en IoT
  Practica4 - VisualizacionTablaDatosEnClienteWeb Servomotor Temperatura Control
  Funcionamiento: Visualización de tabla de datos en un cliente web que se conecta a la tarjeta ESP32 funcionando como servidor de datos del sensor BME280, 
  adicionalmente se tiene unas condiciones de control de temperatura para el movimiento angular del servomotor
  David Martín Gómez
  dmgomez@ing.uc3m.es
*/

//librerias
#include <WiFi.h>
#include <Wire.h>
#include "SparkFunBME280.h"
#include <HTTPClient.h>
#include <Arduino.h>
#include <analogWrite.h>

BME280 mySensor; // Declaración de mySensor para conexión del sensor BME280 a través de I2C

// const char* ssid = "UC3M-LABS"; //conectar tarjetaESP32 y PC a la misma red
// const char* password =  "Uc3M.L4b.2020"; //conectar tarjetaESP32 y PC a la misma red

const char* ssid = "LAPTOP-6EJ2NT4I 0425"; //conectar tarjetaESP32 y PC a la misma red
const char* password =  "16Y{22l8"; //conectar tarjetaESP32 y PC a la misma red

//const char* ssid = "Mario's Galaxy Note20 Ultra"; //conectar tarjetaESP32 y PC a la misma red
//const char* password =  ""; //conectar tarjetaESP32 y PC a la misma red

WiFiServer server(80);

#define PIN_SERVO 4 // Declarión OUTPIN para escritura sobre el servomotor

void setup()
{
  Serial.begin(230400);
  //Modo y escritura servomotor
  pinMode(PIN_SERVO, OUTPUT);

  // Wifi Setup
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP()); //print LAN IP

  // Comprobación de que el sensor BME280 funciona correctamente
  Wire.begin();
  if (mySensor.beginI2C() == false) //Begin communication over I2C
  {
    Serial.println("The sensor did not respond. Please check wiring.");
    while(1); //Freeze
  }

  // Inicio del servidor de datos en tarjeta ESP32
  server.begin();
}


void loop() {

//variable local lectura sensor
byte temp = mySensor.readTempC();

//condiciones de control
if(temp >=28 && temp <=29){
  // Move servo using angles  
    analogServo(PIN_SERVO, 45);
    delay(1000);
}else if (temp <28){
  // Move servo using angles
    analogServo(PIN_SERVO, 0);
    delay(1000);
}else if(temp >29 && temp <=30){
  // Move servo using angles
    analogServo(PIN_SERVO, 90);
    delay(1000);  
}else if(temp >30 && temp <=31){
  // Move servo using angles
    analogServo(PIN_SERVO, 135);
    delay(1000);  
}else if(temp>31 && temp<=32){
  // Move servo using angles
    analogServo(PIN_SERVO, 180);
    delay(1000);  
}else if (temp>32){
  // Move servo using angles
    analogServo(PIN_SERVO, 180);
    delay(1000);
}
// print serial info senso temp
  Serial.print(" Temp: ");
  Serial.print(mySensor.readTempC(), 2);
  Serial.println();

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");          // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<table>");
              client.println("<tr>");
                client.println("<td>");
                 client.println("Presion");
                client.println("</td>");
                
                client.println("<td>");                
                 client.println(mySensor.readFloatPressure(), 0);                                 
                 client.println(" hPa");
                client.println("</td>");
              client.println("</tr>");

              client.println("<tr>");
                client.println("<td>");
                client.println("Temperatura");
                client.println("</td>");

                client.println("<td>");                
                client.println(mySensor.readTempC(), 2);                                 
                client.println(" *C");
                client.println("</td>");

              client.println("</tr>");

              client.println("<tr>");
                client.println("<td>");
                 client.println("Altitud");
                client.println("</td>");
                
                client.println("<td>");                
                 client.println(mySensor.readFloatAltitudeMeters(), 1);                                 
                 client.println(" m");
                client.println("</td>");
              client.println("</tr>");

              client.println("<tr>");
                client.println("<td>");
                 client.println("Humedad");
                client.println("</td>");
                
                client.println("<td>");                
                 client.println(mySensor.readFloatHumidity(), 0);                                 
                 client.println(" %");
                client.println("</td>");
              client.println("</tr>");
              
            client.println("</table>");
            client.println("</html>");
            
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
