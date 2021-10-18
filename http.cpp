#include "http.h"
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include "esp8266_internet_thermostat.h"
#include "DS18B20.h"
#include "config.h"


static bool updateStartedFlag = false;
static ESP8266WebServer webServer(80);

static const char html1[] = R"(
<!DOCTYPE HTML>
<html>
<head>
    <title>Web Switch</title>    
<style>
body{background-color: black; color: white;}
.contain{  width: 100%; min-height: 100vh;}
.center_div{margin:0 auto;max-width:400px;position:relative;}
.btn{width:80px;border:2px solid gray;margin:20px auto;text-align:center;border-radius:50%;}
.btn a{width: 80px;line-height: 80px;color: white;display:block;border-radius: 50%;text-decoration:none;}
#temp{width:50px;margin:30px auto;display:block;}
.glow{box-shadow: 0 0 20px white;}
</style>
</head>
<body>
<div class="center_div">      
)";

static const char html2[] = R"(
<select name="temp" id="temp" onchange='setTemp(this)'>
<option value="23">23</option>
<option value="24">24</option>
<option value="25">25</option>
<option value="26">26</option>
<option value="27">27</option>
<option value="28">28</option>
<option value="29">29</option>
</select>
<script>
function setTemp(selectObj){
var url = window.location.href;
var temp = selectObj.value;
if (temp == null) {temp = '';}
var pattern = new RegExp('\\b(temp=).*?(&|#|$)');
if (url.search(pattern)>=0){
  url=url.replace(pattern,'$1' + temp + '$2');
}else{
  url = url.replace(/[?#]$/,'');
  url = url + (url.indexOf('?')>0 ? '&' : '?') + 'temp=' + temp;
}
window.location.href = url;
} 
document.getElementById('temp').value = document.getElementById('targetTemp').value;
</script>
)";

static String redirectHtml = "<!DOCTYPE HTML><html><head><meta http-equiv=\"refresh\" content=\"5; url=/\"></head><body>"
"<h1>Starting update!</h1>"
"</body></html>";


bool checkClientLocalIP(){
  IPAddress clientIP = webServer.client().remoteIP();
  IPAddress localIp = WiFi.localIP();

  return ((localIp[0] == clientIP[0]) && (localIp[1] == clientIP[1]) && (localIp[2] == clientIP[2]));
}

void handleRoot() {   
  String auth = webServer.arg("auth");  
  
  if(checkClientLocalIP() || auth.equals(AUTH)){
    String state = webServer.arg("state");
    int targetTemp = webServer.arg("temp").toInt();      

    if((22 < targetTemp) && (targetTemp < 30)){
      DS_setTargetTemp(targetTemp);
    }
    String tempParam = "<input type='hidden' id='targetTemp' value='" + String(DS_getTargetTemp()) + "'>";

    int roundedTemp = DS_getTemp() + 0.5;
    
    if(!state.equals("on") && !state.equals("off")){
      if(MAIN_getState()){
        state = "on";
      }else{
        state = "off";
      }
    }
      
    String responseHtml = html1;

    String toggleState = "on";
    
    if(state.equals("on")){
      // Requested on state
      responseHtml += "<div class=\"btn glow\">";    
      MAIN_setState(true);
      toggleState = "off";
    }else{
      responseHtml += "<div class=\"btn\">";
      MAIN_setState(false);
    }
    
    responseHtml += "<a href=\"/?auth=" + auth + "&state=" + toggleState + "\">";    
    responseHtml += String(roundedTemp);
    responseHtml += "&#8451;</a></div>" + tempParam + String(html2) + "</div></body></html>";
    
    webServer.send(200, "text/html", responseHtml);  
  }
}

void startOtaUpdate() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname(DEVICE_NAME);  
   
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
  });
  ArduinoOTA.onEnd([]() {
    updateStartedFlag = false;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {

  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.print("Error: ");
    Serial.println(error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void initOta(){
  if(checkClientLocalIP()){
    updateStartedFlag = true;
    digitalWrite(RELAY_PIN, HIGH); 
    webServer.send(200, "text/html", redirectHtml);
    webServer.handleClient();
    startOtaUpdate(); 
  } 
}

bool HTTP_updateInProgress(){
  return updateStartedFlag;
}

void HTTP_init(){
  webServer.on("/", handleRoot);  
  webServer.on("/start_ota_update", initOta);
  webServer.onNotFound(handleRoot);
  webServer.begin(); 
}

void HTTP_process(){
  if(updateStartedFlag){
    digitalWrite(RELAY_PIN, HIGH);   
    ArduinoOTA.handle();
  }else{      
    webServer.handleClient(); 
  }
}
