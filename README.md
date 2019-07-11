<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
	<meta http-equiv="content-type" content="text/html; charset=utf-8"/>
	<title></title>
	<meta name="generator" content="LibreOffice 6.0.7.3 (Linux)"/>
	<meta name="created" content="00:00:00"/>
	<meta name="changedby" content="hagay "/>
	<meta name="changed" content="2019-07-11T21:52:23.834648840"/>
	<style type="text/css">
		@page { margin: 0.79in }
		p { margin-bottom: 0.1in; line-height: 115% }
		h1 { margin-bottom: 0.08in }
		h1.western { font-family: "Liberation Sans", sans-serif; font-size: 18pt }
		h1.cjk { font-family: "Noto Sans CJK SC Regular"; font-size: 18pt }
		h1.ctl { font-family: "Nachlieli CLM"; font-size: 18pt }
		pre.cjk { font-family: "DejaVu Sans Mono", monospace }
		a:link { so-language: zxx }
	</style>
</head>
<body lang="zh-CN" dir="ltr">
<h1 class="ctl"><font face="Liberation Sans, sans-serif"><span lang="en-US">#
ESP8266_scheduler</span></font></h1>
<pre class="ctl"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">This is an Arduino project for the ESP8266 device family using the Blynk IoT Platform.</span></font></font>
<font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">The ESP8266_scheduler implements a scheduler that can be used to activate and deactivate hardware and software components at specific times given by the Blynk time input widget.</span></font></font></pre>
<ul>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Parametric number of schedulers </span></font></font></pre>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Each scheduler can be used to activate/de-activate different task</span></font></font></pre>
	<ul>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Hardware tasks activate relays, motors, leds… using GPIOs</span></font></font></pre>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Software tasks executes functions (send http requests...). </span></font></font>
<font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Functions should be simple enough not to block the Blynk operation.</span></font></font></pre>
	</ul>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Parametric number of time settings (Time Input widget) per scheduler</span></font></font></pre>
	<ul>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Time setting can have start time only or start time and end time</span></font></font></pre>
	</ul>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Scheduler can be activated/De-activated immediately from Blynk App using Button widget</span></font></font></pre>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Scheduler can be activated/De-activated immediately from a button at the device (GPIO)</span></font></font></pre>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Slider widget from Blynk App defines duration before de-activating scheduler </span></font></font></pre>
	<ul>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">when activated from Time Input and end time is not defined</span></font></font></pre>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">When activated from Button widget</span></font></font></pre>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">When activated from Device button</span></font></font></pre>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">when not de-activated from button</span></font></font></pre>
	</ul>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Slider widget from Blynk App defines maximum duration</span></font></font></pre>
	<ul>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">to prevent multiple overlapping activations (when using a boiler...)</span></font></font></pre>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">setting maximum duration to 0 disable this feature </span></font></font></pre>
	</ul>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">System Sleep Button from Blynk App can temporally de-activate all active tasks </span></font></font></pre>
	<ul>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">when end of sleep all tasks return to same position</span></font></font></pre>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">for example for irrigation systems to temporally stop sprinklers when servicing</span></font></font></pre>
	</ul>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">System Disable Button from Blynk App de-activate all active tasks </span></font></font></pre>
	<ul>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">when end of disable only new start times (or immediate activation) can activate tasks</span></font></font></pre>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">for example to disable boiler system when out for several days</span></font></font></pre>
	</ul>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Slider from Blynk app defines sleep/disable time with resolution of days</span></font></font></pre>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><span lang="en-US"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Over The Air Firmware update (via <a href="http://esp8266_scheduler.local/firmware">http://esp8266_scheduler.local/firmware</a> on local lan(</span></font></font></span></pre>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Automatic Connect (using tzapu WififManager)</span></font></font></pre>
	<ul>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><span lang="en-US"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">use arduino library manager or find it at <a href="http://tzapu.com/esp8266-wifi-connection-manager-library-arduino-ide/">http://tzapu.com/esp8266-wifi-connection-manager-library-arduino-ide/</a></span></font></font></span></pre>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">no need to hardcode the wifi ssid, password or the Blynk auth</span></font></font></pre>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">to connect turn the device on and look for SSID=&quot;AutoConnectAP&quot; on your phone wifi and connect it (no password).</span></font></font>
<span lang="en-US"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">Open browser at <a href="http://192.168.4.1/">http://192.168.4.1</a> and set your router SSID, Password and Blynk auth from there.</span></font></font></span>
<font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">After connecting to your router all settings are saved in EEPROM and will be used at next connection.</span></font></font></pre>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">User defined reset gpio can be used (press for 10 sec) to delete EEPROM and input new SSID, Password or Blynk auth </span></font></font></pre>
	</ul>
	<pre class="ctl" style="margin-bottom: 0.2in"> </pre>
</ul>
<pre class="ctl" style="margin-bottom: 0.2in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">The following Files are included:</span></font></font></pre>
<ul>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">passwd.h – includes user name and password for Over The Air firmware upload</span></font></font></pre>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">DeviceDefines.h – includes all defines to be updated by user:</span></font></font></pre>
	<ul>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">current configurations used are sonoff 4ch pro, NodeMcu, Sonoff th16, and Aptinex RP4CE8. Not all are fully checked</span></font></font></pre>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">SchedulerTask_OnOff function for activating on/off tasks</span></font></font></pre>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">user can change this to implement software tasks or use the provided function for GPIO on/off</span></font></font></pre>
		<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">SchedulerTaskInit Init function for tasks</span></font></font></pre>
	</ul>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">AutoConn.h and AutoConn.ino -for the WifiManager Auto connect.</span></font></font></pre>
	<li/>
<pre class="ctl" style="margin-bottom: 0.08in"><font face="Nakula"><font size="3" style="font-size: 12pt"><span lang="en-US">ESP8266_scheduler.ino – main Blynk code</span></font></font></pre>
</ul>
<pre class="ctl">
</pre>
</body>
</html>
