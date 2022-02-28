#pragma once
#include "esphome.h"

#include <vector>
using namespace std;

#define USER_COUNT 1
#define UPDATE_TIMER_TIME 1000

#define MAX_RELAY 8

template <class I>
String FormatDigit(I data){
    if(String(data).length()<2)return "0" + String(data);
    return String(data);
}

enum timerStuff{
    START_HOUR = 0,
    START_MINUTE = 1,
    END_HOUR = 2,
    END_MINUTE = 3,
};

struct relay_time{
    // h_start,m_start,h_end,m_end
    byte time[4] = {0,0,0,0};
    //since bit field not suitable i just use a byte here
    // nope sat fri thu wed tue mon sun
    byte date = 0b01111111;
    
    // Mon to Sun : 0 to 6
    bool activateDay(uint8_t day){
        return (date & (1<<day));
    }
};

vector<vector<relay_time>> timer_vector;

#define TARGET_RELAYS {id(relay0),id(relay1),id(relay2),id(relay3),id(relay4),id(relay5),id(relay6),id(relay7)} //,id(relay8)
        

//timer menu
class timer_manager : public Component{ //
private:
    unsigned long long update_time;
    
public:
    bool activate = true;

    void toggleRelayState(int relay){
        //need to reload every time 
        gpio::GPIOSwitch taget_relay[] = TARGET_RELAYS;
        //NOTE: taget_relay[relay].toggle(); //not work :(
        changeRelayState(relay,!taget_relay[relay].state);
    }
    void changeRelayState(int relay, bool state){
        gpio::GPIOSwitch taget_relay[] = TARGET_RELAYS;
        if(taget_relay[relay].state == state)
            return;
        //TODO: looking for a better way 
        switch (relay)
        {
        case 0:
            id(relay0).toggle();
            break;

        case 1:
            id(relay1).toggle();
            break;

        case 2:
            id(relay2).toggle();
            break;

        case 3:
            id(relay3).toggle();
            break;
        
        case 4:
            id(relay4).toggle();
            break;
        
        case 5:
            id(relay5).toggle();
            break;
        
        case 6:
            id(relay6).toggle();
            break;
        
        case 7:
            id(relay7).toggle();
            break;

        default:
            break;
        }
    }

    // void changeRelayState(int relay, bool state){
    //     gpio::GPIOSwitch taget_relay[] = TARGET_RELAYS;
    //     if(taget_relay[relay].state)
    //     {
    //         if(!state)
    //             taget_relay[relay].turn_off();
    //             // taget_relay[relay].publish_state(false);
    //     }
    //     else
    //     {
    //         if(state)
    //             taget_relay[relay].turn_on();
    //             // taget_relay[relay].publish_state(true);
    //     }     
    // }
    //work
    bool checkRelayStage(int relay){
        gpio::GPIOSwitch taget_relay[] = TARGET_RELAYS;
        return taget_relay[relay].state;
    }

    relay_time* addTimer(int relay){
        relay_time rt;
            rt.date = 0b01111111;
            rt.time[0] = 0;
            rt.time[1] = 0;
            rt.time[2] = 0;
            rt.time[3] = 0;
        timer_vector[relay].push_back(rt);

        return &timer_vector[relay].back();
    }

    bool removeTimer(int relay, int timerNo){
        if(timer_vector[relay].size() <= timerNo)
            return false;
        timer_vector[relay].erase(timer_vector[relay].begin() + timerNo);
        return true;
    }

    relay_time* getTimer(int relay, int timerNo){
        if(timer_vector[relay].size() <= timerNo)
            ESP_LOGE("relay_time", "access out of range !!!");
        return &timer_vector[relay][timerNo];
    }

    int numOftimer(int relay){
        return timer_vector[relay].size();
    }


	// 0	1		0	0	:	0	0		0	0	:	0	0		A	L	L	
	// 0	2		0	0	:	0	0		0	0	:	0	0		N	O		
	// 0	3		0	0	:	0	0		0	0	:	0	0		C	U	S	M

    String print_Time(int relay, int timerNo){
        if(timer_vector[relay].size() <= timerNo)
            return "none";
        relay_time timer = timer_vector[relay][timerNo];
        String data = " " + FormatDigit(timerNo + USER_COUNT) + " " + FormatDigit(timer.time[0]) + ":" + 
                            FormatDigit(timer.time[1]) + " " + 
                            FormatDigit(timer.time[2]) + ":" + 
                            FormatDigit(timer.time[3]) + " ";
        //old with not usefull 
        // for(int i = 0; i<7;i++){
            // data+=(timer.date & (1<<i))?"y":"n";
        // }

        if(timer.date == 0){
            data+="NO ";
        }else if(timer.date == 0b01111111){ // 7 bit high 
            data+="ALL";
        }else {
            data+="CUS";
        }

        return data;
    }

    void setup() override {

    }
    
