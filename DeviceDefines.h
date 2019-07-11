
//#define SONOFF_4CH_PRO
#define ESP8285_NODEMCU
//#define SONOFF_TH16
//#define APTINEX_RP4CE8







#ifdef SONOFF_4CH_PRO
 #define WIFI_LED 16
 #define WIFI_LED_DEFAULT LOW

 #define RESET_PIN 0
 #define RESET_PIN_POLARITY HIGH
 // define parameters for number of schedulers and number of timers
 #define SCHEDULER_CNT 4
 #define TIME_CNT 4

 // Define GPIO inputs used for immidiate activation of schedulers
 byte in_gpio_pins[] = {0,9,10,14}; // number of gpios used as button to activate scheduler from device 
 byte in_gpio_polarity[] = {HIGH,HIGH,HIGH,HIGH}; // polarity of each gpio pin (HIGH=Reverse) 
 bool gpio_pin_set[] = {false,false,false,false}; // counts the time the gpio is set 

 byte task_gpio_pins[] = {12,5,4,15}; // number of gpio to be used as task control
 // The default value of the gpio for task off is given by task_gpio_default.
 //  (used for gpio negative polarity)
 bool task_gpio_default[] = {LOW,LOW,LOW,LOW}; 
#endif


#ifdef SONOFF_TH16
 #define WIFI_LED 13
 #define WIFI_LED_DEFAULT HIGH

 #define RESET_PIN 0
 #define RESET_PIN_POLARITY HIGH
 // define parameters for number of schedulers and number of timers
 #define SCHEDULER_CNT 1
 #define TIME_CNT 4

 // Define GPIO inputs used for immidiate activation of schedulers
 byte in_gpio_pins[] = {0}; // number of gpios used as button to activate scheduler from device 
 byte in_gpio_polarity[] = {HIGH}; // polarity of each gpio pin (HIGH=Reverse) 
 bool gpio_pin_set[] = {false}; // counts the time the gpio is set 

 byte task_gpio_pins[] = {12}; // number of gpio to be used as task control
 // The default value of the gpio for task off is given by task_gpio_default.
 //  (used for gpio negative polarity)
 bool task_gpio_default[] = {LOW}; 
#endif

#ifdef ESP8285_NODEMCU

 #define WIFI_LED 16
 #define WIFI_LED_DEFAULT HIGH

 #define RESET_PIN 14
 #define RESET_PIN_POLARITY LOW
 // define parameters for number of schedulers and number of timers
 #define SCHEDULER_CNT 4
 #define TIME_CNT 4 

 // Define GPIO inputs used for immidiate activation of schedulers
 byte in_gpio_pins[] = {14,24,24,24}; // number of gpios used as button to activate scheduler from device 
 byte in_gpio_polarity[] = {LOW,LOW,LOW,LOW}; // polarity of each gpio pin (HIGH=Reverse) 
 bool gpio_pin_set[] = {false,false,false,false}; // counts the time the gpio is set 

 byte task_gpio_pins[] = {2,5,13,15}; // number of gpio to be used as task control
 // The default value of the gpio for task off is given by task_gpio_default.
 //  (used for gpio negative polarity)
 bool task_gpio_default[] = {LOW,LOW,LOW,LOW}; 
#endif

#ifdef APTINEX_RP4CE8

 #define WIFI_LED 16
 #define WIFI_LED_DEFAULT LOW

 #define RESET_PIN 0
 #define RESET_PIN_POLARITY HIGH
 // define parameters for number of schedulers and number of timers
 #define SCHEDULER_CNT 4
 #define TIME_CNT 4 

 // Define GPIO inputs used for immidiate activation of schedulers
 byte in_gpio_pins[] = {24,24,24,24}; // number of gpios used as button to activate scheduler from device 
 byte in_gpio_polarity[] = {LOW,LOW,LOW,LOW}; // polarity of each gpio pin (HIGH=Reverse) 
 bool gpio_pin_set[] = {false,false,false,false}; // counts the time the gpio is set 

 byte task_gpio_pins[] = {12,13,14,15}; // number of gpio to be used as task control
 // The default value of the gpio for task off is given by task_gpio_default.
 //  (used for gpio negative polarity)
 bool task_gpio_default[] = {LOW,LOW,LOW,LOW}; 
#endif


//////////////////////////////////////////////////////////////////////////////////
// task activation fuction
// this is an abstraction layer for activating task
// curently only simple activation of gpio but user can change this
//////////////////////////////////////////////////////////////////////////////////
// This system uses gpio on of as tasks.
// The number of used gpio per task is given by task_gpio_pins array
//byte task_gpio_pins[] = {12 , 5 , 4 , 15}; // number of gpio to be used as task control
//byte task_gpio_pins[] = {12}; // number of gpio to be used as task control


void SchedulerTask_OnOff(bool task_state, char scheduler_num){
  bool gpio_val = task_state^task_gpio_default[scheduler_num];
  digitalWrite(task_gpio_pins[scheduler_num],gpio_val);
}

void SchedulerTaskInit(char scheduler_num){
    pinMode(task_gpio_pins[scheduler_num], OUTPUT);  // set output pins
}


// define fimware update server
// firmware update done through "http://esp8266_scheduler.local/firmware"
const char* host = "esp8266_scheduler"; 
const char* update_path = "/firmware";
