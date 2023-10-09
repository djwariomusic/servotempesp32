/*
  Asignatura: Sistemas Informáticos en IoT
  Practica4 - VisualizacionTablaDatosEnClienteWeb Servomotor Temperatura Control
  Funcionamiento: Visualización de tabla de datos en un cliente web que se conecta a la tarjeta ESP32 funcionando como servidor de datos del sensor BME280, 
  adicionalmente se tiene unas condiciones de control de temperatura para el movimiento angular del servomotor
  David Martín Gómez
  dmgomez@ing.uc3m.es
*/

// Librerías
#include <WiFi.h>                           // WiFi
#include <Wire.h>                           // Sensor of temperature sensor BME280
#include "SparkFunBME280.h"                 // Sensor of temperature sensor BME280
#include <analogWrite.h>                    // Handle servo
#include <WebSocketsServer.h>               // git https://github.com/Links2004/arduinoWebSockets

#define PIN_SERVO 4                         // Declarión OUTPIN para escritura sobre el servomotor

const char* ssid = "LAPTOP-6EJ2NT4I 0425";  // Conectar tarjetaESP32 y PC a la misma red
const char* password = "16Y{22l8";          // Conectar tarjetaESP32 y PC a la misma red

BME280 mySensor;                            // Declaración de mySensor para conexión del sensor BME280 a través de I2C
WebSocketsServer webSocket = WebSocketsServer(81);
WiFiServer WebServer(80);

void setup() {
  setupSerial();
  setupWifi();
  setupSensorTemperature();
  setupServo();
  setupServerWebSocket();
  setupServerWeb();
}

void setupSerial(){
  Serial.begin(230400);
  Serial.println("[serial] is ok");
}

void setupWifi(){
  WiFi.begin(ssid, password);

  // Wait some time to connect to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP());           // Print LAN IP
  Serial.println("[wifi] is ok");
}

void setupSensorTemperature(){
  // Comprobación de que el sensor BME280 funciona correctamente
  Wire.begin();
  if (mySensor.beginI2C() == false) //Begin communication over I2C
  {
    Serial.println("[sensor temperature] did not respond. Please check wiring.");
    while(1); //Freeze
  }else{
    Serial.println("[servo temperature] is ok");
  }
}

void setupServo(){
  //Modo y escritura servomotor
  pinMode(PIN_SERVO, OUTPUT);
}

void setupServerWebSocket(){
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void setupServerWeb(){
  WebServer.begin();
}

// Called when receiving any WebSocket message
void onWebSocketEvent(uint8_t num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {
  // Figure out the type of WebSocket event
  switch(type) {
    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
      }
      break;

    // Echo text message back to client
    case WStype_TEXT:
      {
          char temp[] = "";
          dtostrf(mySensor.readTempC(), 4, 2, temp);
          String tempWithUnits = '"' + String(temp) + " *C" + "\",";

          char altitud[] = "";
          dtostrf(mySensor.readFloatAltitudeMeters(), 4, 1, altitud);
          String altitudWithUnits = "\""+ String(altitud) + " m" +"\",";

          String humedadUnits = "\"" + String(mySensor.readFloatHumidity()) + " %" +"\"";

          String json = "{\"temperatura\":" + tempWithUnits + "\"presion\":" + String("\"" + String(mySensor.readFloatPressure()) + " hPa\",") + "\"altitud\":" + altitudWithUnits + "\"humedad\":" + humedadUnits + "}";
          Serial.println(json);
          
          webSocket.sendTXT(num, json);
      }
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

void loop() {
  handleServo();
  renderWebPage();

  // Look for and handle WebSocket data
  webSocket.loop();
}

/*
  Handle servo according to temperature of sensor
*/
void handleServo(){
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
}

void renderWebPage(){
  WiFiClient client = WebServer.available();   // listen for incoming clients

  if (client) {
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
            
            // Script
            client.println("<script>");
              client.println("let webSocket = new WebSocket('ws://'+ window.location.host + ':81');");
                client.println("webSocket.onopen = () => {");
                
                client.println("setInterval( () => {");
                  client.println("webSocket.send('Request data');");

                  client.println("webSocket.onmessage = function(data) {"); 
                    client.println("let dataJSON = JSON.parse(data.data);");
                    client.println("let presion = document.getElementById('presion');");
                    client.println("presion.innerHTML = dataJSON.presion");

                    client.println("let temperatura = document.getElementById('temperatura');");
                    client.println("temperatura.innerHTML = dataJSON.temperatura");
                    
                    client.println("let altitud = document.getElementById('altitud');");
                    client.println("altitud.innerHTML = dataJSON.altitud");

                    client.println("let humedad = document.getElementById('humedad');");
                    client.println("humedad.innerHTML = dataJSON.humedad");
                  client.println("}");

                client.println("}, 1000)");

                client.println("}");
            client.println("</script>");

            client.println("<table>");
              client.println("<tr>");
                client.println("<td id="">");
                 client.println("Presion");
                client.println("</td>");
                
                client.println("<td id=\"presion\">");                
                 client.println(mySensor.readFloatPressure(), 0);                                 
                 client.println(" hPa");
                client.println("</td>");
              client.println("</tr>");

              client.println("<tr>");
                client.println("<td>");
                client.println("Temperatura");
                client.println("</td>");

                client.println("<td id=\"temperatura\">");                
                client.println(mySensor.readTempC(), 2);                                 
                client.println(" *C");
                client.println("</td>");

              client.println("</tr>");

              client.println("<tr>");
                client.println("<td>");
                 client.println("Altitud");
                client.println("</td>");
                
                client.println("<td id=\"altitud\">");                
                 client.println(mySensor.readFloatAltitudeMeters(), 1);                                 
                 client.println(" m");
                client.println("</td>");
              client.println("</tr>");

              client.println("<tr>");
                client.println("<td>");
                 client.println("Humedad");
                client.println("</td>");
                
                client.println("<td id=\"humedad\">");                
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