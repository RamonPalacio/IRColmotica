
void iniciarWebServer();

//**** Paginas Web Navegadas *****
void NotFound();
void SetSTA_AP();
void GetSTA_AP();
void GetRedes();
void SocketGetter();
void ConectServerSokect();
void SendData();
void webSocketEvent(uint8_t , WStype_t , uint8_t * , size_t );
//*********************************
String pagina = "<html lang=\"es\"><head> <meta name='viewport' content='width=device-width, initial-scale=1' charset=\"UTF-8\"><title>Configuracion</title>"
"<head>"
"<script>"
"var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);"
"connection.onopen = function ()       { connection.send(new Date()); };"
"connection.onerror = function (error) { console.log('WebSocket Error ', error);};"
"connection.onmessage = function (e)   { document.getElementById('g').value = e.data;};"
"function sender(text) {"
" var g = document.getElementById('g').value;"
" connection.send(text==0?g:text);"
"}"
"</script>"
"</head>"
"<body>"
"<div style='width: 150px; background-color: gray; zoom: 250%;'>Control:<br/><br/>"
"<input type='text' style='font-size:6px; width: 150px;' name='g' id='g' value=''>"

"<br><button name='44' onclick='sender(0);'>                    Personalizado </button>"
"<br><br><button name=        '8,8,1,*|.@*|.@*|?**3,1'       onclick='sender(this.name);'>           Vol+          </button>"
"<button name=                '8,8,1,*|.@*|?*@|@*|+|@3,1'     onclick='sender(this.name);'>          <<            </button>"
"<button name=                '8,8,1,*|.@*|?+*@**|+3,1'       onclick='sender(this.name);'>          ↑             </button>"
"<button name=                '8,8,1,*|.@*|.@|+**+*.3,1'       onclick='sender(this.name);'>        Menu           </button>"
"<br><br><button name=        '8,8,1,*|.@*|.@*@|.+|@*|3,1'     onclick='sender(this.name);'>         Vol-           </button>&nbsp;"
"<button name=                '8,8,1,*|.@*|.@|@|+*+|@*+3,1'     onclick='sender(this.name);'>         ←             </button>"
"<button name=                '8,8,1,*|.@*|?|@*@*|@|+3,1'     onclick='sender(this.name);'>           ■             </button>"
"<button name=                '8,8,1,*|.@*|.+|+@*@|@*|+3,1'     onclick='sender(this.name);'>         →             </button>"
"<br><br><button name=        '8,8,1,*|.@*|.+|.+|@**|3,1      onclick='sender(this.name);'>         Power           </button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
"<button name=                '8,8,1,*|.@*|.@|.*+**+3,1'       onclick='sender(this.name);'>          ↓              </button></div>"

"</body>"
"</html>";

void iniciarWebServer() //HTTP
{
      server.onNotFound(        NotFound);
      server.on("/SetSTA_AP",   SetSTA_AP);
      server.on("/read",        GetSTA_AP);
      server.on("/redes",       GetRedes);
      server.on("/send",        SendData);
      server.on("/", []() {server.send(200, "text/html", pagina); });

      server.begin();
}
     
void NotFound() //HTTP
{
      server.send(404, "text/html", "Recurso no Disponible");
}

