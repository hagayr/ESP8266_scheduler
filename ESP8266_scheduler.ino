/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/
/* INTRODUCTION 
 *    This code implements a parametric shceduler to activate a task 
 *      at specific times and specific days . 
 *    The system operates at resolution of minuets. 
 *    Each scheduler can be assigned one task (i.e. turn on/off a gpio ...).
 *    The user can paramtrize the number of schedulers (up to 16). 
 *    The user can parametriz the number of timers assigned to each scheduler. 
 *    Each timer can be configured (from the app) to activate/deactivate 
 *      the task at specific start/end time and days of week 
 *    Each task can also be activated immidiatly and turned off after a 
 *      configurable defalut time per scheduler (from the app).  
 *      If end time is not given the task will be deactivated 
 *      after the scheduler default time. 
 *    Each scheduler can be configured (from app) with a max duration.  
 *      If when timer activates a task the max duration is smaller than
 *      the configured task duration then the task is activated for max duration.
 *      If max duration is smaller than 2min it is ignored.
 *    
 *    The system can be disabled for a configurable (from app) number of days. 
 *      When in sleep mode all active tasks and all schedulers are turned off. 
 *      If number of days is 0 all current active tasks will be turned off
 *      If number of days is 1 all tasks and all schedulers will be 
 *      turned off until midnight of today.
 *      If number of days is 2 all tasks and all schedulers will be 
 *      turned off until midnight of next day. ...
 *    The system can also be put in sleep (from app) .  
 *      When in sleep mode all active tasks are turned off. Schedulers continue working
 *      but will not activate tasks. Once out of sleep mode tasks are turned on if they 
 *      ware expected to be active without the sleep mode.
 *      if number of days is 0 the sleep button has no effect.
 *      if number of days is 1 the sleep mode continue until midnight.
 *      if number of days is 2 the sleep mode continue until midnight next day....
 *      
 */

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

/////////////////////////////////////////////////////////////
// system configuration section
// The following section contains code that should be changed 
//  by user to match the exact system needs.
////////////////////////////////////////////////////////////
// place all passwords and tokens in the following file
#include "passwd.h"

#define WIFI_LED 13
//#define WIFI_LED 5

// define parameters for number of schedulers and number of timers
#define SCHEDULER_CNT 4
#define TIME_CNT 4

// Define GPIO inputs used for immidiate activation of schedulers
//byte in_gpio_pins[] = {0}; // number of used as button to activate task from device 
byte in_gpio_pins[] = {14,14,14,14}; // number of gpios used as button to activate scheduler from device 
//byte in_gpio_polarity[] = {HIGH}; // polarity of each gpio pin (HIGH=Reverse) 
byte in_gpio_polarity[] = {LOW,LOW,LOW,LOW}; // polarity of each gpio pin (HIGH=Reverse) 
byte last_in_gpio_pins[] = {LOW,LOW,LOW,LOW}; // holds last status of gpio pin (used for creating toggle) 



// define fimware update server
// firmware update done through "http://esp8266_boiler.local/firmware"
const char* host = "esp8266_boiler"; 
const char* update_path = "/firmware";

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;


//////////////////////////////////////////////////////////////////////////////////
// task activation fuction
// this is an abstraction layer for activating task
// curently only simple activation of gpio but user can change this
//////////////////////////////////////////////////////////////////////////////////
// This system uses gpio on of as tasks.
// The number of used gpio per task is given by task_gpio_pins array
//byte task_gpio_pins[] = {12 , 5 , 4 , 15}; // number of gpio to be used as task control
//byte task_gpio_pins[] = {12}; // number of gpio to be used as task control
byte task_gpio_pins[] = {2,5,13,15}; // number of gpio to be used as task control
// The default value of the gpio for task off is given by task_gpio_default.
//  (used for gpio negative polarity)
bool task_gpio_default[] = {LOW,LOW,LOW,LOW}; 

void SchedulerTask_OnOff(bool task_state, char scheduler_num){
  bool gpio_val = task_state^task_gpio_default[scheduler_num];
  digitalWrite(task_gpio_pins[scheduler_num],gpio_val);
}

