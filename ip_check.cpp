#include <ESP8266HTTPClient.h>

#define CHKIP_URL_1     "http://icanhazip.com"
#define CHKIP_URL_2     "http://checkip.amazonaws.com"


String IPCHK_get(){
  HTTPClient http; 

  Serial.print("Trying to get IP from:");
  Serial.println(CHKIP_URL_1);  
  
  http.begin(CHKIP_URL_1); 
  int httpCode = http.GET();
  String payload = "";

  if (httpCode > 0) {
    payload = http.getString();
    Serial.println(payload);  
  }else{
    Serial.print("Failed. Trying to get IP from:");
    Serial.println(CHKIP_URL_2);  

    http.end();
    http.begin(CHKIP_URL_2); 
    httpCode = http.GET();

    if (httpCode > 0){
        payload = http.getString();   
        Serial.println(payload);  
    }else{
        Serial.println("Failed!");  
    }
  }

  http.end();   //Close connection

  return payload;
}
