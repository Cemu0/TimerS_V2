#pragma once
#include "esphome.h"

#include <vector>
using namespace std;

//base on dimension
#define SCREEN_COLLUMS 20
#define SCREEN_ROUND 4
// #define VIRUAL_SCREEN_UPDATE_TIME 100

class virtualScreen : public Component
{
private:
    //this not good but good enoughs :)
    String old_line[4];
    String line[4];

    unsigned long timer = 0;
    
    bool update(){
        updated = false;
        if(old_line[0] != line[0]){
            id(line1).publish_state(line[0].c_str());
            old_line[0] = line[0];
            updated = true;
        }
        if(old_line[1] != line[1]){
            id(line2).publish_state(line[1].c_str());
            old_line[1] = line[1];
            updated = true;
        }
        if(old_line[2] != line[2]){
            id(line3).publish_state(line[2].c_str());
            old_line[2] = line[2];
            updated = true;
        }
        if(old_line[3] != line[3]){
            id(line4).publish_state(line[3].c_str());
            old_line[3] = line[3];
            updated = true;
        }
        return updated;
    }

public:
    bool updated = false;

    void print(uint8_t column, uint8_t row, const char *str){
        //clear data 
        if(row > 3)
            return;

        int j = 0;
        for(uint8_t i = column ; i < 21; ++i){
            if(str[j] == '\0')
                break;
            line[row][i] = str[j];
            ++j;
        }
    }

    void updateScreen(){
        if(update()){
            for(uint8_t i = 0 ; i < 4; ++i)
                line[i] = "                     ";
            
        }
    }
    void setup() override {
        for(uint8_t i = 0 ; i < 4; ++i)
            line[i] = "                     ";
    }

    void loop() override {
        // if((unsigned long) millis() - timer > VIRUAL_SCREEN_UPDATE_TIME){

        //     timer = millis();
        // }
    }


}virtual_screen;

void dualPrint(uint8_t column, uint8_t row, const char *str){
    my_display->print(column,row,str);
    virtual_screen.print(column,row,str);
}

//baseClass for inheritent 
class screen {
public:
    int _NowChose = 0; //save the previous stage
    virtual ~screen(){}
    virtual int name() = 0;
    //when button is trigered
    // 0: exit
    // orther: enter orther menu :v
    virtual int buttonCallBack() = 0;
    //to print out the screen 
    virtual void render() = 0;
};

//menus
enum MenuType{
    IGNORE = -1,
    RETURN = 0,
        MAIN_MENU,
            //MAIN_MENU
            HOME_SCREEN,
            RELAY_MENU,
                CHOSE_TIMER_MENU,
                    EDIT_TIMER_SCREEN,
            MANUAL_CONTROL_SCREEN,
            SYSTEM_REPORT_SCREEN,
};

// int lastMax = 0;
void limitScroll(int max){
    // limit the scroll
    // if(max != lastMax)
    // {
    //     lastMax = max;
    //     id(encoder).set_min_value(0);
    //     id(encoder).set_max_value(max-USER_COUNT);
    // }
    //bug ??????? -> cant use in menu
    if(id(encoder).state<0){id(encoder).set_value(0);};
    if(id(encoder).state>max-USER_COUNT){id(encoder).set_value(max-USER_COUNT);}
}

//for some weird bug 
bool lastEntry = false;

class menu : public screen {
private:
    //variable for scolling system
    int _NowShow = 0;
    // int Editting_ = 0;
    int NowValue_ = 0;
    int _max_lines;
    //function for render the menu
    
public:
    // menu(){
    //     if(id(encoder).state != _NowChose)
    //         id(encoder).set_value(_NowChose);
    // }
    //soooooo wrogn with this =))) but it just done it job so well
    template<class T>
    // void MenuShow(const char menu[][SCREEN_COLLUMS], int max_lines){
    // void MenuShow(char** menu, int max_lines){
    void MenuShow(T menu, int max_lines){

            //not work after boot
            limitScroll(max_lines);

            //fix some stupit bug
            if(!lastEntry){
                    id(encoder).set_value(0);
                    delay(100);
                    if(id(encoder).state == 0)
                        lastEntry = true;
            }

            // get the scroll
            _NowChose = id(encoder).state;
            if(_NowChose<0){_NowChose = 0; id(encoder).set_value(_NowChose);};
            if(_NowChose>max_lines-1){_NowChose = max_lines-1; id(encoder).set_value(_NowChose);}
        
            //go down
            if(_NowChose > _NowShow+(SCREEN_ROUND-1)){ //_NowShow+=1; 
                _NowShow = _NowChose - (SCREEN_ROUND-1);
            }else if(_NowChose < _NowShow){ // go up
                _NowShow = _NowChose;   
            }

        for(int i = 0;i<SCREEN_ROUND;i++){
            if(_NowChose == 0 && i==0)
                dualPrint(0,i,"<");
            else if(i+_NowShow==_NowChose)
                dualPrint(0,i,">"); 
            else dualPrint(0,i," ");//
            dualPrint(1,i,menu[_NowShow+i]);
        }
    }
};