void SchedulerTaskInit(char scheduler_num){
    pinMode(task_gpio_pins[scheduler_num], OUTPUT);  // set output pins
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// end system configuration section                                               //
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// The following defines will automaticaly assign virtual pins                  //
//  The first SCHEDULER_CNT*TIME_CNT virtual pins are assigned for time inputs  //
//   wher virtual pins 0..TIME_CNT-1 are for scheduler 0                        //
//                pins TIME_CNT..2*TIME_CNT-1 for scheduler 1 ...               //
//  The following SCHEDULER_CNT pins are used for default duration sliders      //
//  The following SCHEDULER_CNT pins are used for immidiate activation button   //
//  The following SCHEDULER_CNT pinst are used for maximum duration slider      //
//  Pin ( SCHEDULER_CNT*(TIME_CNT+3) ) +3 used for system sleep button          //
//  Pin ( SCHEDULER_CNT*(TIME_CNT+3) ) +4 used for system disable button        //
//  Pin ( SCHEDULER_CNT*(TIME_CNT+3) ) +5 used for slider of number of days     //
//    to disable system                                                         //
// User must ensure that number of used Virtual pins will not exceed 127        //
//  So the following rule must be kept when selecting SCHEDULER_CNT and TIME_CNT//
//                                                                              //
//                SCHEDULER_CNT*(TIME_CNT+3) < 123                              //
//                                                                              //
// Also SCHEDULER_CNT must be smaller or equal to 16                            //               
//////////////////////////////////////////////////////////////////////////////////
// define time inputs Virtual Pin (Time Input Widget) for timer 0 of scheduler 0 
#define VP_SWCH0_TIME0 V0

// define activation duration Virtual Pin (Slider Widget) for Scheduler 0 (V16)
#define VP_ACTV_DUR_SCHD0 (VP_SWCH0_TIME0+(TIME_CNT*SCHEDULER_CNT))

// define immidiate activation Virtual Pin (Button Widget) for Scheduler 0 (V20)
#define VP_IMMD_ACTV_SCHD0 (VP_ACTV_DUR_SCHD0+SCHEDULER_CNT)

// define max duration Virtual Pin (Slider Widget) Scheduler 0 (V24)
#define VP_MAX_DUR_SCHD0 (VP_IMMD_ACTV_SCHD0+SCHEDULER_CNT)

// define disable system input and number of days to disable input
#define VP_SLEEP_SYS (VP_MAX_DUR_SCHD0+SCHEDULER_CNT+3)
#define VP_DISBL_SYS (VP_MAX_DUR_SCHD0+SCHEDULER_CNT+4)
#define VP_DISBL_DAYS (VP_DISBL_SYS+1)

////////////////////////////////////////////////////////////////////////////////////////
// time input arrays
////////////////////////////////////////////////////////////////////////////////////////
int  start_time_min[SCHEDULER_CNT][TIME_CNT];       // Array of TIME_CNT timers per scheduler containing 
                                                    //  minuet in day for scheduler activation
bool weekdays[SCHEDULER_CNT][TIME_CNT][8];          // Array of 8 days (day 0 not used) for each time
int  active_duration_sec[SCHEDULER_CNT][TIME_CNT+1];// duration per time input (in sec) additional "timer" is used to save the default active duration
int  max_duration_sec[SCHEDULER_CNT];               // max duration per scheduler (in sec)

bool scheduler_state[SCHEDULER_CNT];               // save state of scheduler (for sleep mode)

int  scheduler_num_array[SCHEDULER_CNT];            // array of scheduler number. a pointer to elements from this array is passed as parameter to the timer callback function
                                                    // the array is initialized (and never change) with 0,1,2,3... at setup

bool system_sleep_mode = false;                     // current sleep mode  
/////////////////////////////////////////////////////////////////////////////
// TIMER IDs variables and arrays
///////////////////////////////////////////////////////////////////////////////////////
// when activating a scheduler a timer is set for the configured duration
// when the duration ends the scheduler is turned off by the timer
// the id of the timer is saved using end_timer_id
// if scheduler is manually turned off the end_timer_id is used to stop the timer.
// end_timer_id is initialized to 32 (per entry) at the setup section
int  end_timer_id[SCHEDULER_CNT];

// saves id of main timer
int  main_timer_id;

// system disable timer
int  disable_timer_id = 32;                    // when system is disabled a timer is set to wake the system disable_timer_id saves this timer ID

// system disable days 
int  system_disable_days_msec;                 // VP_DISBL_DAYS defines days to disable the system (including today) given in msec

// timer object declaration
BlynkTimer SystemTimer;                              // for turning off schedulers at end time
BlynkTimer SchedulerTimer;                        // for calling activetoday and for turning off sleep/disable modes

// this code use Real Time Clock widget in the blynk app to keep the clock updated from net
WidgetRTC rtc;

BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
  Blynk.syncAll();
}