    void loop() override {
        if(activate)
            if((unsigned long long) update_time - millis() > UPDATE_TIMER_TIME){
                update_time = millis();


                for(int relay = 0; relay < MAX_RELAY; relay++)
                    for (auto timer : timer_vector[relay])
                    {
                    auto now_time = my_time->now();
                    //hour
                    //minute
                    //day of the week; sunday=1 [1-7] mon 
                    if(timer.activateDay(now_time.day_of_week - 1)){
                        int state = -1; // do notthing
                        //should have a better way here but ... nope im too lazy for mathmatic and timing stuff
                        if(timer.time[START_HOUR] == now_time.hour &&  timer.time[START_MINUTE] == now_time.minute)
                            state = 1; // on
                        if(timer.time[END_HOUR] == now_time.hour &&  timer.time[END_MINUTE] == now_time.minute)
                            state = 0; // on
                        if(state != -1)
                            changeRelayState(relay,state);
                        };
                    }

            }
            // This will be called very often after setup time.
            // think of it as the loop() call in Arduino
    }

}timer_mng;

#include "FS.h"
#include "SD.h"

#define SD_CS 5
// default pin:
// #define SCK_PIN 18
// #define MISO_PIN 19
// #define MOSI_PIN 23
#define READ_MODE 1
#define WRITE_MODE 0
class SDCard : public Component {
 private:
    bool SDcardLoad;
    int lastReadRelay = -1;
    File dataFile;

 public:

    /*------------------------===SD CARD===--------------------*/
    bool checkSDcard(){
        // u have to reopen the sd card to check if it out :|
        SD.end();
        SDcardLoad = SD.begin(SD_CS);
        return SDcardLoad;
    }

    bool openFile(int relay, bool ReadOrWrite, int trytoReInit = false){ 

    String fileName = "/relay_"+String(relay + USER_COUNT) + ".txt";
    if(ReadOrWrite){
        dataFile = SD.open(fileName.c_str(), FILE_READ);
        ESP_LOGD("Card",("ReadFile " + fileName).c_str());
    }else{
        // O_READ | O_WRITE | O_CREAT | O_TRUNC
        dataFile.close();
        // SD.remove(fileName.c_str()); //just ignore it (last version not work)
        dataFile = SD.open(fileName.c_str(), FILE_WRITE); // replace line also the file !!
        ESP_LOGD("Card",("WriteFile " + fileName).c_str());
    }
    if(dataFile){
        return true;
    }
    if(!trytoReInit){
        checkSDcard();
        //try again one more time
        return openFile(relay,ReadOrWrite,true);
    }
    return false;
    }

    // #read all the data in file and store it in ram
    bool readRelayTime(int relay){ 
         // data struction:   
         /*
         11:00 11:15 127\n
         11:00 11:15 127\n 
         11:00 11:15 127\n 
         */
        if(relay!=lastReadRelay){
        //open file
        if(!openFile(relay,READ_MODE))
            return false;
        //clear vector 
        timer_vector[relay].clear();
        //start read
        String data;

        // read file
        char r_data;
        data = "";
        while(dataFile.available()){
            r_data = dataFile.read();
            data+=r_data;
        };

        //convert data into vector 
        //in case trash in string
        while(data.length()>5){
        relay_time Time;
            for(int i=0;i<4;i++){
                Time.time[i] = data.substring(0,data.indexOf((i%2)==0 ? ':' : ' ')).toInt();
                data = data.substring(data.indexOf((i%2)==0 ? ':' : ' ')+1,data.length());
            }

            Time.date = data.substring(0,data.indexOf(' ')).toInt();
            data = data.substring(data.indexOf('\n')+1,data.length()); //ending the line
            
            timer_vector[relay].push_back(Time);

        }
        
        dataFile.close();
        }
        return true;
    }

    // write data
    bool writeRelayTime(int relay){
        if(!openFile(relay,WRITE_MODE))return false;
        for( int i = 0; i < timer_vector[relay].size(); i++){
            relay_time Time = timer_vector[relay][i];
            //because the data is fixed so u can...
            String data = FormatDigit(Time.time[0]) + ":" + 
                        FormatDigit(Time.time[1]) + " " + 
                        FormatDigit(Time.time[2]) + ":" + 
                        FormatDigit(Time.time[3]) + " " + String(Time.date) + "\n";
            dataFile.print(data);
        }
        dataFile.flush();
        dataFile.close();
        return true;
    }

    void setup() override {
        // SPI.pins(SCK_PIN, MISO_PIN, MOSI_PIN, SD_CS);
        // SD.begin(SD_CS);  
        SDcardLoad = SD.begin(SD_CS);
        if(!SDcardLoad) {
            ESP_LOGE("Card Mount", "Failed!");
            return;
        }else{
            ESP_LOGD("Card Mount", "success !");

        for(int j = 0; j < MAX_RELAY; j++){
            vector<relay_time> timers;
            timer_vector.push_back(timers);
            readRelayTime(j);
        }
    }


  }
//   void loop() override {
      //check sd card frequenly ?

    // SPI.beginTransaction(...)
    // If the data.txt file doesn't exist
    // Create a file on the SD card and write the data labels

    // SPI.write(0x42);
//   }
} SDCard_mng;