void SetSTA_AP() //HTTP
{
      //http://192.168.4.1/SetSTA_AP?id=TIGO-6C8A&pass=4D9697506216&apid=俄非俄非黑阿&appass=123456789.&ipserver=192.168.1.3&portserver=888
      const string id          = server.arg("id").c_str();
      const string pass        = server.arg("pass").c_str();

      const string ap_id       = server.arg("apid").c_str();
      const string ap_pass     = server.arg("appass").c_str();

      const string ipserver    = server.arg("ipserver").c_str();
      const string portserver  = server.arg("portserver").c_str();
      const string namedns  = server.arg("namedns").c_str();

      std::string buf{"staID: "};  buf.append(id);  buf.append("\t staPass");  buf.append(pass);    buf.append("\t ApID");    buf.append(ap_id);    buf.append("\t ApPass");    buf.append(ap_pass);buf.append("\t namedns");buf.append(namedns);
      server.send(200, "text/plain", buf.c_str()); 

      StaticJsonDocument<50> filter;
      StaticJsonDocument<400> configSTA_AP;
      filter["WiFi"] = true;
      configSTA_AP = JsonReadFile("/config.json",filter);    

      if(!strcmp(id.c_str(),"")==0)
      {
            configSTA_AP["WiFi"]["SSID"] = id;
            configSTA_AP["WiFi"]["PASS"] = pass;
      }
      
      if(!strcmp(ap_id.c_str(),"")==0)
      {
            configSTA_AP["WiFi"]["APSSID"] = ap_id;
            configSTA_AP["WiFi"]["APPASS"] = ap_pass;
      }

      if(!strcmp(ipserver.c_str(),"")==0)
      {
            configSTA_AP["WiFi"]["SERVERTCP"] = ipserver;
            configSTA_AP["WiFi"]["SERVERPORT"] = portserver;
      }

      configSTA_AP["WiFi"]["HOSTNAME"] = (!strcmp(namedns.c_str(),"null")==0)?namedns:"";

      File file {SPIFFS.open("/config.json", "w")};
            if(!file)return; // Error Abriendo Archivo
            serializeJson(configSTA_AP, file);
            serializeJsonPretty(configSTA_AP, Serial);
      file.close();
}

void GetSTA_AP() //HTTP  Mostrar por Web   http://192.168.4.1/read
{
      // timer.disable(TimerID);
      StaticJsonDocument<50> filter;
      StaticJsonDocument<400> read;
      filter["WiFi"] = true;                    
      read = JsonReadFile("/config.json",filter);    
      serializeJsonPretty(read, Serial);
      std::string buf{};
      serializeJsonPretty(read, buf);
      server.send(200, "application/json", buf.data());
}

void GetRedes() //HTTP
{  
      // timer.disable(TimerID);
      String mensaje = "";
      int n = WiFi.scanNetworks(); //devuelve el número de redes encontradas
      Serial.println("escaneo terminado");
      if (n == 0) { //si no encuentra ninguna red
            Serial.println("no se encontraron redes");
            mensaje = "no se encontraron redes";
      } else
      {
            Serial.print(n);
            Serial.println(" redes encontradas");
            mensaje = "";
            for (int i = 0; i < n; ++i)
            {
                  mensaje = (mensaje) + "<button onclick=\"getElementById('wifi').value='" + WiFi.SSID(i) + "'\">" + WiFi.SSID(i) + "</button><br><br>";
                  // mensaje = (mensaje) + "<p>" + String(i + 1) + ": " + WiFi.SSID(i) + " (" + (String)WiFi.RSSI(i) + ") Ch: " + (String)WiFi.channel(i) + " Enc: " + (String)WiFi.encryptionType(i) + " </p>\r\n";
                  delay(1);
            }

            StaticJsonDocument<50> filter;
            StaticJsonDocument<350> configJson;
            filter["WiFi"] = true;
            configJson = JsonReadFile("/config.json",filter);

            String s = "";
            if (WiFi.status() == WL_CONNECTED) 
            {
                  s+="<html lang=\"es\"><head><meta charset=\"UTF-8\"><title>Configuracion</title>";
                  s+="</head><body><form action=\"/SetSTA_AP\" method=\"get\">";
                  s+="<br> Nombre de Red: <input type=\"text\" name=\"id\" id=\"id\" value=\"" + (String)configJson["WiFi"]["SSID"] + "\">";
                  s+=" Contraseña: <input type=\"text\" name=\"pass\" value=\"" + (String)configJson["WiFi"]["PASS"] + "\">";
                  s+="<br> Nombre de Red Local: <input type=\"text\" name=\"apid\" value=\"" + (String)configJson["WiFi"]["APSSID"] + "\">";
                  s+=" ContraseñaAP: <input type=\"text\" name=\"appass\" value=\"" + (String)configJson["WiFi"]["APPASS"] + "\">";
                  s+="<br> Server TCP: <input type=\"text\" name=\"ipserver\" value=\"" + (String)configJson["WiFi"]["SERVERTCP"] + "\">";
                  s+=" Port TCP: <input type=\"text\" name=\"portserver\" value=\"" + (String)configJson["WiFi"]["SERVERPORT"] + "\">";
                  s+="<br><br>Ejemplo: http://esp8266.local/<br> host name: http://<input type=\"text\" name=\"namedns\" value=\"" + (String)configJson["WiFi"]["HOSTNAME"] + "\">.local/";
                  s+="<br> <input type=\"submit\" value=\"Guardar\">";
                  s+=(WiFi.status() == WL_CONNECTED)?"<br><br>Actualmente Conectado a la Red:<h1>" + (String)configJson["WiFi"]["SSID"]:"<br><br>Estado:<h1>desconectado!";
                  s+="</h1> </form></body></html>";
                  

                  server.send(200, "text/html", mensaje + s);

            }
      }     
}