//////////////////////////////////////////////////////////////////////////////////////
// End of Sleep and Disable Modes                                                   //
//  (They will probebly not be used together so if one ends the other must end too) //
//////////////////////////////////////////////////////////////////////////////////////
void sleep_disable_mode_off()
{
   system_sleep_mode = false;       
   // iterate over all schedulers and reactivate tasks if needed.
   for (int scheduler_cnt = 0;  scheduler_cnt< SCHEDULER_CNT; scheduler_cnt++) 
      if(scheduler_state[scheduler_cnt]) SchedulerTask_OnOff(HIGH, scheduler_cnt); 

   // enable main timer (only if in disable mode)    
   SystemTimer.enable(main_timer_id);  
   // set disable and sleep buttons to off
   Blynk.virtualWrite(VP_DISBL_SYS,0);
   Blynk.virtualWrite(VP_SLEEP_SYS,0);

   // set disable timer id to 32 to prevent disabling system timer by mistake
   SchedulerTimer.deleteTimer(disable_timer_id);
   disable_timer_id = 32;
}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////                 BLYNK_WRITE_DEFAULT                                 ///////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////    
// BLYNK_WRITE_DEFAULT enables defining configurable number of schedulers and 
//  time inputs without chaning the code (only change defines)
// BLYNK_WRITE_DEFAULT takes any write to virtual pin and parse it using request.pin 
BLYNK_WRITE_DEFAULT() { 
  int pin = request.pin;      // Which exactly pin is handled?
 
  // save current time in msec. this is used to calculate msecs until end of today 
  //  used for disable and sleep modes
  int nowmseconds = 0; 
  if (system_disable_days_msec!=0)
       nowmseconds = ((hour() * 3600) + (minute() * 60) + second())*1000;

  String currentDate = String(day()) + "\\" + month() + "\\" + year();
      
  ///////////////////////////////////////////////////////////////////////
  // DISABLE DAYS SLIDER INPUT                                         //
  ///////////////////////////////////////////////////////////////////////
  if (pin == VP_DISBL_DAYS) system_disable_days_msec = param.asInt()*86400000; // V33
  ///////////////////////////////////////////////////////////////////////
  // SYSTEM DISABLE BUTTON INPUT                                       //
  ///////////////////////////////////////////////////////////////////////
  else if(pin == VP_DISBL_SYS )                                           // V32
  {
    if (param.asInt()!=0){ 

      // disable main time tick 
      SystemTimer.disable(main_timer_id);   

      // turn off all active schedulers  
      Serial.println(String("Disable all"));
      for (int scheduler_cnt = 0;  scheduler_cnt< SCHEDULER_CNT; scheduler_cnt++) 
          scheduler_turn_off((void*) &scheduler_num_array[scheduler_cnt]);
 
      // create a timer to wake the system up after system_disable_days_msec are over
      SchedulerTimer.deleteTimer(disable_timer_id); // make sure no disable timer is active
      disable_timer_id = SchedulerTimer.setTimeout(system_disable_days_msec-nowmseconds,sleep_disable_mode_off);
 
      // print current data on VP_DISBL_SYS button
      Blynk.setProperty(VP_DISBL_SYS, "onLabel", currentDate);
    }
  else
    { // if system is manually enabled delete disable timer and enable system time tick
       sleep_disable_mode_off();       
    }  
  }
  ////////////////////////////////////////////////////////////////////////////////////
  //  SYSTEM SLEEP BUTTON INPUT                                                     //
  ////////////////////////////////////////////////////////////////////////////////////
  else if (pin == VP_SLEEP_SYS) {
       if (param.asInt()!=0){ 
        
          system_sleep_mode = true;
          // turn off all tasks (do not disable the schedulers )
          for (int task_cnt = 0;  task_cnt< SCHEDULER_CNT; task_cnt++) 
            SchedulerTask_OnOff(LOW, task_cnt);
            
          // create a timer to wake the system up after system_disable_days_msec are over
          SchedulerTimer.deleteTimer(disable_timer_id); // make sure no disable timer is active
          disable_timer_id = SchedulerTimer.setTimeout(system_disable_days_msec-nowmseconds,sleep_disable_mode_off);
          // print current data on VP_SLEEP_SYS button
          Blynk.setProperty(VP_SLEEP_SYS, "onLabel", currentDate);
      
        }
        else {// if system is manually out of seep mode delete disable timer and enable schedulers
          sleep_disable_mode_off();
        }
  }
  ////////////////////////////////////////////////////////////////////////////////////
  // MAX DURATION SLIDER INPUT                                                      //
  ////////////////////////////////////////////////////////////////////////////////////
  else if (pin >= VP_MAX_DUR_SCHD0) {                                       //V24..V27
     if (pin < VP_MAX_DUR_SCHD0+SCHEDULER_CNT) max_duration_sec[pin-VP_MAX_DUR_SCHD0] = param.asInt()*60;
  }
  ///////////////////////////////////////////////////////////////////////////////////
  // SCHEDULER IMMIDIATE TASK ACTIVATION BUTTON INPUT                              //
  ///////////////////////////////////////////////////////////////////////////////////
  else if (pin >= VP_IMMD_ACTV_SCHD0) {                                     //V20..V23
    if ( pin <(VP_IMMD_ACTV_SCHD0+SCHEDULER_CNT) ) {
      if (param.asInt()==0)
        scheduler_turn_off((void*) &scheduler_num_array[pin-VP_IMMD_ACTV_SCHD0]);
      else
        scheduler_turn_on(pin-VP_IMMD_ACTV_SCHD0,TIME_CNT);
    }
  }
  ///////////////////////////////////////////////////////////////////////////////////
  // DEFAULT ACTIVE DURATION SLIDER INPUT                                          //
  ///////////////////////////////////////////////////////////////////////////////////
  else if (pin >= VP_ACTV_DUR_SCHD0) {                                       //V16..V19
    // default duration is written in extra "timer"
    if (pin <VP_ACTV_DUR_SCHD0+SCHEDULER_CNT) active_duration_sec[pin-VP_ACTV_DUR_SCHD0][TIME_CNT] = param.asInt()*60;
  }
  ///////////////////////////////////////////////////////////////////////////////////
  // TIME INPUT                                                                    //
  ///////////////////////////////////////////////////////////////////////////////////
  else if (pin >= VP_SWCH0_TIME0) {                                          // V0..V15
    int scheduler_num = (pin-VP_SWCH0_TIME0)/TIME_CNT;
    int time_num = (pin-VP_SWCH0_TIME0)%TIME_CNT;
    
    if ((scheduler_num<SCHEDULER_CNT) &( time_num<TIME_CNT)) {
       TimeInputParam t(param); // convert to time input parameter
       
       // Process start time
       start_time_min[scheduler_num][time_num]= -1;  // start_time_min of -1 indicate no start time is set (will never match current time)
  
       if (t.hasStartTime())
       {
         Serial.println(String("Start: ") + t.getStartHour() + ":" + t.getStartMinute() + ":" + t.getStartSecond());
         Serial.println(String("Start in sec: ") + param[0].asLong() + String(" for scheduler") + scheduler_num + String(" time_no: ") + time_num);
    
         start_time_min[scheduler_num][time_num]=param[0].asLong()/60;     
       }
       //////////////////////////////////////////////////////////////////////////////
       // check if end time is received convert and save it as day time in seconds //
       //////////////////////////////////////////////////////////////////////////////
       active_duration_sec[scheduler_num][time_num] = -1;
  
       if (t.hasStopTime())
       {
         Serial.println(String("Stop: ") + t.getStopHour() + ":" + t.getStopMinute() + ":" + t.getStopSecond());
         Serial.println(String("Stop in sec: ") + param[1].asLong() + String(" for scheduler") + scheduler_num + String(" time_no: ") + time_num);

         active_duration_sec[scheduler_num][time_num] = (param[1].asLong()-param[0].asLong());
   
         // if end time is smaller than start time this means end time is at next day
         if (active_duration_sec[scheduler_num][time_num]<0) 
           active_duration_sec[scheduler_num][time_num] = 86400+active_duration_sec[scheduler_num][time_num];    
       }
       /////////////////////////////////////////////////////////////////////////////
       // Process weekdays (1. Mon, 2. Tue, 3. Wed, ...)                          //
       /////////////////////////////////////////////////////////////////////////////
       for (int i = 1; i <= 7; i++) {
         weekdays[scheduler_num][time_num][i] = t.isWeekdaySelected(i);
    
         if (t.isWeekdaySelected(i)) 
           Serial.println(String("Day ") + i + " is selected"); 
       }
       
       Serial.println();
    }    
  }
} 
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////             END  OF  BLYNK_WRITE_DEFAULT                            ///////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////   

