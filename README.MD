# TIMERS V2 by Cemu0

## I'm lazy to read, give me some video :) 

[![Watch the video](https://img.youtube.com/vi/iN_e02LLBGo/default.jpg)](https://youtu.be/iN_e02LLBGo)

<!-- <iframe width="560" height="315" src="https://www.youtube.com/embed/iN_e02LLBGo" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe> -->

## WHAT IS THIS?

- [x] And you also need to control your gardens watering, pumps or lights **by schedule**?
  
- [x] You don't wanna buy **multiple timers** and want a centralised control hub?

- [x] You need timerS with **Global** connection & **control** but you also need it to **work offline**?

## HOW YOU SOLVE THAT?

- By using ESPHome you can create your timers without knowing much about C++ code (except you want to modify this repo).

- This will connect to your Home Assistant(HA) via an MQTT broker to provide remote control. I personally found that tracking the history of each relay through HA is quite useful for debugging and tracking the watering status of plans.

- This will use an SD card to save your schedules and will turn on/off your devices according to that schedule, therefore you don't need an internet or wifi connection to run your stuff schedule.


## LIMITATION 

- This repo doesn't have any TIME menu, so you need to have a wifi connection to set the time automatically over SNTP.

- Currently, this repo code only supports 8 relays, but in theory, you can extend it to ... IDK, 10 relays without external module and 8x8x5 = 320? relays (in theory) with pcf8574_hubs. Moreover in theory this can control others in LAN devices with a bit change in [code]https://github.com/esphome/feature-requests/issues/52). But I leave that to you.

- The number of schedule control for each relay is not limited (it should work well up to a hundred timers in total).

Note: Although all the relays will be connected to HA without any config and you can control any Relay directly, the online control just replaces the screen with text sensors (with you should disable history track in HA of this) and a cover (like the gate control ) so it is quite hard to control online.
 
## INSTRUCTION 
### REQUIREMENT:
#### Hardware:
- Wemos D1 R32 board.
- A logging shield for Arduino (you can use an SD card module and an RTC module like ds1307, but you will need to wring a lot, so I found that modifying the shield make things a lot easier), Also prepare an SD card too (should be lower than 
Rotary encoder module to navigate through the menu. If you only want to control through the internet, you can skip this, but it will be quite hard to control.
- 20x4 LCD screen come with an I2C module.
- Relay module, any relay module with low-level control (if 5v) should work you can use an SSR relay module like mine, since the SSR does not create sparks when triggered this can provide more stability for the esp, but in fact, I used the traditional relay module with esp32 for years without any problems until now (even without any snubber). To be safe if you using a contact relay module you should add a snubber circuit through each load to prevent EMI from killing your circuit. 
- You can choose to add a DTH sensor for ... fun, I mean to measure the temp and hump. Thanks to espHome this value also report to your HA.
- Some things to put all that stuff into, A 3d print box design also comes with this repo.
#### Software:
- If you need the timer to work online or remote control, An MQTT broker is needed, I recommend using the [Home Assistant Add-on: Mosquitto broker](https://github.com/home-assistant/addons/blob/master/mosquitto/DOCS.md) for easy setup 
- To upload the code you need to [install ESPHome on your local machine by using python](https://esphome.io/guides/installing_esphome.html), but in fact, you can simply plug the device into your HA server to flash the ESP through the [ESPHome extension](https://esphome.io/guides/getting_started_hassio.html), anyway in my case this system run remotely so I use my local machine to upload the code. 

### STEP 1: Wiring

If you were familiar with ESPhome, you can simply check out the config file (timer.yaml) to see with connections go.  
If using a logging shield you can skip the SPI and I2C connection wiring for the RTC. But you should cut the back of the shield-like this if you plan to use pins 36 & 39 because this board was designed for Arduino and those pins only accept input.
![cutpin][0]
We will use the default SPI connection, so it going like this.  
> **SD_CS 5**  
**SCK_PIN 18**  
**MISO_PIN 19**  
**MOSI_PIN 23**  

And the RTC module, the LCD screen needs to connect to this bus too (the 21 & 22 pins already labelled as SDA & SCL in R32 board). Note that if you use the pcf8574, you should use other I2C pins because in my case using this can lead to many bugs, like RTC and LCD not working and I am not sure why)  
> **SDA 21**  
**SCL 22**  

The encoder module will be like, you can swap DT and CLK pins or pin numbers in setup to revert scroll direction.  
> **SW 16**  
**DT 14**  
**CLK 27**  

![requirement][1]

**Important NOTE:** [you need to add a 330ohm resistor to keep the Wifi connection on the R32 board stable](https://github.com/espressif/arduino-esp32/issues/2144#issuecomment-657672609)

After doing that connection, you basically complete the necessary wire, now the relay timer need relays. You can use my pins config below
> **relay1: 2**  
**relay2: 4**  
**relay3: 12**  
**relay4: 13**  
**relay5: 33**  
**relay6: 25**  
**relay7: 26**  
**relay8: 32**  

*NOTE:* if you use the pcf8574 board you can use any relay pin above to set up an I2C connection. Importantly, I tested the pcf8574 with the relays, everything worked without load, but if have a load on the AC side, the pcf8574 reset itself and lost control for a while. I'm not clearly sure why. This held event using SSR relay (SSR do reduce the problem), and it looks like many people get [the same issue](https://www.letscontrolit.com/forum/viewtopic.php?t=7043) at this so can try another pin expand boards, add snubber or move the expansion board faraway from relays module.

If you want to connect to a sensor, you can use pin 15 or 17 for this reason. 
> **DTH data pin: 15**

Full connection going like this
![fully connection][2]

### STEP2: Upload
Download this repo and open it in your favourite editor (like the VScode) 

Edit the "timer.yaml" file (wifi, MQTT, OTA setting or pins) to your custom setup.
![edit][3]

Plugging in your hardware (the MicroUSB on esp32 board) and upload the code by running `esphome timers.yaml upload` inside the repo directory

### STEP3: Enjoy
After upload, if your wiring correctly, you should be able to use the project. Again the first time you boot the system, you will need to have a WiFi connection to let the ESP set the RTC time automatically. After that, you needn't.
If you don't know about [ESPhome](https://esphome.io/index.html), go and check out many devices you can integrate with.

[0]: images/cutit.jpg
[1]: images/Requirement.png
[2]: images/Timers_wiring_bb.png
[3]: images/edit.png