class main_menu : public menu {
private:
    //main menu
    #define MAIN_MENU_MAX_LN 4
    const char PROGMEM main_menu_array[MAIN_MENU_MAX_LN][SCREEN_COLLUMS] = {
        "<Home Screen",
        " Set Relay Timer",
        " Manual Control",
        " System Report",
    };
public: 
    int name(){
        return MAIN_MENU;
    }
    int buttonCallBack(){
        // int _NowChose = id(encoder).state;
        switch (_NowChose)
        {
            case 0:
                return RETURN;
            case 1:
                return RELAY_MENU;
            case 2:
                return MANUAL_CONTROL_SCREEN;
            case 3:
                return SYSTEM_REPORT_SCREEN;
        }
        return IGNORE; // never reach
    }
    void render(){
        MenuShow(main_menu_array,MAIN_MENU_MAX_LN);
        
    }
};

//home screen
class home_screen : public screen {
public: 
    int name(){
        return HOME_SCREEN;
    }
    int buttonCallBack(){
        return MAIN_MENU;
    }
    void render(){
        //skip this line
        // my_display->printf(0,0,"    Date:");
        my_display->strftime(0,0,"Time: %H:%M:%S", my_time->now());
        my_display->strftime(0,1,"Date: %d-%m-%Y", my_time->now());
        // my_display->printf(0,1,"%8.0f", encoder->state);

        // my_display->strftime(0,1,"%H:%M:%S %d-%m-%Y", my_time->now()); #in case you need more screen to do sth else


        //Wifi
        dualPrint(0,2,"WIFI:");
        dualPrint(5,2,swifi->is_connected()?  (String((int)wfsig->state)+"dB").c_str():String("NO").c_str());

        //MQTT
        dualPrint(12,2,"MQTT:");
        dualPrint(17,2,smqtt->is_connected()?"YES":"NO");

        //max display: 9 , +2 to center the text
        for(int i = 0; i < 8; ++i)
            dualPrint(i*2 + 2,3,timer_mng.checkRelayStage(i)? (String(i+1)+" ").c_str() : "_ ");

        //test
        // my_display->printf(0,3,"%s",String(millis()).c_str());
        // // it.printf(0,3,"%8.0f", id(touchsensor).state);
        // my_display->printf(8,3,"%s",String(ESP.getFreeHeap()).c_str());
        
    }
};

class relay_menu : public menu {
private: 
    //                                  MAX_RELAY+1 but i want to extent
    const char PROGMEM relay_menu_array[17][SCREEN_COLLUMS] = {
    "<Main Menu",
    " relay 1",
    " relay 2",
    " relay 3",
    " relay 4",
    " relay 5",
    " relay 6",
    " relay 7",
    " relay 8",
    " relay 9",
    " relay 10",
    " relay 11",
    " relay 12",
    " relay 13",
    " relay 14",
    " relay 15",
    " relay 16",
    // and more ... , we free a bunch of ram 
    };

public: 
    int name(){
        return RELAY_MENU;
    }
    int buttonCallBack(){
        // int _NowChose = id(encoder).state;
        //to do code here
        switch (_NowChose)
        {
            case 0:
                return RETURN;
            default:
                return CHOSE_TIMER_MENU;
        }
        return RETURN;
    }
    void render(){
        MenuShow(relay_menu_array,MAX_RELAY+1);
        
    }
};

class timer_menu : public menu {
private:
    // char** timers;
    bool needReload = false;
    int numberOfTimers;
    int _relay_number;
    // (no need)note remember to delete after use
    // char* toCstr(String data){
    //     char * cstr = new char [data.length()+1];
    //     strcpy(cstr, data.c_str());
    //     return cstr;
    // }
    class rendering_class{
        private:
            int numberOfTimers; //well free ram xD
            int _relay_number;
            char tmp_char[21];

