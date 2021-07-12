

#ifndef WEMOS
      #define WEMOS
#endif

      WiFiClient client;
      elapsedMillis countMillis;
      ESP8266WebServer server(80);
      WebSocketsServer webSocket(81);

//******** Declaracion de Funciones ************
      DynamicJsonDocument JsonReadFile();
      void IRGetter();
      void Enviar_IR_String();
      void Enviar_IR_Dec();
      String listarArchivos();
      
string ReplaceAll(std::string , const std::string& , const std::string& );
//**********************************************
            bool STAConect{false};


            #if defined(WEMOS)
              struct Board{const uint8_t D0=16,D1=5,D2=4,D3=0,D4=2,D5=14,D6=12,D7=13,D8=15,TX=1,RX=3,A0=17;};Board wemos;
            #endif

            // extern "C" {
            //   #include<user_interface.h>
            // }

            //================Variables InfraRojo================
            #if DECODE_AC
              const uint8_t kTimeout {50};
            #else 
              const uint8_t kTimeout = 15;
            #endif

            const uint16_t kMinUnknownSize {12};

            IRsend irsend{wemos.D2};  // ESP8266 GPIO pin usado por el IR Trasmisor
            IRrecv irrecv{wemos.D3, 1024, kTimeout, true}; // (ESP8266 GPIO pin usado por el IR Receptor , kCaptureBufferSize , kTimeout ,  true)
            
            // DynamicJsonDocument doc(7000);  

  DynamicJsonDocument JsonReadFile(char* path,StaticJsonDocument<200> filter)
  {
      DynamicJsonDocument Respuesta(300);
      File file {SPIFFS.open(path, "r")};
      if(!file)return Respuesta; // Error Abriendo Archivo
      DeserializationError error = deserializeJson(Respuesta, file, DeserializationOption::Filter(filter));          
      file.close();
      return Respuesta;
  }

  void Enviar_IR_String(std::string Raw)
  {
        uint16_t Total {std::count(Raw.begin(), Raw.end(), ',')+1};

        if(Total==1)return; //Comando no encontrado

        uint16_t Signal_X[Total+1]; //Maximo Raw 300  
        for(uint16_t i = 0; i <= Total; i++)
        {
            Signal_X[i] = std::stoi(Raw.substr(0,Raw.find_first_of(",")));
            Raw = Raw.substr(Raw.find_first_of(",")+1);
        }
        irsend.sendRaw(Signal_X, Total, 38);  // Send a raw data capture at 38kHz.    
        Serial<<F("Enviado");
  }

   void Enviar_IR_Dec(std::string& Raw)
  {
        Serial<<F("\nEnvi....:");
        uint16_t Total {std::count(Raw.begin(), Raw.end(), ',')+1};

        if(Total==1)return; //Comando no encontrado

        uint16_t Signal_X[Total+1]; //Maximo Raw 300  
        for(uint16_t i = 0; i <= Total; i++)
        {
            Signal_X[i] =stoi(Raw.substr(0,Raw.find_first_of(",")))*560;
            Raw = Raw.substr(Raw.find_first_of(",")+1);
            Serial.printf("%d",Signal_X[i])  ;
            Serial << ",";
        }
        irsend.sendRaw(Signal_X, Total, 38);  // Send a raw data capture at 38kHz.    
        Serial<<F("\n...ando");
  }

  String listarArchivos()
  {
        Dir dir {SPIFFS.openDir("/")};
        String output = "";

        while (dir.next())
        {
              File entry = dir.openFile("r");
              String FileName = String(entry.name());
              output += FileName + ",";
              entry.close();
        }

        return output;
  }

      void IRGetter ()
      {
            decode_results results; 
            if (irrecv.decode(&results))
            {
                  // string IRGet{};
                  Serial<<(results.overflow)?F("el IR code es muy grande para el buffer (Raw >= 1024)\n"):0;
                  // Serial.println("");
                        // Serial.println(getCorrectedRawLength(&results));
                        // uint16_t* resultado = resultToRawArray(&results);
                  // String Sourcecode = resultToSourceCode(&results);
                        // String normal = resultToHumanReadableBasic(&results);
                        string resultado2 = to_string(results.value);
                        Serial<<"\n";Serial << resultado2.data();Serial<<"\n";
                  // Serial<<"\nsource:";Serial.println(Sourcecode);Serial<<"\n";
                        // Serial<<"\nNormal:";Serial.println(normal);
                        // Serial<<"\nresultado2:";Serial.println(resultado2);
                        // Serial<<"\nBits:";Serial.println((String)(results.bits/8));
                        // Serial<<"\nLen:"; Serial.println(resultado2.length()/2);


                  uint16_t Signal_X[results.rawlen]; //Maximo Raw 300  
                        string res;
                        for (uint16_t i = 0; i < results.rawlen-1; i++) 
                        {
                              uint32_t usecs = (results.rawbuf[i+1]+100)/280;
                              usecs = trunc((usecs))==0?1:trunc((usecs));
                              res += to_string(usecs);
                              if(results.rawlen-2 > i)
                                    res += ",";
                              Signal_X[i]=results.rawbuf[i+1];
            
                        }
                        Serial << res.data(); Serial << "\n";

                              res = ReplaceAll(res, "3,1,", "|") ;
                              res = ReplaceAll(res, "1,1,", "@") ;
                              res = ReplaceAll(res, "||", "*") ;
                              res = ReplaceAll(res, "@@", "+") ;
                              res = ReplaceAll(res, "++", ".") ;
                              res = ReplaceAll(res, "..", "?") ;
                       
                        Serial << res.data(); Serial << "\n";

                              if(STAConect){
                                    webSocket.sendTXT(0,res.data());
                                    webSocket.sendTXT(1,res.data());
                                    webSocket.sendTXT(2,res.data());
                                    webSocket.sendTXT(3,res.data());
                                    webSocket.sendTXT(4,res.data());
                              }
                              res = ReplaceAll(res, "?", "..") ;
                              res = ReplaceAll(res, ".", "++") ;
                              res = ReplaceAll(res, "+", "@@") ;
                              res = ReplaceAll(res, "*", "||") ;
                              res = ReplaceAll(res, "@", "1,1,") ;
                              res = ReplaceAll(res, "|", "3,1,") ;


                        // delay(2000);                        
                        // Enviar_IR_Dec(res);


                        // Serial<<"\nlen:";Serial.println(results.rawlen-1);Serial<<"\n";
                        // Serial<<"\nRespuesta:";Serial<<res.data();

                       

                        // irsend.sendRaw(Signal_X,results.rawlen-1,38);
                        //Enviar_IR_String(res.data());
                  //  (ir_Dec,ReadStringTemp.length()*4); // SendIR
                  // Serial.println(" (BIN)");
                  


                  // IRGet = resultToSourceCode(&results).c_str();

                  // if ((results.bits/8)==(resultado2.length()/2)){
                  //       Serial.println((String)(results.bits/8));
                  //       Serial.println(resultado2.length()/2);
                  //       Serial.println(resultado2);
                  //       webSocket.sendTXT(0,resultado2);
                  //       webSocket.sendTXT(1,resultado2);
                  //       webSocket.sendTXT(2,resultado2);
                  //       webSocket.sendTXT(3,resultado2);
                  //       webSocket.sendTXT(4,resultado2);
                  // }
                  irrecv.resume();
                  // Serial.printf((string*)resultado);
                                
                  // IRGet.erase(std::remove_if(IRGet.begin(), IRGet.end(), ::ispunct), IRGet.end());

                  // char* pch = strrchr(IRGet.data(),' ')+1;
                   
                  // if(strchr(pch,'x')) pch+=2;
                  // String datel = pch;

                   
                  // if(datel.length()>=1){
                  //       webSocket.sendTXT(0,datel);
                  //       webSocket.sendTXT(1,datel);
                  //       webSocket.sendTXT(2,datel);
                  //       webSocket.sendTXT(3,datel);
                  //       webSocket.sendTXT(4,datel);
                  // }
                  
                  // webSocket.sendTXT(1,pch);


                  // client << pch;


                  // int ini {IRGet.find_last_of(" ")};
                  // int Fin {IRGet.size()};
                  
                  // IRGet = IRGet.substr(ini, Fin - ini);
                  // IRGet.erase(std::remove_if(IRGet.begin(), IRGet.end(), ::isspace), IRGet.end());
		      // if(IRGet.find("0x")==0)    IRGet.replace(IRGet.find("0x"), 2, "");
                  
                  // uint32_t ir_Dec = std::stoul(IRGet, nullptr, 16);

                  
                  

                  
                  // Serial.printf("Valor Hex:");Serial.println(IRGet.data());
                  // Serial.printf("Valor bits:");Serial.println(IRGet.length()*4);
                  // Serial.printf("Valor Raw:");Serial.println(IRGet.c_str());  

                  // if (client.connected())  client.printf(IRGet.data());

               

                  // int ini {IRGet.find("uint64_t data =")+18};
                  // int Fin {IRGet.find_last_of(";")};
                  // IR_HEX = IRGet.substr(ini, Fin - ini);

                  // ini = IRGet.find_last_of(" ");
                  // Fin = IRGet.find_last_of(";");
                  // // string veamos = veamos.substr(ini, Fin - ini);
                  // Serial.printf("Valor ini:");Serial.println(ini);
                  // Serial.printf("Valor Fin:");Serial.println(Fin);

                  
                  // ini = IRGet.find("{")+1;
                  // Fin  = IRGet.find("}");
                  // IRGet = IRGet.substr(ini, Fin - ini);
                  // IRGet.erase(std::remove_if(IRGet.begin(), IRGet.end(), ::isspace), IRGet.end());


                  // uint16_t Total {count(IRGet.begin(), IRGet.end(), ',')+1};
                  // if(Total!=1)
                  // {
                  //       uint16_t Signal_X[Total+1]; //Maximo Raw 300  
                  //       for(uint16_t i = 0; i <= Total; i++)
                  //       {
                  //             Signal_X[i] = std::stoi(IRGet.substr(0,IRGet.find_first_of(",")));
                  //       }
                  // }
                  
            }
      }

string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}