/////////////////////////////////////////////////////////////////
// Handle Scheduler events (from app or from timers )          //
/////////////////////////////////////////////////////////////////

// handle scheduler ON
void scheduler_turn_on(byte scheduler_num , byte time_no){

     // save scheduler state (in case of system sleep)
     scheduler_state[scheduler_num] = true; 
    // turn on the task 
    if (!system_sleep_mode) SchedulerTask_OnOff(HIGH, scheduler_num);
    
    // if new timer is set before another one ended then disable previous timer
    if (end_timer_id[scheduler_num]!=32) SystemTimer.deleteTimer(end_timer_id[scheduler_num]);
    
    // calculate activation duration for this call of scheduler_turn_on
    int curr_active_duration_sec ;
 
    if (active_duration_sec[scheduler_num][time_no]<0) // if no end time defined use default duration
      curr_active_duration_sec = active_duration_sec[scheduler_num][TIME_CNT]; 
    else if ( (active_duration_sec[scheduler_num][time_no]<max_duration_sec[scheduler_num]) |
              (max_duration_sec[scheduler_num]< 120)                                  )
      curr_active_duration_sec = active_duration_sec[scheduler_num][time_no];
    else
      curr_active_duration_sec = max_duration_sec[scheduler_num];
      
    // create time as string to print on activation button
    char Time_print[10];
    sprintf(Time_print, "%02d:%02d", hour(), minute());
    
    // set button on and write the activation time on the button 
    Blynk.setProperty(VP_IMMD_ACTV_SCHD0+scheduler_num, "onLabel", String(Time_print));
    Blynk.virtualWrite(VP_IMMD_ACTV_SCHD0+scheduler_num,1);

    // set timer to turn off scheduler 
    // the callback function scheduler_turn_off needs scheduler number.
    //  the scheduler number is passed as a void pointer. It must be pointing to a static/global value hance the scheduler_num_array
    if (curr_active_duration_sec != 0) {
      end_timer_id[scheduler_num]=SystemTimer.setTimeout(curr_active_duration_sec*1000, scheduler_turn_off, (void*) &scheduler_num_array[scheduler_num]);
 
      Serial.println(String("turn ON scheduler: ") + scheduler_num + String(" for duration: ") + curr_active_duration_sec + String("sec "));
    }
}

