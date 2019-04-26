# Software Report

## Overview of software module
## A flow chart indicating the dependencies between these functions. 
For instance, if you have a main.py and LCD.py, you need to show that LCD.py is a module used by main.py.
## Dev/build tool information: Package name and version info. 
For example, OpenCV 4.0.3  with Python 3.8.1, using CUDA Toolbox 10.0 and GCC 9.1 and CMake 3.14.2
## Install the project software stack
### Installation of webapp on local machine
1. Clone the repository on to the local machine. 
2. Add .env file that contains the API keys(content of .env is provided by our team) .
3. Install Python 3 (Python 2 won’t work for this project)
4. Install Flask
5. Install Boto3 python client made by AWS
6. Install AWS client
7. Create new access key for this boto3 client on AWS 
8. Run “aws configure” to add your generated access keys to your system

### Command to run the the server:
1. Go to the webapp directory
2. “./run.sh” is the only command to run the server.
3. Route for the webapp is localhost:5000/login/dashboard

### Configuring Wemo Smart Plug with your WLAN:
1. Download “Wemo” App from App Store onto your phone (available for both iOS and Android)
2. Connect your phone to your local 2.4GHz network. NOTE: we recommend disabling auto-reconnect to all other wifi networks or entering Airplane mode to ensure your device is paired to the correct network
3. Open Wemo App and select “Add a Wemo” in the settings page
4. Follow instructions on phone to add Wemo device. NOTE: if this is your first time installing Wemo or your SSID changes, you must hold down the power button while plugging your Wemo in to reset to factory mode. Wemo is ready for setup when light is flashing white and orange.
5. If remote access is not automatically enabled, select “Enable remote access” under settings
6. Plug heater into Wemo


Example:
• Debian Linux 9.1 into embedded system, with kernel patch to enable the USB controller
• how to use CMake to cross-compile code, how to transfer binary into the target, how
target autoruns binary, etc.
• How to setup/startup cloud instance that collects and processes data from the
embedded system