        public:
            void setRL(int r, int numberOfTimers){
                this->numberOfTimers = numberOfTimers;
                _relay_number = r;
            }
            const char* operator[](int number){
                    if(number == 0){
                        // return (String("relay ") + String(_relay_number)).c_str();
                        strcpy(tmp_char,("<relay " + String(_relay_number + USER_COUNT)).c_str());
                        return tmp_char;
                    }

                    if(number == numberOfTimers - 1){
                        return " + add new timer";
                        //add a new timer
                    }
                    return timer_mng.print_Time(_relay_number,number - USER_COUNT).c_str();
            }
    }rc;

    void reload(){
        numberOfTimers = timer_mng.numOftimer(_relay_number) + 2; //for the title and 
        rc.setRL(_relay_number,numberOfTimers);
    }

public: 

    timer_menu(int relay_number){
        _relay_number = relay_number;
        numberOfTimers = timer_mng.numOftimer(relay_number) + 2; //for the title and 
        rc.setRL(relay_number, numberOfTimers);
    }

    ~timer_menu(){
        SDCard_mng.writeRelayTime(_relay_number);
    }
    int name(){
        return CHOSE_TIMER_MENU;
    }
    int buttonCallBack(){
        switch (_NowChose)
        {
            case 0:
                return RETURN;
            default:
                needReload = true;
                return EDIT_TIMER_SCREEN;
        }
        return -1;
    }
    void render(){
        if(needReload){
            needReload = false;
            reload();
        }
            
        MenuShow(rc,numberOfTimers);
        
    }
};

//edit timer screen
class edit_timer_screen : public screen{
private:
    int _relay;
    int _timerNo;
    //equal 0 if not edit if edit = last _NowChose
    int editing = 0;
    relay_time* relayTimer;
    // int lastMax = 0;
public: 
    edit_timer_screen(int relay, int timerNo){
        timer_mng.activate = false;
        _relay = relay;
        _timerNo = timerNo;
        if(timerNo >= timer_mng.numOftimer(relay))
        {
            ESP_LOGD("relay_time", "add new timer");
            relayTimer = timer_mng.addTimer(relay);
        }
        else
            relayTimer = timer_mng.getTimer(relay,timerNo);
    }

    ~edit_timer_screen(){
        timer_mng.activate = true;
    }

    int name(){
        return EDIT_TIMER_SCREEN;
    }

    int buttonCallBack(){
        if(editing == 0){
            if(_NowChose == 0)
                return RETURN;
            
            //delete
            if(_NowChose == 10){
                ESP_LOGD("relay_time", "remove timer");
                timer_mng.removeTimer(_relay,_timerNo);
                return RETURN;
            }

            if(_NowChose < 3){
                if(_NowChose == 1){
                    id(encoder).set_value(relayTimer->time[0]);
                    editing = 1;
                }
                if(_NowChose == 2){
                    id(encoder).set_value(relayTimer->time[2]);
                    editing = 3;
                }
            }
            else{
                //toggle bit
                relayTimer->date ^= 1UL << (_NowChose - 3);
            }
        }   
        else{
            if(editing == 1){
                id(encoder).set_value(relayTimer->time[1]);
                editing = 2;
            }

            else if(editing == 3){
                id(encoder).set_value(relayTimer->time[3]);
                editing = 4;
            }
            else { //logic xD
                if(editing == 2)
                    _NowChose = 1;
                
                if(editing == 4)
                    _NowChose = 2;

                id(encoder).set_value(_NowChose);
                editing = 0;
            }
        }
        return IGNORE;
    }

    bool checkBlink(){
            if((millis() / 500)%2)
                return true;
            return false;
    }

