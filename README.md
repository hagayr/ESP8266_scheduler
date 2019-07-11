# ESP8266_scheduler
This is an Arduino project for the ESP8266 device family using the Blynk IoT Platform.
The ESP8266_scheduler implements a scheduler that can be used to activate and deactivate hardware and software components at specific times given by the Blynk time input widget.

    • Parametric number of schedulers 
    • Each scheduler can be used to activate/de-activate different task
        ◦ Hardware tasks activate relays, motors, leds… using GPIOs
        ◦ Software tasks executes functions (send http requests...). 
          Functions should be simple enough not to block the Blynk operation.
    • Parametric number of time settings (Time Input widget) per scheduler
        ◦ Time setting can have start time only or start time and end time
    • Scheduler can be activated/De-activated immediately from Blynk App using Button widget
    • Scheduler can be activated/De-activated immediately from a button at the device (GPIO)
    • Slider widget from Blynk App defines duration before de-activating scheduler 
        ◦ when activated from Time Input and end time is not defined
        ◦ When activated from Button widget
        ◦ When activated from Device button
        ◦ when not de-activated from button
    • Slider widget from Blynk App defines maximum duration
        ◦ to prevent multiple overlapping activations (when using a boiler...)
        ◦ setting maximum duration to 0 disable this feature 
    • System Sleep Button from Blynk App can temporally de-activate all active tasks 
        ◦ when end of sleep all tasks return to same position
        ◦ for example for irrigation systems to temporally stop sprinklers when servicing
    • System Disable Button from Blynk App de-activate all active tasks 
        ◦ when end of disable only new start times (or immediate activation) can activate tasks
        ◦ for example to disable boiler system when out for several days
    • Slider from Blynk app defines sleep/disable time with resolution of days
    • Over The Air Firmware update (via http://esp8266_scheduler.local/firmware on local lan)
    • Automatic Connect (using tzapu WififManager)
        ◦ use arduino library manager or find it at http://tzapu.com/esp8266-wifi-connection-manager-library-arduino-ide/
        ◦ no need to hardcode the wifi ssid, password or the Blynk auth
        ◦ to connect turn the device on and look for SSID="AutoConnectAP" on your phone wifi and connect it (no password).
          Open browser at http://192.168.4.1 and set your router SSID, Password and Blynk auth from there.
          After connecting to your router all settings are saved in EEPROM and will be used at next connection.
        ◦ User defined reset gpio can be used (press for 10 sec) to delete EEPROM and input new SSID, Password or Blynk auth 
       
The following Files are included:
    • passwd.h – includes user name and password for Over The Air firmware upload
    • DeviceDefines.h – includes all defines to be updated by user:
        ◦ current configurations used are sonoff 4ch pro, NodeMcu, Sonoff th16, and Aptinex RP4CE8. Not all are fully checked
        ◦ SchedulerTask_OnOff function for activating on/off tasks
        ◦ user can change this to implement software tasks or use the provided function for GPIO on/off
        ◦ SchedulerTaskInit Init function for tasks
    • AutoConn.h and AutoConn.ino -for the WifiManager Auto connect.
    • ESP8266_scheduler.ino – main Blynk code



