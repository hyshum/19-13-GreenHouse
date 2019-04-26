# EC464: Greenhouse Project, 2018-2019
A small-scale greenhouse that allows the user to view current sensor readings within the greenhouse from a web application and enables automatic heating if temperature values are too low. The device is intended to be used by casual gardeners who want to allow their plants to receive natural sunlight in the winter but whose plants will die or be severely damaged if left to completely withstand the elements alone. This device serves as an alternative to traditional greenhouses, which are often quite large and expensive to put together and are simply not realistic for casual gardeners. 

This project was created for EC463/464 Senior Design for Electrical and Computer Engineers at Boston University in 2018-2019.

#### Team Members:
Olivia Dorencz  
Laura Morgan  
Hok Yin Shum  
Kiahna Tucker  
Qian Zhang  

## Current Project Status
The current project status consists of a working prototype which meets the following requirements:

### Greenhouse Structure
1. Enclosure for a plant no larger than 2m x 1m x 1m
2. Walls should be transparent to allow light in
3. Can be opened easily for maintenance and watering
4. Plants can be removed from greenhouse if desired without assistance
5. Transportable by an adult
6. Stable against wind of 39-46 mph
7. Able to withstand rain and snow without damage to plants or electronics
8. Automatically maintains a suitable range of temperature during New England winter. Heating must be automatic, but any need for cooling can be performed manually by use. Ideal temperature range can be set by the user via the web application
9. Has sensors mesasuring temperature, air humidity, soil moisture, and power consumption
10. Sensor data is sent to web application over user's WiFi network
11. Works using one standard North American wall outlet (110-120V AC)

### Web Application
1. Displays all current sensor readings within the greenhouse
2. User can set ideal temperature range
3. Notifiations are sent to user if greenhouse is outside of ideal temperature range


These requirements have been met and the mechanisms for meeting them are described in more detail in [README_HARDWARE.md](Hardware/README_HARDWARE.md) and [README_SOFTWARE.md](Software/README_SOFTWARE.md) 

## Usage

### Reconfiguring Wifi
Wifi networks must be reconfigured to use this device with wifi settings that are not the ones that we have been using throughout this semester. Both the NodeMCU wifi board and the Wemo Smart Plug must be reconfigured.
#### Reconfiguring NodeMCU
To change the settings on the NodeMCU, you will need to unplug it from the extension cord and plug it into your laptop. You should download the Arduino IDE in order to flash the device. Disconnect the serial TX and RX pins from the TX and RX pins of the Arduino. Connect a wire from the D3 pin on the NodeMCU to its own ground. This enables the board to be flashed.  
Download the nodemcu code [here](Software/arduino/test_wifi/node_mcu).  
Open node_mcu.ino in the Arduino IDE.  
If secrets.h does not open in an additional tab, manually open that file using your text editor of choice.  
Change the values of ssid and password in secrets.h to match the Wifi network you wish to use.  
You will also need to generate your own AWS IOT keys. The process for doing that is described in [this tutorial](https://github.com/debsahu/ESP-MQTT-AWS-IoT-Core/blob/master/doc/README.md).  
Save everything and compile before attempting to upload to the board. If you are missing any libraries, you will need to install them. All libraries used are linked below:

[ESP8266Wifi](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi)  
[ArduinoJson](https://github.com/bblanchon/ArduinoJson)  
[WiFiClientSecure](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFiClientSecure)  
[MQTT](https://github.com/256dpi/arduino-mqtt)  

You may also need to install the ESP8266 board library if you have not previously used an ESP8266 device. This can be done within the Arduino IDE by going to Tools > Board > Boards Manager, searching for "esp8266" and installing the first result that appears.

When you are ready to upload your code to the NodeMCU, select "NodeMCU 0.9 (ESP-12 Module)" under Tools > Board. Also set the baud rate to 11520 under Tools > Upload Speed. You should now be able to upload your code.

After uploading your code, unplug your NodeMCU from your computer and reconnect it to the extension cord in the greenhouse. You can then re-attach the serial pins of the NodeMCU to the serial pins of the Arduino. Remember: TX on one connects to RX on the other. Your board should now be able to connect to the new wifi network you provided.

#### Reconfiguring Wemo Smart Plug
You must go through a different process to change the wifi network that the Wemo smart plug is connected to. This also must be a 2.4GHz network, as the smart plug is not able to communicate over a 5GHz network. You may be able to complete most of the setup steps using a 5GHz network, but the smart plug will fail to appear in connected devices after setup. Check beforehand to make sure you're using a 2.4GHz network, or you will likely end up wasting a lot of time.  

First, you must download the Wemo app onto your phone. It is available in both the Apple App Store and the Google Play Store. There is not currently a desktop compatible app for configuring your Wemo device at this time. (Maybe this will have changed).

Then you must reset your Wemo. To do this, hold down the power button while plugging the Wemo into an outlet. Continue to hold the button down until the light next to the power button begins to flash white rapidly. You can then stop holding down the button. The Wemo will then reset itself. Once the light flashes between white and orange, the device is ready to be set up.

I found it easiest to put my phone in Airplane mode and disable auto-reconnect on all Wifi networks while I was setting up the Wemo smart plug. This ensures that you correct your smart plug to the correct wifi network. Once you've downloaded the app, you should open the app. You can then click the settings icon in the upper right corner and select "Add a Wemo". You should then select "Wemo Smartplugs". Follow the instructions in the app to set up the smart plug. If the app seems to "freeze" on the wifi connection screen of the setup process, close the app, connect directly to the wifi network generated by your Wemo smart plug, and reopen the app. When you are asked to select your home network, make sure you select the correct network or you will need to reset your smart plug and your settings in the app. 

Your Wemo smart plug should now be connected to the Wifi network of your choosing!
