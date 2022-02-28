# TIMERS V2 by Cemu0

### I'm lazy to read, give me some video :) 

...

### WHAT IS THIS?

- [x] And you also need to control your gardens watering, pumps or lights **by schedule**?
  
- [x] You don't wanna buy **multiple timers** and want a centralised control hub ?

- [x] You need timerS with **Global** connection & **control** but you also need it to **work offline**?

### HOW YOU SOLVE THAT?

- By using ESPHome you can create your timers without knowing much about C++ code (except you want to modify this repo) 

- This will connect to your Home Assistant to provide remote control 

- This will use an SD card to save your schedules and will turn on/off your devices according to that schedule, therefore you don't need internet or wifi connection to run your stuff schedule 


### LIMITATION 



# Instruction 

![requirement][1]

![fully connection][2]




[1]: images/Requirement.png
[2]: images/Timers_wiring_bb.png

NOTE: [you need to add a 330ohm resistor to keep the Wifi connection on R32 board stable](https://github.com/espressif/arduino-esp32/issues/2144#issuecomment-657672609)

## build
`esphome timers.yaml compile`

## upload
`esphome timers.yaml upload`