void SendData() //HTTP
{
      string ReadStringTemp = server.arg("hex").c_str();
      server.send(200, "text/html",ReadStringTemp.data());   
      if ( !(ReadStringTemp.length() <= 1))
      {
            uint32_t ir_Dec = stoul(ReadStringTemp, nullptr, 16);
            if(ir_Dec != 0)
            {                                        
                  irsend.sendNEC(ir_Dec,ReadStringTemp.length()*4); // SendIR
                  // irsend.sendGree("28584B7540");
                  
                  Serial<<ReadStringTemp.data();
            }
      } 
}


//****************************************SOCKETS*********************************************************


void SocketGetter() // TCP
{
      if (client.available()) //Recibiendo Datos por TCP
      {
            string ReadStringTemp {client.readStringUntil('|').c_str()};
            if ( !(ReadStringTemp.length() <= 1))
            {
                  uint32_t ir_Dec = stoul(ReadStringTemp, nullptr, 16);
                  if(ir_Dec != 0)
                  {                                        
                        irsend.sendNEC(ir_Dec,ReadStringTemp.length()*4); // SendIR
                        Serial<<ReadStringTemp.data();
                        server.send(200, "text/html", ReadStringTemp.data());
                  }
            }                   
      }
}

void ConectServerSokect() //TCP
{     
      if (!client.connected())
      {
            if (client.connect("192.168.1.3", 8888)) 
            {
                  Serial<<F("ConectadoSocket");
                  string respuestaConect {"ESP8266->" + std::to_string(ESP.getChipId())};
                  client<<respuestaConect.data();
            }
      }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) { 

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            webSocket.sendTXT(num, "Connectado al Servidor");
        }
            break;
        case WStype_TEXT:  // Recibe Texto del Socket
            Serial.printf("[%u] WS: %s\n", num, payload);
            
            string ReadStringTemp = (const char*)payload;
            ReadStringTemp = ReplaceAll(ReadStringTemp, "?", "..") ;
            ReadStringTemp = ReplaceAll(ReadStringTemp, ".", "++") ;
            ReadStringTemp = ReplaceAll(ReadStringTemp, "+", "@@") ;
            ReadStringTemp = ReplaceAll(ReadStringTemp, "*", "||") ;
            ReadStringTemp = ReplaceAll(ReadStringTemp, "@", "1,1,") ;
            ReadStringTemp = ReplaceAll(ReadStringTemp, "|", "3,1,") ;
            Serial << ReadStringTemp.data();
            Enviar_IR_Dec(ReadStringTemp);
            break;
    }
}