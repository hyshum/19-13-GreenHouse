# EC464: Greenhouse Project, 2018-2019
A small-scale greenhouse that allows the user to view current sensor readings within the greenhouse from a web application and enables automatic heating if temperature values are too low. The device is intended to be used by casual gardeners who want to allow their plants to receive natural sunlight in the winter but whose plants will die or be severely damaged if left to completely withstand the elements alone. This device serves as an alternative to traditional greenhouses, which are often quite large and expensive to put together and are simply not realistic for casual gardeners. 

This project was created for EC463/464 Senior Design for Electrical and Computer Engineers at Boston University in 2018-2019.  
Customer: Ned Utzig, ned@utzig.com

#### Team Members:
Olivia Dorencz  
Laura Morgan  
Hok Yin Shum  
Kiahna Tucker  
Qian Zhang  

### Table of Contents
1. [Current Project Status](#status)  
  a. [Greenhouse Structure](#struct)  
  b. [Web Application](#app)  
2. [Usage](#usage)  
  a. [Reconfiguring Wifi](#wifi)  
    1. [Reconfiguring NodeMCU](#mcu)  
    2. [Reconfiguring Wemo Smart Plug](#smart)  
3. [Moving the Greenhouse](#move)
4. [NodeMCU Tips](#nodetips)
5. [AWS Lambda Tips](#lambda)
6. [Further Improvements](#imp)

<a name = "status"></a>
## Current Project Status 
The current project status consists of a working prototype which meets the following requirements:
<a name = "struct"></a>
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
<a name = "app"></a>
### Web Application
1. Displays all current sensor readings within the greenhouse
2. User can set ideal temperature range
3. Notifiations are sent to user if greenhouse is outside of ideal temperature range


These requirements have been met and the mechanisms for meeting them are described in more detail in [README_HARDWARE.md](Hardware/README_HARDWARE.md) and [README_SOFTWARE.md](Software/README_SOFTWARE.md) 

<a name = "usage"></a>
## Usage

<a name = "wifi"></a>
### Reconfiguring Wifi 
Wifi networks must be reconfigured to use this device with wifi settings that are not the ones that we have been using throughout this semester. Both the NodeMCU wifi board and the Wemo Smart Plug must be reconfigured.
<a name = "mcu"></a>
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
<a name = "smart"></a>
#### Reconfiguring Wemo Smart Plug
You must go through a different process to change the wifi network that the Wemo smart plug is connected to. This also must be a 2.4GHz network, as the smart plug is not able to communicate over a 5GHz network. You may be able to complete most of the setup steps using a 5GHz network, but the smart plug will fail to appear in connected devices after setup. Check beforehand to make sure you're using a 2.4GHz network, or you will likely end up wasting a lot of time.  

First, you must download the Wemo app onto your phone. It is available in both the Apple App Store and the Google Play Store. There is not currently a desktop compatible app for configuring your Wemo device at this time. (Maybe this will have changed).

Then you must reset your Wemo. To do this, hold down the power button while plugging the Wemo into an outlet. Continue to hold the button down until the light next to the power button begins to flash white rapidly. You can then stop holding down the button. The Wemo will then reset itself. Once the light flashes between white and orange, the device is ready to be set up.

I found it easiest to put my phone in Airplane mode and disable auto-reconnect on all Wifi networks while I was setting up the Wemo smart plug. This ensures that you correct your smart plug to the correct wifi network. Once you've downloaded the app, you should open the app. You can then click the settings icon in the upper right corner and select "Add a Wemo". You should then select "Wemo Smartplugs". Follow the instructions in the app to set up the smart plug. If the app seems to "freeze" on the wifi connection screen of the setup process, close the app, connect directly to the wifi network generated by your Wemo smart plug, and reopen the app. When you are asked to select your home network, make sure you select the correct network or you will need to reset your smart plug and your settings in the app. 

Your Wemo smart plug should now be connected to the Wifi network of your choosing!

<a name = "move"></a>
## Moving the Greenhouse 
The greenhouse is fairly easy to move. It is not very heavy, but we have added wheels to make it easier to transport. The greenhouse can only be rolled back and forth along one axis, so make sure to move it carefully. To date, we have not yet cemented the structure to the base of the greenhouse. This is because we discovered that it can be a tight fit getting the greenhouse through doorways. If you are moving the greenhouse, make sure that the doorways the greenhouse will need to pass through are large enough to accomodate it. At this time, the structure can still be removed in case we need to deal with any difficult to navigate doorways on ECE day. 

Unplug the greenhouse from any wall sockets before attempting to move the greenhouse. You should also attempt to hold the extension cord above the ground while moving the greenhouse to prevent any damage to the cord. Once you have moved the greenhouse to its new location, ensure that all electronics are still properly attached. This includes individual sensors, the serial connection between the NodeMCU and the Arduino, and the actual connections into the extension cord. Once this has all been verified, you can safely plug the greenhouse back in.
<a name = "sensors"></a>
## Hardware Tips
The DHT11 temperature and humidity sensors that we used are quite reliable, however they also can be damaged. The sensors have an upper temperature limit of approximately 140 degrees Fahrenheit. If subjected to temperatures higher than this, the sensor will likely be destroyed. Do not use a heat gun when testing these sensors, even if set to temperatures lower than 140 degrees Fahrenheit. Depending on the proximity to the heat gun, the temperature sensor can still be exposed to temperatures that will damage the sensor. If a temperature sensor is reporting a reading of 1766.0, it is likely that the sensor is either destroyed or has become disconnected, as this is what the calculated reading returns when nothing is attached to the digital input pin. The DHT11 sensors are also fairly slow to respond to changes in temperature, so if you heat a sensor using your hand or another safe heating device, do not be surprised if it takes a bit of time for the temperature to return to room temperature. This is inherit in the design of the temperature sensors, so isn't something you can accelerate by changing anything in the software.

Be careful when plugging and unplugging things from the extension cord. This seems like an obvious tip, but it can be hard to see into the enclosure when plugging things in. For safety, the extension cord should not be plugged into the wall when plugging things into the extension cord. We are including this tip for a reason. Please don't be like one of our teammates. 
<a name = "nodetips"></a>
## NodeMCU Tips
The NodeMCU and the libraries we used can be a little finnicky. One such instance is when reflashing the NodeMCU, make sure you remove the connection grounding the D3 pin before unplugging the device. Otherwise, the device will be reflashed the next time you plug it in and all of your uploaded code will be gone.

The ArduinoJSON library has some limitations on the amount of data that you are able to send. For this reason, we ended up compressing all of our data into a single string and then parsing the string in AWS Lambda. When we tried to package each element individually, we would frequently receive an error that we had run out of space in the JSON buffer, despite allocating more space than would have been necessary to store all of our data. If your NodeMCU suddenly disconncects from AWS IoT Core and cannot reconnect, this is likely the cause. 

Another issue is to make sure that you're verifying that the packets you receive in AWS Lambda are actual, legitimate packets. We ran into an issue where we would periodically send empty packets because the NodeMCU was attempting to read from the Arduino when the Arduino was not actually writing data, so the NodeMCU read all zeroes. We then added a check to our AWS Lambda code to make sure that we were not receiving packets with empty data. 
<a name = "aws"></a>
## AWS Lambda Tips
We used AWS Lambda to extract the readings from AWS IoT Core, send them to AWS DynamoDB, and trigger any other actions like activating the heater or sending an email notification. Within AWS Lambda, we send HTTPS requests using the requests library of Node.js. Using outside libraries in AWS Lambda can be a little bit confusing if you're using the online text editor. We would recommend not using the online text editor and actually just editing all your files locally. You can then compress everything into a zip archive and upload it to AWS Lambda. One complication with this is if you have a Mac, the compression tool will automatically add another directory at the top level of the compressed archive. One way around this is to simply have somebody who does not have a Mac compress the archive and upload it. That is largely what we did throughout the course of this project. There are likely other workarounds to this issue, but would have to be discovered by someone who uses a Mac. 

To use our AWS Lambda code as-is, zip the contents of [this directory](Software/Lambda_smartplug) and upload to AWS Lambda. This will preserve all of our functionality, but the "exports.js" file within that directory must be modified if you would like to add functionality to this.

<a name = "imp"></a>
## Further Improvements
Our customer gave us a few optional suggestions of ways that we could further improve our device, but that we ultimately decided were out of the scope of what we wanted to complete during the course of this project. 

The first was using some sort of alternate power supply. Our current device, must be plugged into a wall to operate. Our customer suggested that it may be interesting to explore whether solar power or a backup battery could also be used. Solar power would ideally replace the entire current power supply, making the device low cost to use and with less of an environmental footprint. A backup battery might be ideal for if power to the greenhouse is suddenly lost. During a power outage, this could continue to maintain a suitable temperature for the plants.

Another suggestion was to use cameras and computer vision to monitor the growth of the plants. Cameras could periodically capture images of the plants and compare the images with what expected plant growth should look like to see whether the plants are actually growing well.

A third suggestion was to also implement an automatic watering functionality. Currently, only the temperature is maintained automatically. However, the project could be extended to automatically water certain plants if their soil moisture is detected to be below a certain level. This would be fairly straightforward to implement, as the readings are already collected, they would just then need to trigger another function if the readings were too low. We decided not to pursue this as it was not a priority for our customer. 
