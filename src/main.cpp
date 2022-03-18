#include <Arduino.h>
#include <ChRt.h>
#include "MAX7300.h"

// struct definition
// struct actuation
// {
//   int timestamp;
//   int action;
// };

MAX7300 max7300;
// mailbox declaration
const size_t mailboxBufferSize = 20;
static msg_t actuationsQueue[mailboxBufferSize];
MAILBOX_DECL(Mqueue, &actuationsQueue, mailboxBufferSize);

// Declare working area of trigger task
static THD_WORKING_AREA(waTriggerTask, 2000);
thread_t *triggerTaskPointer; // pointer to trigger task

// Declare callback function for trigger task
// sorting parameters
int32_t teensyDelay = 3;
int32_t travelDelay = 0;
int32_t holdOpenDelay = 600;
int32_t portNumber = 1;
int32_t serialTimestamp;
bool flag = false;
static THD_FUNCTION(triggerTask, arg)
{
  (void)arg;
  Serial.println("Waiting for serial input.0.");
  while (true)
  {

    serialTimestamp = TIME_I2US(chVTGetSystemTime());
    if (Serial.available() > 0)
    {
      char serialMessage = Serial.read();
      if (serialMessage == 'x') // if the serial char is a droplet detection
      {
        chMBPostTimeout(&Mqueue, (msg_t)serialTimestamp, TIME_INFINITE);  // send to mailbox first timestamp to open gate
        chMBPostTimeout(&Mqueue, (msg_t)-serialTimestamp, TIME_INFINITE); ////send to mailbox second timestamp to close gate
      }
      else if (serialMessage == 'p') // p,travelDelay,holdOpenDelay,portNumber if the serial char is p then parse the parameters
      {
        travelDelay = Serial.parseInt();
        holdOpenDelay = Serial.parseInt();
        portNumber = Serial.parseInt();
        Serial.printf("New parameters applied: %d,%d,%d \n", travelDelay, holdOpenDelay, portNumber);
      }
      else if (serialMessage == 'a')
      {
        if (flag)
        {
          max7300.setAll(0); /* code */
          flag = !flag;
          Serial.println("all port are off");
        }
        else
        {
          max7300.setAll(1); /* code */
          flag = !flag;
          Serial.println("all port are on");
        }
      }
    }
    // Serial.println(micros()-serialTimestamp);
    chThdSleepMicroseconds(1);
  }
}

// Declare working area of sorting task
static THD_WORKING_AREA(waSortingTask, 2000);
thread_t *sortingTaskPointer; // pointer to sorting task

// Declare callback function of sorting task

static THD_FUNCTION(sortingTask, arg)
{
  (void)arg;

  Serial.println("sorting task is running");
  while (true)
  {
    int32_t *buffer;

    if (chMBGetUsedCountI(&Mqueue) > 0)
    {
      int32_t msg = (int32_t)chMBPeekI(&Mqueue);
      int time = TIME_I2US(chVTGetSystemTime());
      chMBFetchI(&Mqueue, (msg_t *)&buffer);
      // msg = (int32_t *)buffer;
      // int time = micros();
      // int32_t receive = *buffer;
      if (msg > 0)
      {
        int32_t delta = time - msg;
        Serial.printf("Received %d  at %d with delta %dum \n", buffer, time, delta);
      }
      else
      {
        Serial.printf("Received %d  at %d\n", buffer, time);
      }
      // Serial.println(receive);
      if (msg > 0)
      {
        int32_t sleep = max(0, msg) + travelDelay - TIME_I2US(chVTGetSystemTime());
        Serial.printf("sleeping for %d \n", max(0, sleep));
        chThdSleepMicroseconds(max(0, sleep));
        max7300.setHigh(portNumber);
        Serial.println("gate is open");
      }
      else
      {
        int32_t sleep = abs(msg) + travelDelay + holdOpenDelay - TIME_I2US(chVTGetSystemTime());
        Serial.printf("sleeping for %d \n", max(0, sleep));
        chThdSleepMicroseconds(max(0, sleep));
        Serial.println("gate is closed");
        max7300.setLow(portNumber);
      }
    }
    chThdSleepMicroseconds(1);
  }
}
void chSetup()
{
  // schedule trigger task
  sortingTaskPointer = chThdCreateStatic(waSortingTask, sizeof(waSortingTask), NORMALPRIO + 1, sortingTask, NULL);
  triggerTaskPointer = chThdCreateStatic(waTriggerTask, sizeof(waTriggerTask), NORMALPRIO + 1, triggerTask, NULL);
}

void setup()
{
  // Setup serial
  Serial.begin(20000000);
  CCM_CSCDR1 = 105450240;
  Serial.println("Welcome");
  Serial.println(Serial.baud());
  while (!Serial.available())
  {
  }
  // init MAX7300
  max7300.begin();
  // init and start ChibiOS
  chBegin(chSetup);
  while (true)
  {
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
}