This repository contains the sourcecode for a prototype called "LoRaKastl".
It has been developed by 5 students at UAS Technikum Wien during the course "InnovationLab 1".

- [1. Introduction](#1-introduction)
  - [1.1 Mailbox](#11-mailbox)
  - [1.2 Data Transmission](#12-data-transmission)
- [2. Prerequisites](#2-prerequisites)
  - [2.1 Hardware](#21-hardware)
  - [2.2 Software](#22-software)
- [3. Step-by-Step instructions](#3-step-by-step-instructions)

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
- MicroUSB **Data** Cable
- Reed Switch
- Jumper Cables

## 2.2 Software
Make sure you have
- an Account at https://www.thethingsnetwork.org/
- **PlatformIO** with VSCode (or any other compatible IDE)
- **Docker** installed on the system your Node-RED backend should run on

# 3. Step-by-Step instructions

**1. clone the repository**
```bash
$ git clone https://github.com/LoRaWahnsinn/LoRaKastl.git
```
