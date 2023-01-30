/**
 * @file main.cpp
 * @author Team LoRaWahnsinn
 * @brief The source code for the Heltec CubeCell AB01 microcontroller
 *
 * @copyright Copyright Team LoRaWahnsinn (c) 2023
 *
 */

#include "LoRaWanMinimal_APP.h"
#include "Arduino.h"
#define timetillsleep 10000

/*
 * set LoraWan_RGB to Active,the RGB active in loraWan
 * RGB red means sending;
 * RGB purple means joined done;
 * RGB blue means RxWindow1;
 * RGB yellow means RxWindow2;
 * RGB green means received done;
 */

// OTAA (Over the air activation) parameters matching the device registered at TTN
static uint8_t devEui[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t appEui[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t appKey[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint16_t userChannelsMask[6] = {0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

static TimerEvent_t sleep;
uint8_t lowpower = 1;

void onSleep()
{
  Serial.println("Interrupt!");
  delay(5);
  lowpower = 1;
}

void onWakeUp()
{
  Serial.println("Woke up by GPIO");
  lowpower = 0;
}


void setup()
{
  Serial.begin(9600);

  LoRaWAN.begin(LORAWAN_CLASS, ACTIVE_REGION);

  // Enable Adaptive Data Rate
  LoRaWAN.setAdaptiveDR(true);

  while (1)
  {
    Serial.print("Joining... \n");
    LoRaWAN.joinOTAA(appEui, appKey, devEui);
    if (!LoRaWAN.isJoined())
    {
      Serial.println("JOIN FAILED!\n");
    }
    else
    {
      Serial.println("JOINED.");
      break;
    }
  }

  pinMode(GPIO1, INPUT_PULLUP);

  attachInterrupt(GPIO1, onWakeUp, RISING);

  TimerInit(&sleep, onSleep);
}


void loop()
{
  if (lowpower)
  {
    Serial.println("Going into lowpower mode. Press user key to wake up.");
    lowPowerHandler();
  }

  // We don't request confirmation packets
  bool requestack = false;

  // Set message
  char msg[] = "Check mail";

  Serial.println("Sending");

  // Send the message
  if (LoRaWAN.send(sizeof(msg) - 1, (uint8_t *)msg, 1, requestack) == 0)
  {
    Serial.println("SENT");
  }
  else
  {
    Serial.println("SEND FAILED");
  }

  delay(5000);
  lowpower = 1;
}