    void render(){

        //render the whole screen 
        dualPrint(0,0,(" <Timer " +  String(_timerNo+USER_COUNT) + " R " + String(_relay+USER_COUNT) + "   DEL").c_str() );
        String data = " STR " + FormatDigit(relayTimer->time[0]) + ":" + 
                                FormatDigit(relayTimer->time[1]) + " " + 
                       "END " + FormatDigit(relayTimer->time[2]) + ":" + 
                                FormatDigit(relayTimer->time[3]) + " ";
        dualPrint(0,1,data.c_str());
        dualPrint(0,2,"Su Mo Tu We Th Fr Sa");

        data = " ";
        for(int i = 0; i<7;i++){
            data+=(relayTimer->date & (1<<i))?"y":"n";
            if(i<6)data+="  ";
        }
        dualPrint(0,3,data.c_str());
        
        //scroll thought each element ()
        //   1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20 x
        //0  <	T	i	m	e	r	 	0	1	    R       0   1           >   D   E   L 								
        //1  >	S	T	R		0	0	:	0	0	>	E	N	D		0	0	:	0	0
        //2  M	o		T	u	 	W	e		T	h		F	r		S	a		S	u
        //3  >	n		>	n		>	n		>	n		>	n		>	n		>	n
        //y  >: 10 + <: 1 = 11
        
        _NowChose = id(encoder).state;
        
        int editPos = _NowChose;

        char simbol[2] = ">";

        switch (editing)
        {
        case 1:
            if(checkBlink())
                dualPrint(4,1,simbol);
            limitScroll(24);
            editPos = 1;
            break;

        case 2:
            if(checkBlink())
                dualPrint(7,1,simbol);
            limitScroll(60);
            editPos = 1;
            break;
        
        case 3:
            if(checkBlink())
                dualPrint(14,1,simbol);
            limitScroll(24);
            editPos = 2;
            break;
        
        case 4:
            if(checkBlink())
                dualPrint(17,1,simbol);
            limitScroll(60);
            editPos = 2;
            break;

        default: // = 0
            limitScroll(11);

            int x = 0;
            int y = 0;
            switch (editPos)
            {
                case 0: //0,0
                    strcpy(simbol,"<");
                    break;
                case 1:
                    x = 0, y = 1;
                    break;
                case 2:
                    x = 10, y = 1;
                    break;
                case 10:
                    x = 15, y = 0;
                    break;

                default:
                    x = (_NowChose-3)*3; y = 3;
                    break;
            }
            dualPrint(x,y,simbol);

            break;
        }

        //update edited element
        if(editing > 0 && editing < 5)
            relayTimer->time[editing - 1] = _NowChose;

        

    }
};

//Manual Control screen
class manual_control_screen : public menu {
private:

    const int maxline = MAX_RELAY + 1;

    class rendering_class{
        private:
            char tmp_char[21];

        public:
            const char* operator[](int number){
                switch (number)
                {
                case 0:
                    return "< Main menu";
                    break;
                default:
                    strcpy(tmp_char,(" relay " + FormatDigit(number) + " " + (timer_mng.checkRelayStage(number - 1)? "ON" : "OFF")).c_str());
                    break;
                }
                return tmp_char;
            }
    }rc;
public: 
    int name(){
        return MANUAL_CONTROL_SCREEN;
    }
    int buttonCallBack(){
        if(_NowChose == 0)
            return RETURN;
        else
        {
            timer_mng.toggleRelayState(_NowChose - USER_COUNT);
        }
        return IGNORE;
    }
    void render(){
        MenuShow(rc,maxline);
        
    }
};

//change this if you want to report more data
//System Report screen
class system_report_screen : public menu {
private:
    //change this number arcoding to the number of lines in the switch
    const int maxline = 6;
    class rendering_class{
        private:
            char tmp_char[21];

        public:
            const char* operator[](int number){
                switch (number)
                {
                case 0:
                    return "< Main menu";
                    break;
                case 1:
                    strcpy(tmp_char,(" last boot:" + String(millis()/1000) + "s").c_str());
                    break;
                case 2:
                    strcpy(tmp_char,(" free ram :" + String(ESP.getFreeHeap())).c_str());
                    break;
                case 3:
                    strcpy(tmp_char,(" temperature :" + String(id(dthsensor1tmp).state)).c_str());
                    break;
                case 4:
                    strcpy(tmp_char,(" humidity :" + String(id(dthsensor1hmd).state)).c_str());
                    break;
                case 5:
                    strcpy(tmp_char,String("TimersV2 by Cemu0").c_str());
                    break;
                default:
                    strcpy(tmp_char,"");
                    break;
                }
                return tmp_char;
            }
    }rc;
public: 
    int name(){
        return SYSTEM_REPORT_SCREEN;
    }
    int buttonCallBack(){
        return RETURN;
    }
    void render(){
        MenuShow(rc,maxline);
        
    }
};


// template screen for you to add more menu
// class system_report_screen : public screen {
// public: 
//     int name(){
//         return MANUAL_CONTROL_SCREEN;
//     }
//     int buttonCallBack(){
//         return IGNORE;
//     }
//     void render(){
//     }
// };