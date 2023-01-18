/**
 * This is an example of joining, sending and receiving data via LoRaWAN using a more minimal interface.
 *
 * The example is configured for OTAA, set your keys into the variables below.
 *
 * The example will upload a counter value periodically, and will print any downlink messages.
 *
 * please disable AT_SUPPORT in tools menu
 *
 * David Brodrick.
 */
#include "LoRaWanMinimal_APP.h"
#include "Arduino.h"

/*
 * set LoraWan_RGB to Active,the RGB active in loraWan
 * RGB red means sending;
 * RGB purple means joined done;
 * RGB blue means RxWindow1;
 * RGB yellow means RxWindow2;
 * RGB green means received done;
 */

// Set these OTAA parameters to match your app/node in TTN
static uint8_t devEui[] = {0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x05, 0x95, 0x43};
static uint8_t appEui[] = {0x00, 0x10, 0x30, 0x03, 0x70, 0x04, 0x02, 0x00};
static uint8_t appKey[] = {0x71, 0xD6, 0x86, 0x16, 0xA0, 0xB5, 0x5F, 0x63, 0x77, 0x35, 0xD1, 0x4A, 0x0F, 0xFA, 0xD6, 0x2B};

uint16_t userChannelsMask[6] = {0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

///////////////////////////////////////////////////


#define timetillsleep 10000
static TimerEvent_t sleep;
uint8_t lowpower = 1;

void onSleep() {
    Serial.println("Interrupt!");
    delay(5);
    lowpower = 1;
}

void onWakeUp() {
    //delay(10);
    //if (digitalRead(GPIO1) == HIGH) {
    Serial.println("Woke up by GPIO");
    lowpower = 0;
    // timetillsleep ms later into lowpower mode;
    // TimerSetValue(&sleep, timetillsleep);
    // TimerStart(&sleep);
    //}
}

///////////////////////////////////////////////////
void setup()
{
  Serial.begin(9600);

  if (ACTIVE_REGION == LORAMAC_REGION_AU915)
  {
    // TTN uses sub-band 2 in AU915
    LoRaWAN.setSubBand2();
  }

  LoRaWAN.begin(LORAWAN_CLASS, ACTIVE_REGION);

  // Enable ADR
  LoRaWAN.setAdaptiveDR(true);

  while (1)
  {
    Serial.print("Joining... \n");
    LoRaWAN.joinOTAA(appEui, appKey, devEui);
    if (!LoRaWAN.isJoined())
    {
      // In this example we just loop until we're joined, but you could
      // also go and start doing other things and try again later
      Serial.println("JOIN FAILED! Sleeping for 30 seconds \n");
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

///////////////////////////////////////////////////
void loop()
{
  if (lowpower) {
    Serial.println("Going into lowpower mode. Press user key to wake up.");
    lowPowerHandler();
  }

  // Here we send confirmed packed (ACK requested) only for the first five (remember there is a fair use policy)
  bool requestack = false;

  // Send hello world
  char msg[] = "Hello World";

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