// handle scheduler OFF
// this function is called as callback from timer so void input pointer is used
void scheduler_turn_off(void* scheduler_num ){
    
    // convert void pointer to regular byte
    byte scheduler_num_tmp = *(byte*)scheduler_num;

    scheduler_state[scheduler_num_tmp] = false; 
    // set task off
    SchedulerTask_OnOff(LOW, scheduler_num_tmp);

    // delete associated timer
    SystemTimer.deleteTimer(end_timer_id[scheduler_num_tmp]);
    end_timer_id[scheduler_num_tmp]=32;
    Serial.println(String("turn OFF scheduler: ") + scheduler_num_tmp);

      // reset button
    Blynk.virtualWrite(scheduler_num_tmp + VP_IMMD_ACTV_SCHD0,0); 
}
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//// the following function is called every 60 seconds by a timer                            ////
////  the function checks if a start time is reached and if yes it will call                 ////
////   the scheduler_turn_on function (to turn on the scheduler and                          ////
////                                   set timer for turning off scheduler)                  ////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
int  last_min = -1;                                 // saves the last minute that activetoday was started at
                                                    // this is later used to ensure activetoday will not be started twice at the same minute 
                                               
void activetoday(){         // check if schedule #1 should run today

  // set wifi led if no connection
  if(Blynk.connected()) 
    digitalWrite(WIFI_LED,LOW);
  else
    digitalWrite(WIFI_LED,HIGH);
  
  int now_min = minute()+hour()*60;
  Serial.println(String("activetoday at: ") + now_min);

  if (now_min != last_min) { // check if activetoday was already started in this minute

    last_min = now_min;

    if(year() != 1970){ // if RTC was not started do nothing 

      // adjust day number for sunday
      int dayadjustment = -1;  
      if(weekday() == 1)
        dayadjustment = 6; // needed for Sunday Time library is day 1 and Blynk is day 7
    
      // loop on all time inputs
      for (int scheduler_cnt = 0;  scheduler_cnt< SCHEDULER_CNT; scheduler_cnt++) {
         for (int timer_cnt = 0;  timer_cnt< TIME_CNT; timer_cnt++) {
              ///////////////////////////////////////////////////////////
              // check if time input start is valid and today is set and current hour and minute are equal
              // if condition is true then activate the scheduler 
                if ((weekdays[scheduler_cnt][timer_cnt][weekday() + dayadjustment]==true) &
                    (now_min == start_time_min[scheduler_cnt][timer_cnt])                  ){
                        scheduler_turn_on(scheduler_cnt,timer_cnt);
                    }
         }
      }
    }
  }   
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//// GPIO Immidiate Activation                                                               ////
////  The following function is used to bind GPIO into immidiate activation.                 ////
////  It pools on the given GPIOs (one per scheduler)                                        ////
////    and if set will cause immidiate activation of the scheduler                          ////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
void gpio_to_virtualpin() {
     int gpio_state;
     for (int scheduler_cnt = 0;  scheduler_cnt< SCHEDULER_CNT; scheduler_cnt++) {
       if (digitalRead(in_gpio_pins[scheduler_cnt])==!in_gpio_polarity[scheduler_cnt]) {
          
            if (last_in_gpio_pins[scheduler_cnt]==LOW){
              Serial.println(String("scheduler_state==: ") + scheduler_state[scheduler_cnt]);
              if (scheduler_state[scheduler_cnt]){
                Blynk.virtualWrite(VP_IMMD_ACTV_SCHD0+scheduler_cnt,0);
                Serial.println("Switch OFF \n"); }
              else {
                Blynk.virtualWrite(VP_IMMD_ACTV_SCHD0+scheduler_cnt,1);
                Serial.println("Switch ON \n"); }
                  
              Blynk.syncVirtual(VP_IMMD_ACTV_SCHD0+scheduler_cnt);
            }
            last_in_gpio_pins[scheduler_cnt]=HIGH;
       }
       else
         last_in_gpio_pins[scheduler_cnt]=LOW;
    }
}

/////////////////////////////////////
// BLYNK 
void setup() {
  // Debug console
  Serial.begin(115200);  


  // reset output pins used and system variables
  for (int i = 0; i<SCHEDULER_CNT ; i++){
//    SchedulerTaskInit(i);
    pinMode(task_gpio_pins[i], OUTPUT);  // set output pins

    pinMode(in_gpio_pins[i], INPUT);

    SchedulerTask_OnOff(LOW,i );                 // turn tasks off
    end_timer_id[i] = 32;                // reset end timer id's
    scheduler_num_array[i] = i;          // initialize scheduler number array 
    scheduler_state[i] = false;
     
    for (int j = 0; j<TIME_CNT ; j++) {
      active_duration_sec[i][j] = -1;
      start_time_min[i][j]= -1;           // start_time_min of 255 indicate no start time is set
    }
    
    active_duration_sec[i][TIME_CNT] = 0; // this is the duration from the slider
  }
  
  pinMode(WIFI_LED, OUTPUT);
  

  WiFi.hostname(host);
  Blynk.begin(auth, ssid, pass);

  Serial.printf("Blynk_begin \n");

  MDNS.begin(host);

  httpUpdater.setup(&httpServer, update_path, update_username, update_password);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local%s in your browser and login with username '%s' and password '%s'\n", host, update_path, update_username, update_password);
  
  // active today is called every 55 secondes to check if start time was reached
  // using 55 sec ensures that active today will visit every minuet even if 
  //   RTC gets time update of up to 5 sec (unreasonable big change) from the web
  main_timer_id = SystemTimer.setInterval(58000L, activetoday);  

  // check GPIOs every 100msec
  SystemTimer.setInterval(100L,gpio_to_virtualpin);

  // Blynk sync
  setSyncInterval(10 * 60); // Sync interval in seconds (10 minutes)

}

void loop() {
  Blynk.run();
  SystemTimer.run();
  SchedulerTimer.run();
  httpServer.handleClient();
 
}
