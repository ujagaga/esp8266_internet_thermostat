#include <ESP8266WiFiMulti.h>
#include "esp8266_internet_thermostat.h"
#include "email.h"
#include "ip_check.h"
#include "http.h"
#include "DS18B20.h"
#include "config.h"


static uint32_t ipChkTimestamp = 0;
static String externIp = "";
static bool currentState = false;
static bool oldBtnPressedState = false;
static uint32_t btnProcessTimestamp = 0;

ESP8266WiFiMulti wifiMulti;


static void processPushButton(){
  // Software debounce
  if((millis() - btnProcessTimestamp) > 100){
    btnProcessTimestamp = millis();
    
    bool btnPressed = digitalRead(PUSHBTN_PIN) == 0;
  
    if(btnPressed){
      if(!oldBtnPressedState){
        // Button was just pressed. Toggle relay state;
        MAIN_setRelayOn(!currentState);
      }
    }
  
    oldBtnPressedState = btnPressed;
  }
}

static void checkIp(){
  String currentIp = IPCHK_get();

  currentIp.trim();

  if(!currentIp.equals(externIp)){
    String msg = "New Web Thermostat address: http://" + currentIp + "?auth=" + String(AUTH) + "\n\n";
    EMAIL_send(msg);
  }

  externIp = currentIp;
}

void MAIN_setState(bool state){
  currentState = state;
  if(!state){
    MAIN_setRelayOn(false);
  }
}

bool MAIN_getState(){
  return currentState;
}

void MAIN_setRelayOn(bool state){  
  if(state){
    digitalWrite(RELAY_PIN, LOW); 
    digitalWrite(LED_PIN, LOW); 
  }else{
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);  
  }
}



void setup() { 
  delay(50);
  Serial.begin(115200);  
  pinMode(RELAY_PIN, OUTPUT);  
  pinMode(PUSHBTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); 
  digitalWrite(LED_PIN, HIGH);    

 // WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(SSID_1, PASS_1);
  wifiMulti.addAP(SSID_2, PASS_2);
  
  Serial.print( "\n\nConnecting" );
  while ( wifiMulti.run() != WL_CONNECTED ) {
    delay ( 500 );
    ESP.wdtFeed();
    Serial.print ( "." );
  }
  String IP =  WiFi.localIP().toString();
  String wifi_statusMessage = "\nConnected to: " + WiFi.SSID() + String(". IP address: ") + IP;   
  Serial.println(wifi_statusMessage);  
  Serial.println(String("To trigger OTA update go to http://") +  IP + String("/start_ota_update"));
  Serial.println(String("Then go to your Arduino IDE and change device port to: ") + String(DEVICE_NAME));
  
  HTTP_init();
  DS_init();

  checkIp();
} 

void loop() {  

  HTTP_process();  
  
  if(HTTP_updateInProgress()){    
    digitalWrite(RELAY_PIN, HIGH); 
  }else{
    if((millis() - ipChkTimestamp) > CHKIP_TIMEOUT){
      ipChkTimestamp = millis();
      checkIp();
    }

    DS_process();  
    processPushButton();
  }   
} 
