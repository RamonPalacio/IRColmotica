#include <ESP8266WiFi.h>        // Include the Wi-Fi library

#include <ESP8266WebServer.h>

ESP8266WebServer server(80);
void NotFound();
void handleRoot();              // function prototypes for HTTP handlers
void ServerConfig();

// Configuración de la WiFi generada
const char *ssid = "俄非俄非黑阿卡";
const char *password = "123456789.";


void setupAp() {
  Serial.begin(2000000);
  delay(10);
  
  WiFi.mode(WIFI_AP);
  while(!WiFi.softAP(ssid, password))
  {
   Serial.println(".");
    delay(100);
  }
  
  Serial.print("Iniciado AP ");
  Serial.println(ssid);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());

  iniciarServer();
  
}

void loop()
{ 
server.handleClient();  
}


void iniciarServer()
{
    server.onNotFound(NotFound);
    server.on("/red", ServerConfig);
    server.begin();
}
     
void NotFound()
{
  server.send(404, "text/html", "Recurso no Disponible");
}

void ServerConfig() 
{
  const String ID = server.arg("id");
  const String pass = server.arg("pass");
  server.send(200, "text/plain", "ID:" + ID + "\t  Pass:" + pass); 

}