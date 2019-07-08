
// connect tiker with WIFI led
void tick()
{
  //toggle state
  int state = digitalRead(WIFI_LED);  // get the current state of GPIO pin
  digitalWrite(WIFI_LED, !state);     // set pin to the opposite state
}

// the following function is executed when wifi manager enters config mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// the following function is called at setup
// it calles the wifimanager and implements the autoconnect
void AutoConnSetup() {
  //set led pin as output
  pinMode(WIFI_LED, OUTPUT);

  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //timeout - this will quit WiFiManager if it's not configured in 3 minutes, causing a restart
  wifiManager.setConfigPortalTimeout(180);

  //custom params
  EEPROM.begin(512);
  EEPROM.get(0, settings);
  EEPROM.end();

  if (settings.salt != EEPROM_SALT) {
    Serial.println("Invalid settings in EEPROM, trying with defaults");
    WMSettings defaults;
    settings = defaults;
  }

  Serial.println(settings.blynkToken);
  Serial.println(settings.blynkServer);
  Serial.println(settings.blynkPort);

  WiFiManagerParameter custom_blynk_text("<br/>Blynk config. <br/> No token to disable.<br/>");
  wifiManager.addParameter(&custom_blynk_text);

  WiFiManagerParameter custom_blynk_token("blynk-token", "blynk token", settings.blynkToken, 33);
  wifiManager.addParameter(&custom_blynk_token);

  WiFiManagerParameter custom_blynk_server("blynk-server", "blynk server", settings.blynkServer, 33);
  wifiManager.addParameter(&custom_blynk_server);

  WiFiManagerParameter custom_blynk_port("blynk-port", "port", settings.blynkPort, 6);
  wifiManager.addParameter(&custom_blynk_port);


  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);
 
  if (!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  } 


  //Serial.println(custom_blynk_token.getValue());
  //save the custom parameters to FS
  if (shouldSaveConfig) {
     Serial.println("Saving config");
     strcpy(settings.blynkToken, custom_blynk_token.getValue());
     strcpy(settings.blynkServer, custom_blynk_server.getValue());
     strcpy(settings.blynkPort, custom_blynk_port.getValue());

     Serial.println(settings.blynkToken);
     Serial.println(settings.blynkServer);
     Serial.println(settings.blynkPort);

     EEPROM.begin(512);
     EEPROM.put(0, settings);
     EEPROM.end();
  }
 

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  ticker.detach();  
  digitalWrite(WIFI_LED,WIFI_LED_DEFAULT);
}

// AutoConnRst is called by the application (when button pressed for 10 sec)
// the old settings are removed and the device reset
void AutoConnRst() {
   WMSettings defaults;
   settings = defaults;
   EEPROM.begin(512);
   EEPROM.put(0, settings);
   EEPROM.end();
   WiFi.disconnect();
   delay(1000);
   ESP.reset();
   delay(1000); 
}
