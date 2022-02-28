#pragma once
#include "esphome.h"
#include <vector>

// i know this shouldn't be used 
using namespace std;

#define RETURN_TITLE 1
#define MAX_SCREEN_UPDATE_TIME 100

//main UI control class
class UI {
private:
    vector<screen*> screens;
    unsigned long timer = 0;
    unsigned long update_interval = 1000;
public:
    void buttonCallBack(){

        int callBack = screens.back()->buttonCallBack();
        ESP_LOGD("button call back", String(callBack).c_str());
        if(callBack == IGNORE){
            return;
        }
        if(callBack == RETURN){
            //check if not the home screen 
            if(screens.size() > 1){
                //set the previous chosen
                id(encoder).set_value(screens[screens.size() - 2]->_NowChose);
                delete screens.back();
                screens.pop_back();
            }
        }
        else {
            screen* p;
            id(encoder).set_value(0);

            // note we just go forward (enter orther screen)
            if(screens.back()->name() == HOME_SCREEN)
                if(callBack == MAIN_MENU)
                    p = new main_menu();

            if(screens.back()->name() == MAIN_MENU){
                if(callBack == RELAY_MENU)
                    p = new relay_menu();

                if(callBack == MANUAL_CONTROL_SCREEN)
                    p = new manual_control_screen();

                if(callBack == SYSTEM_REPORT_SCREEN)
                    p = new system_report_screen();
            }

            if(screens.back()->name() == RELAY_MENU){
                //the last screen contain the relay number chosen
                if(callBack == CHOSE_TIMER_MENU)
                    p = new timer_menu(screens.back()->_NowChose - RETURN_TITLE); 
            }

            if(screens.back()->name() == CHOSE_TIMER_MENU){
                int timerNo = screens[screens.size()-1]->_NowChose - RETURN_TITLE;
                int relay = screens[screens.size()-2]->_NowChose - RETURN_TITLE;
                if(callBack == EDIT_TIMER_SCREEN)
                    p = new edit_timer_screen(relay,timerNo); 
            }

            ESP_LOGD("enter", String(p->name()).c_str());
            screens.push_back(p);
            // p->_NowChose = -1;
        }
        ESP_LOGD("screenVectorSize", String(screens.size()).c_str());
    }
    //init
    UI(){
        screen* p = new home_screen();
        screens.push_back(p);
    }
    ~UI(){
        for(auto i : screens)
            delete i;
    }
    //render
    void render(){

        if(((unsigned long) millis() - timer > MAX_SCREEN_UPDATE_TIME)){
            screens.back()->render();
            virtual_screen.updateScreen();
            // virtual_screen.updated = false;
            if(screens.size() > 1 && screens.back()->name() != MANUAL_CONTROL_SCREEN) {
                if(update_interval==1000){
                    update_interval=100;
                     id(my_display).set_update_interval(update_interval);
                     id(my_display).call_setup();
                }
            } else {
                if(update_interval==100){
                    update_interval = 1000;
                    id(my_display).set_update_interval(update_interval);
                    id(my_display).call_setup();
                    
                }
            }
        }

        //screen light
        if (id(screenlight).state != id(screen_on)){
            //wait for buff or the screen will print mess
            delay(1);

            id(screen_on) = id(screenlight).state;
              if(id(screen_on))
                    my_display->backlight();
              else{
                    my_display->no_backlight();

                    //auto return to home screen
                    while(screens.size() > 1){
                        //set the previous chosen
                        id(encoder).set_value(screens[screens.size() - 2]->_NowChose);
                        delete screens.back();
                        screens.pop_back();
                    }
              }
                    
        }
    }

} UIsys;