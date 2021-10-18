# Web triggered switch

ESP8266 based thermostat with on/off switch.

The intention is to controll a relay, based on current ambient temperature, via web interface from outside local network. The device is behind a router with a public IP address and the port 80 is passed to the device.
Also to prevent device from changing the IP, the router should also bind the IP to devices MAC. An alternative is to set a fixed IP address, but that is an easy fix not included here.
Since the router can be restarted at any time and IP address can change, there is a mechanism implemented to periodically check the external IP address and send an email when it changes.
This way I can always keep track of current IP address. To prevent unauthorized access, the device expects an additional key within the get request like:


		http://<router_ip>?auth=S0meL0ngSecur1tyKey
		
		
If the correct key is not supplied, the device will not even respond, so any scanning from the internet will not return a response, thus preventing detection in case of any hackers attempt to access.
Accessing on a local network does not require the security key.

	
		http://<local_device_ip>
		

As this is intended to controll a heater, there is also a DS18B20 probe connected to measure the current room temperature, which is displayed on the web UI.
For a more convenient On/Off on premises, I added a physical pushbutton as well.

Besides on/off function, the web UI provides an option to select desired temperature and it is saved in EEPROM to persist through restart.

## Dependencies

Before compiling in Arduino IDE, make sure you have installed the following libraries:
- DallasTemperature
- OneWire
- ESP_EEPROM
- EMailSender
- ArduinoOTA

Checkout the "config.h" and adjust parameters to your needs.

## Contact

email: ujagaga@gmail.com
