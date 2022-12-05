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
static uint8_t devEui[] = {0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x05, 0x7F, 0x2B};
static uint8_t appEui[] = {0x13, 0x37, 0x42, 0x06, 0x90, 0x00, 0x00, 0x00};
static uint8_t appKey[] = {0xB4, 0xF4, 0x1F, 0x53, 0xA3, 0x2C, 0x0D, 0x4E, 0x37, 0x0B, 0xD5, 0x50, 0x98, 0x42, 0x19, 0xE6};

uint16_t userChannelsMask[6] = {0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

///////////////////////////////////////////////////
// Some utilities for going into low power mode
TimerEvent_t sleepTimer;
// Records whether our sleep/low power timer expired
bool sleepTimerExpired;

static void wakeUp()
{
  sleepTimerExpired = true;
}

static void lowPowerSleep(uint32_t sleeptime)
{
  sleepTimerExpired = false;
  TimerInit(&sleepTimer, &wakeUp);
  TimerSetValue(&sleepTimer, sleeptime);
  TimerStart(&sleepTimer);
  // Low power handler also gets interrupted by other timers
  // So wait until our timer had expired
  while (!sleepTimerExpired)
    lowPowerHandler();
  TimerStop(&sleepTimer);
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
    Serial.print("Joining... ");
    LoRaWAN.joinOTAA(appEui, appKey, devEui);
    if (!LoRaWAN.isJoined())
    {
      // In this example we just loop until we're joined, but you could
      // also go and start doing other things and try again later
      Serial.println("JOIN FAILED! Sleeping for 30 seconds");
      lowPowerSleep(30000);
    }
    else
    {
      Serial.println("JOINED");
      break;
    }
  }
}

///////////////////////////////////////////////////
void loop()
{

  // In this demo we use a timer to go into low power mode to kill some time.
  // You might be collecting data or doing something more interesting instead.
  lowPowerSleep(15000);

  // Here we send confirmed packed (ACK requested) only for the first five (remember there is a fair use policy)
  bool requestack = false;

  // Send hello world
  char msg[] = "Hello World";

  Serial.print("Sending: ");
  Serial.println(msg);

  // Send the message
  if (LoRaWAN.send(sizeof(msg) - 1, (uint8_t *)msg, 1, requestack) == 0)
  {
    Serial.println("SENT");
  }
  else
  {
    Serial.println("SEND FAILED");
  }
}

///////////////////////////////////////////////////
// Example of handling downlink data
void downLinkDataHandle(McpsIndication_t *mcpsIndication)
{
  Serial.printf("Received downlink: %s, RXSIZE %d, PORT %d, DATA: ", mcpsIndication->RxSlot ? "RXWIN2" : "RXWIN1", mcpsIndication->BufferSize, mcpsIndication->Port);
  for (uint8_t i = 0; i < mcpsIndication->BufferSize; i++)
  {
    Serial.printf("%02X", mcpsIndication->Buffer[i]);
  }
  Serial.println();
}
