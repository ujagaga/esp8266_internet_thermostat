#ifndef CONFIG_H
#define CONFIG_H

#define RELAY_PIN                     (12) 
#define PUSHBTN_PIN                   (14) 
#define LED_PIN                       (16)             
#define DEVICE_NAME                   "WebTherm"
#define CHKIP_TIMEOUT                 (300000ul)      /* Check external IP every 5 minutes. */
#define AUTH                          "S0meL0ngSecur1tyKey"

/* Define 2 WiFi AP to connect to in case one is not available. 
This is usefull when developing at home and then using elsewhere. */
#define SSID_1                        "A_wifi_AP_name"
#define PASS_1                        "A_wifi_AP_pass"

#define SSID_2                        "Alternative_wifi_AP_name"
#define PASS_2                        "Alternative_wifi_AP_pass"

#endif
