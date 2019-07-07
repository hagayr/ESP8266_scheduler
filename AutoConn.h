
// Global parameters
#define EEPROM_SALT 12667
typedef struct {
  char  blynkToken[33]    = "";
  char  blynkServer[33]   = "blynk-cloud.com";
  char  blynkPort[6]      = "8442";
  int   salt              = EEPROM_SALT;
} WMSettings;

WMSettings settings;


//for LED status
#include <Ticker.h>
Ticker ticker;


void tick();


void configModeCallback(WiFiManager *myWiFiManager); 


//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback();

void AutoConnSetup(); 

void AutoConnRst();
