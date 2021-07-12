

//******** Declaracion de Funciones ************
            void iniciarWiFi();
//**********************************************

// Configuración de la WiFi generada
      void iniciarWiFi()
      {
            WiFi.mode(WIFI_AP_STA);
            WiFi.setAutoReconnect(true);

            if(!SPIFFS.begin())return; //Inicia la SDFlash
            
            StaticJsonDocument<100> filter;
            StaticJsonDocument<400> configJson;
            filter["WiFi"] = true;
            configJson = JsonReadFile("/config.json",filter);

            while(!WiFi.softAP((const char*)configJson["WiFi"]["APSSID"],(const char*)configJson["WiFi"]["APPASS"]))
            {
                  Serial.println("Ap.");
                  delay(50);
            }

            WiFi.begin((const char*)configJson["WiFi"]["SSID"],(const char*)configJson["WiFi"]["PASS"]);
      }


