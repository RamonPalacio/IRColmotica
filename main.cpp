//******** Declaracion Variables Globales ************
using namespace std;
//****************************************************

//******** Declaracion de Librerias Externas ************
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h> 
#include <IRutils.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <elapsedMillis.h>


//*******************************************************

//******** Declaracion de Librerias Propias  ************
#include "main.hpp"
#include "webrequests.hpp"
#include "wificonfig.hpp"
//*******************************************************

      void setup()
      {
            Serial.begin(2000000); // Inicializa Terminal Serial en los Baudios indicados
            Serial<<F("SetupIN\n");
                        pinMode(wemos.D4,OUTPUT);

                        // if(iniciarWiFi()) {TimerID = timer.setInterval(5000, ConectServerSokect);}
                         
                        iniciarWiFi();
                        iniciarWebServer();
                        webSocket.begin();
                        webSocket.onEvent(webSocketEvent);

                        Serial.println(listarArchivos());

                        #if DECODE_HASH
                              irrecv.setUnknownThreshold(kMinUnknownSize); // Ignorar valores menores a 12 milisegundos.
                        #endif  // DECODE_HASH
                        irrecv.enableIRIn();  // Iniciar IR Emisor                
                        irsend.begin();       // Iniciar IR Transmisor    
                        // // StaticJsonDocument<200> filter;
                        // // filter["CONTROLES"] = true;                     // Filtro de control
                        // // doc = JsonReadFile("/control.json",filter);     //Cargar controles
                        // // serializeJsonPretty(doc, Serial); //Imprime JsonControl Bonito
                        //======================== CONFIG IR ========================
                        // WiFi.printDiag(Serial);
            Serial<<F("SetupOut\n");
            //Enviar_IR_String("8960,4480,560,1680,560,560,560,560,560,1680,560,560,560,560,560,1680,560,560,560,1680,560,1680,560,1680,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,1680,560,1680,560,560,560,560,560,560,560,560,560,560,560,1680,560,560,560,1680,560,560,560,560,560,1680,560,560,560,20720,560,1680,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,1680,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,1680,560,1680,664");
      }

      void loop()
      {
            // int sensorValue = analogRead(wemos.A0);
            // Serial.println(sensorValue);
            webSocket.loop();
            SocketGetter();
            server.handleClient(); // Listen for HTTP requests from cliemts
            IRGetter();

            switch (countMillis)
            {
            case 1000:  
                        countMillis+=1;
                        digitalWrite(wemos.D4,0);
                  break;
                  
            case 2000:  
                        countMillis+=1;
                        digitalWrite(wemos.D4,1);
                  break;

            default:                      
                        if(countMillis > 2000)
                        {
                              if(WiFi.status()==WL_CONNECTED && !STAConect) 
                              {
                                    STAConect=true;
                                    Serial<<F("\n***Conectado***");
                                    IPAddress Ip(192,168,1,200); 
                                    IPAddress Gateway(192,168,1,1); 
                                    IPAddress Subnet(255,255,255,0); 
                                    WiFi.config(Ip, Gateway, Subnet); 
                                    Serial.println(WiFi.localIP()); 
                              }
                              // uint8_t MemoriaDisponible = ESP.getFreeHeap()/524.26;
                              // Serial.printf("  SRAM Libre: %d Porciento\n", MemoriaDisponible);    
                              countMillis = 0;
                        }
                  break;
            }
      }

