# Software Report
## Table of Content
1. [Overview of software module](#Overview_of_software_module)<br /> 
    a. [Arduino and NodeMCU Embedded System](#Arduino_and_NodeMCU_Embedded_System)<br /> 
    b. [Lambda](#Lambda)<br /> 
    c. [Web Application](#Web_Application)<br /> 
2. [Flow chart](#Flow_chart) 
3. [Dev/build tool information](#Dev_build_tool_information)
4. [Install the project software stack](#Install_the_project_software_stack)  
    a. [Installation of webapp on local machine](#Installation_of_webapp_on_local_machine)<br /> 
    b. [Command to run the server](#Command_to_run_the_server)<br /> 
    c. [Web Configuring Wemo_Smart Plug with WLAN](#Configuring_Wemo_Smart_Plug_with_WLAN)<br /> 
    
    
    
<a name = "Overview_of_software_model"></a>
## Overview of software module

<a name = "Arduino_and_NodeMCU_Embedded_System"></a>
### Arduino and NodeMCU Embedded System:

Two microcontrollers are used to read current measurements and send these measurements to the DynamoDB database over a wifi connection. The first microcontroller is an Arduino Genuino Uno. All sensors are directly connected to the Arduino using digital and analog connections depending on the sensors requirements. The Arduino packs all of these readings into a single string which it sends to the NodeMCU over a Serial UART connection every 5 seconds. 

Upon setup, the NodeMCU connects to the provided wifi network. It then attempts to connect to AWS IoT Core using MQTT. If either of these connections fails, the board will attempt to reconnect until successful. The NodeMCU receives the string from the Arduino, parses it, and then sends the measurements to AWS IoT Core over the user’s wifi connection. The Arduino does not at any point read data sent by the NodeMCU, though the NodeMCU does print some information over a Serial connection for debugging purposes. 

If the NodeMCU disconnects from AWS or the user’s wifi at any point, it will attempt to reconnect and continue operation. To detect and avoid of sending empty data package, an constant variable is sent with all other sensor data, and its value is always one. 

<a name = "Lambda"></a>
### Lambda:
The central system is written in NodeJS and is located on Lambda supported by AWS, where the code can run without provisioning or managing servers. The central system integrates all virtual funcalities except backend of the web application, and most of them are supported by AWS as well. Application built in Lambd is responsive to events and new information. For our system, the application is responding to events triggered by AWS IOT. We linked AWS IOT to our greenhouse by registering Nodemcu in IOT. The communication between Nodemcu and IOT is through MQTT, the publish-subscribe-based messaging protocol. 

When AWS IOT is triggered by the MQTT request, it transfers the JSON package to AWS Lambda and triggers the central system. The central system in Lambda firsts check if the value of constant variable is equal to one. If its value is zero, this data package is invalid and rejected. If its value is one, this data package is valid and accepted. Next, the central system reads the current date and time and adds the timestamp into the data package, so every set of sensor data includes the timestamp. Wth the timestamp, the central system adds the data package inside the database. The database linked to Lambda is DynamoDB, the fully managed proprietary NoSQL database service that offered by Amazon as part of the AWS portfolio. DynamoDB does not only store the sensor data package, but also the parameters. Some of the parameters are only for running the system and stored internally, while some others are set by our client, including the lowest temperature in the greenhouse that our customers desired. Lambda reads the lowest temperature parameter stored in DynamoDB, and compares the temperature inside the greenhouse. If the lowest desired temperature is lower than temperature inside the greenhouse, Lambda sends an alarm to our client’s email address via Simple Notification Service (SNS). SNS is notification service supported by Amazon and our client’s email address is pre-subscribed to SNS. Lambda customizes the message and triggers the SNS, which sends the message to our client’s email address. 

Simultaneously when SNS is triggered, Lambda triggers the heater. The heater is powered by Smartplug that could be triggered by receiving HTTP request. By sending HTTP request, Lambda turns on and turns off the heater remotely. If the temperature inside the greenhouse is lower than the lowest desired temperature and the heater is currently off, Lambda sends a HTTP request to turn on the heater. Similarly, If the temperature inside the greenhouse is higher than the lowest desired temperature and the heater is currently on, Lambda sends a HTTP request to turn off the heater. Lambda also records how long the heater is on and stores the time in DynamoDB by months. By multiplying the time by the power of the heater, the power consumption is estimated. By multiplying the power consumption by the electricity rates, the electricity cost is estimated. 
We also provide the service of updating our the client the status of the greenhouse at a specific time. This time is input in the web application and stored in DynamoDB. Matching the specific time to the timestamp of data package is difficult. The central system stores and updates the latest timestamp in a item. Whenever Lambda receive the new data package, it compares the latest timestamp and the new timestamp to the specific time. If the latest timestamp is before the specific time and the new timestamp is after the specific time, Lambda customize a message which includes all the sensor data and power consumption and sends to client by triggering SNS.  

<a name = "Web_Application"></a>
### Web Application:
#### Backend:
The backend of the web application is built with Flask written in Python. There are many reasons for using Flask. First, the software engineers on this team are fairly familiar with Python, so Flask seemed like an obvious extension of that. Secondly, Flask is a lightweight framework and accelerates the development and production as developers can focus on the creation of the functions. Thirdly, it has Object-Relational Mapping support and clear and defined Model-View-Controller organization which assists in rapid software development. Last but not least, there are many available Python modules available so choosing Flask allows us to utilize existing Python modules.

The Flask backend communicates with the system through the use of BOTO3 Python AWS Client for simplicity. Flask uses BOTO3 to interact with the DynamoDB NoSQL service of AWS.  Using BOTO3 greatly simplifies the process of development and also accelerates development. BOTO3 retrieves lists of readings of data from DynamoDB as JSON packets. With the JSON module, reading of JSON data is easy to decode. 

Control settings are the only mechanism by which the user directly communicates with the database. These controls are handled by a NoSQL write in the DynamoDB database. The low temperature setting, high temperature setting, and the weekly notification time can all be set by the user from the web application. These new values are sent to both the AWS Lambda Function and the AWS DynamoDB database.

#### Frontend: 
The rendering of the dynamic dashboard is done with the Flask template. The only other module used in the front end is the Chart.JS framework. The Chart.JS is used to render the graphs displayed to the user. Data are retrieved from the database, transformed into an array of values in the backend from JSON objects. Then, those arrays are sent to the front end as a part of the response parameters. With the Flask templating engine, the templated variables are now provided with the necessary data for rendering from the provided data. The templated variables on the side bars are also provided and replaced to show the current state of the greenhouse. With the templated fields, the graphs are rendered.  

#### Hosting:
The client has informed us that hosting the web application on one of his existing domains or hosting locally are both acceptable options. We have chosen to host locally to reduce unnecessary cost. The project is easy to run on localhost. A run script has been written to simplify the process to start the server. Running the server should be a fairly simple task. Though the user does not have to configure AWS to run on their own account, it is strongly recommended they do so. Currently DynamoDB and Amazon Lambda are being hosted on a student account, which is likely to close after graduation. The user should set up their own AWS account to continue to receive information after May.
  
<a name = "Flow_chart"></a>
## Flow chart
![alt text](https://github.com/BostonUniversitySeniorDesign/19-13-GreenHouse/blob/master/Software/Flow%20Chart.png)
Arduino collects all sensor data and transfer to Nodemcu.
Nodemcu sends the data to AWS IOT. 
AWS IOT sends the data to Lambda. 
Lambda stores the data into AWS DynamoDB, and present the data on Web Application
Lambda triggers the Simple Notification service if the temperature is too low or too high
Lambda turns on the heater is the temperature is too low, or turns off the heater if the temperature is higher the desired lowest temperature

<a name = "Dev_build_tool_information"></a>
## Dev/build tool information: Package name and version info. 
For example, OpenCV 4.0.3  with Python 3.8.1, using CUDA Toolbox 10.0 and GCC 9.1 and CMake 3.14.2



<a name = "Install_the_project_software_stack"></a>
## Install the project software stack
<a name = "Installation_of_webapp_on_local_machine"></a>
### Installation of webapp on local machine
1. Clone the repository on to the local machine. 
2. Add .env file that contains the API keys(content of .env is provided by our team) .
3. Install Python 3 (Python 2 won’t work for this project)
4. Install Flask
5. Install Boto3 python client made by AWS
6. Install AWS client
7. Create new access key for this boto3 client on AWS 
8. Run “aws configure” to add your generated access keys to your system

<a name = "Command_to_run_the_server"></a>
### Command to run the server:
1. Go to the webapp directory
2. “./run.sh” is the only command to run the server.
3. Route for the webapp is localhost:5000/login/dashboard

<a name = "Configuring_Wemo_Smart_Plug_with_WLAN"></a>
### Configuring Wemo_Smart Plug with WLAN:
1. Download “Wemo” App from App Store onto phone (available for both iOS and Android)
2. Connect your phone to your local 2.4GHz network. NOTE: we recommend disabling auto-reconnect to all other wifi networks or entering Airplane mode to ensure your device is paired to the correct network
3. Open Wemo App and select “Add a Wemo” in the settings page
4. Follow instructions on phone to add Wemo device. NOTE: if this is your first time installing Wemo or your SSID changes, you must hold down the power button while plugging your Wemo in to reset to factory mode. Wemo is ready for setup when light is flashing white and orange.
5. If remote access is not automatically enabled, select “Enable remote access” under settings
6. Plug heater into Wemo



