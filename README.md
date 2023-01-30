This repository contains the sourcecode for a prototype called "LoRaKastl".
It has been developed by 5 students at UAS Technikum Wien during the course "InnovationLab 1".

- [1. Introduction](#1-introduction)
  - [1.1 Mailbox](#11-mailbox)
  - [1.2 Data Transmission](#12-data-transmission)
- [2. Prerequisites](#2-prerequisites)
  - [2.1 Hardware](#21-hardware)
  - [2.2 Software](#22-software)
- [3. Step-by-Step instructions](#3-step-by-step-instructions)
  - [3.1 Set up the end device in your TTN account](#31-set-up-the-end-device-in-your-ttn-account)
  - [3.2 Download the repository](#32-download-the-repository)
  - [3.3 Build \& Upload CubeCell software](#33-build--upload-cubecell-software)
  - [3.4 Test the LoRaWAN connection](#34-test-the-lorawan-connection)
  - [3.5 Test sending a message](#35-test-sending-a-message)
  - [3.6 Add the Reed Switch](#36-add-the-reed-switch)
  - [3.7 Build \& run the Node-RED docker image\*\*](#37-build--run-the-node-red-docker-image)
  - [3.8 configure MQTT](#38-configure-mqtt)
  - [3.9 Configure Telegram Bot](#39-configure-telegram-bot)

# 1. Introduction

The purpose of the project was to learn about LoRa/LoRaWAN and IoT programming in general. The final idea was to build a physical mailbox that notifies you on your phone, whenever it is being opened. It was meant to be a POC to overcome the inital hurdles while working with our tech stack.

## 1.1 Mailbox
The mailbox itself has the following components:
- a LoRaWAN compatible microcontroller with an antenna attached to it that sends a message into the "TheThingsNetwork" once the cover is being opened, and otherwise idles in low power sleep.
- a Reed Switch Sensor to recognize the box cover being opened
- a battery, to power the mc
- a 3d-printed case the mc fits in

## 1.2 Data Transmission

Once the data has arrived at [TheThingsNetwork](https://thethingsnetwork.org), it needs to be forwarded.
To receive the message at our phone, we did the following:

- TheThingsNetwork has a built in **MQTT** broker which we used to subscribe to our device's uplink messages via a
- [**Node-RED**](https://nodered.org/) instance - e.g. running at a remote server or a local Docker container.
- the Node-RED instance parses the message and forwards it to the **Telegram Bot API**
- in the end, the message arrives in the Telegram App. 


Microcontroller -> LoRaWAN -> TTN -> NodeRed (MQTT) -> Telegram Bot

# 2. Prerequisites
The following things are needed in order to set up the whole message delivery pipeline:

## 2.1 Hardware
- Heltec CubeCell AB01 https://heltec.org/project/htcc-ab01/
  - Data sheet: https://resource.heltec.cn/download/Manual%20Old/AB01%20User%20Manual.pdf
- MicroUSB **Data** Cable
- Reed Switch x1
- Jumper Cables x2
- Arduino Battery
- Some Magnet

## 2.2 Software
Make sure you have
- an Account at https://www.thethingsnetwork.org/
- **PlatformIO** as VSCode extension (or any other compatible IDE)
- **Docker** installed on the system your Node-RED backend should run on

# 3. Step-by-Step instructions

## 3.1 Set up the end device in your TTN account

- Log into your TTN account
- Create a new application and give an appropriate name
- Register a new end device
  - Select "HelTec AutoMation" as End device brand
  -  "HTCC-AB01(Class A)" as Model
  -  Hardware Version "Unknown"
  -  Firmware Ver "1.0"
  -  Profile (Region) "EU_863_870" - the right model + image should pop up
  -  Select the recommended frequency plan (SF9)
  -  For the JoinEUI, enter all zeros
  -  Generate a DevEUI
  -  Generate an AppKey
  -  Hit "Register end device" 
- leave the browser tab open

## 3.2 Download the repository

```shell
$ git clone https://github.com/LoRaWahnsinn/LoRaKastl.git
```
- open it in VSCode via `code LoRaKastl/`
- PlatformIO should recognize the configuration automatically
- goto `src/main.cpp` and fill in the Variables with the values of your device registered at ttn
  - those are needed for the over-the-air-activation (OTAA)
```cpp
static uint8_t devEui[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t appEui[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t appKey[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
```

## 3.3 Build & Upload CubeCell software

- Plug your CubeCell into a USB-Port
- Press the "Home" icon located at the bottom bar, to get to the PIO Home
- Click left on "Devices" and check, if your device appears under the "Serial" tab
  - If its not present, (under Windows) check for optional driver updates and/or reboot your system
  - Linux users should be able to solve the problem themselves.
- Hit "Build" (the checkmark) in the bottom bar and hope it turns out green.
- If so, hit "Upload" right next to it.
- As soon as the process finishes successfully, the LED on your CubeCell should begin to blink, indicating the LoRa module being active. 

## 3.4 Test the LoRaWAN connection
- open the PIO Serial Monitor, the socket and wait until `JOINED` is being printed.
- open your ttn device browser tab and switch to "Live Data"
- If you see a join request, the connection was successful

## 3.5 Test sending a message
- attach or solder a Jumper Cable to GND and GPIO1 each
- Bring the tho cables together and watch the Serial Monitor, it prints "SENT" if successful
- Check the Live Data at TTN if your Message arrived (Uplink data)
- It should contain the string "Check mail" in Hexadecimal format

## 3.6 Add the Reed Switch

- connect each cable to the reed switch
- one side has two contacts, we need the default CLOSED one 
- bring a magnet near the sensor, then release it and check, if a message has been sent
- if a message is sent regardless of the magnet being present, try the other contact
 
## 3.7 Build & run the Node-RED docker image**
```shell
$ docker build -t lorakastl node-red/ 
$ docker run --detach -p 1880:1880 --name lorakastl lorakastl
```

## 3.8 configure MQTT

- at TTN select left Integrations -> MQTT
- generate a new API Key and save it, it is only viewable once.
- note the username
- goto http://localhost:1880
- doubleclick the node "TTN Endpoint"
  - Hit the pencil icon to configure the MQTT server
  - Change to tab "Security" and enter your username and API Key
  - Hit Update
  - The topic you need to subscribe to has the following format: `v3/<your-username-here>/devices/eui-<your-eui-here>/up` - Enter your username and id of your device
  - hit Done, then hit Deploy in the upper right corner

## 3.9 Configure Telegram Bot

- doubleclick the Telegram node
- Hit the pencil icon to configure the bot
- enter your Token you got from BotFather
- if you need help, consider reading: https://flows.nodered.org/node/node-red-contrib-telegrambot
- Hit Update, hit done.
- Finally, hit deploy

**Done! Now you should (fingers crossed) get a message to your Telegram Bot, shortly after you remove the magnet from to your reed switch.**