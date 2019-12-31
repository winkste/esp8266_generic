/* Hello World Example
 
   This example code is in the Public Domain (or CC0 licensed, at your option.)
 
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "driver/gpio.h"

 
#define BLINK_GPIO 2

#define BIT_0	( 1 << 0 )
#define BIT_4	( 1 << 4 )

TimerHandle_t xTimer;

/* Declare a variable to hold the created event group. */
EventGroupHandle_t xCreatedEventGroup;

 QueueHandle_t xQueue;
 unsigned int queueMsgCounter = 0U;

 // Task to create a queue and post a value.
 void QueueSenderTask( void *pvParameters )
 {
    // Create a queue capable of containing 10 pointers to AMessage structures.
    // These should be passed by pointer as they contain a lot of data.
    xQueue = xQueueCreate(10, sizeof(unsigned int));
    if( xQueue == 0 )
    {
        // Failed to create the queue.
    }
    else
    {
        while(1)
        {
            xQueueSend(xQueue, (void *)&queueMsgCounter, (TickType_t) 0);
            queueMsgCounter++;
            vTaskDelay(500 / portTICK_RATE_MS);
        }
        
    }
 }

 // Task to receive from the queue.
 void QueueReceiverTask( void *pvParameters )
 {
    unsigned int receivedMsgCounter = 0;
    bool msgReceived = false;

    if(xQueue != 0)
    {
        while(1)
        {
            do
            {
                // Receive a message on the created queue.  Block for 10 ticks if a
                // message is not immediately available.
                if(xQueueReceive(xQueue, &receivedMsgCounter, (TickType_t) 0))
                {
                    msgReceived = true;
                    printf("Message received :%d\n", receivedMsgCounter);
                }
                else
                {
                    msgReceived = false;
                    printf("No more messages, go to sleep\n");
                }
            }while(true == msgReceived);

            vTaskDelay(2000 / portTICK_RATE_MS);
        }
    }

	// ... Rest of task code.
 }
 
 
void hello_task(void *pvParameter)
{
 
	while(1)
	{
	    printf("Hello world2!\n");
	    vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

void timer_callback(TimerHandle_t xTimer)
{
 
	printf("Hello from Software Timer!\n");

    /* Set bit 0 and bit 4 in xEventGroup. */
    (void)xEventGroupSetBits(
                              xCreatedEventGroup,   /* The event group being updated. */
                              BIT_0 /*| BIT_4 */);      /* The bits being set. */
}
 
void blinky(void *pvParameter)
{
 
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) 
    {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_RATE_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void eventWaiting(void *pvParameter)
{
    EventBits_t uxBits;
    const TickType_t xTicksToWait = 10000 / portTICK_PERIOD_MS;

    while(1) 
    {
        if(NULL != xCreatedEventGroup)
        {
            /* Wait a maximum of 100ms for either bit 0 or bit 4 to be set within
            the event group.  Clear the bits before exiting. */
            uxBits = xEventGroupWaitBits(
                        xCreatedEventGroup,     /* The event group being tested. */
                        BIT_0 | BIT_4,          /* The bits within the event group to wait for. */
                        pdTRUE,                 /* BIT_0 & BIT_4 should be cleared before returning. */
                        pdFALSE,              /* Don't wait for both bits, either bit will do. */
                        //pdTRUE,                /* wait for both bits, either bit will do. */
                        xTicksToWait);         /* Wait a maximum of 100ms for either bit to be set. */

            if((uxBits & ( BIT_0 | BIT_4))  == (BIT_0 | BIT_4))
            {
                /* xEventGroupWaitBits() returned because both bits were set. */
                printf("Received event 0 & 4\n");
            }
            else if( ( uxBits & BIT_0 ) != 0 )
            {
                /* xEventGroupWaitBits() returned because just BIT_0 was set. */
                printf("Received event 0\n");
            }
            else if( ( uxBits & BIT_4 ) != 0 )
            {
                /* xEventGroupWaitBits() returned because just BIT_4 was set. */
                printf("Received event 4\n");
            }
            else
            {
                /* xEventGroupWaitBits() returned because xTicksToWait ticks passed
                without either BIT_0 or BIT_4 becoming set. */
                printf("Timeout for wait\n");
            }
        }
    }
}
 
 
void app_main()
{
    /* Attempt to create the event group. */
    xCreatedEventGroup = xEventGroupCreate();

    /* Was the event group created successfully? */
    if( xCreatedEventGroup == NULL )
    {
        /* The event group was not created because there was insufficient
        FreeRTOS heap available. */
        printf("Error while allocating event group\n");
    }
    else
    {
        /* The event group was created. */
        xTaskCreate(&eventWaiting, "waitForEvent", 2048, NULL, 5, NULL);
    }

    //nvs_flash_init();
    xTimer = xTimerCreate
                   ( /* Just a text name, not used by the RTOS
                     kernel. */
                     "Timer",
                     /* The timer period in ticks, must be
                     greater than 0. */
                     1000 / portTICK_RATE_MS,
                     /* The timers will auto-reload themselves
                     when they expire. */
                     pdTRUE,
                     /* The ID is used to store a count of the
                     number of times the timer has expired, which
                     is initialised to 0. */
                     ( void * ) 0,
                     /* Each timer calls the same callback when
                     it expires. */
                     timer_callback
                   );
    /* Start the timer.  No block time is specified, and
    even if one was it would be ignored because the RTOS
    scheduler has not yet been started. */
    if( xTimerStart(xTimer, 0) != pdPASS )
    {
        /* The timer could not be set into the Active
        state. */
        printf("Error while allocating timer\n");
    }
    xTaskCreate(&hello_task, "hello_task", 2048, NULL, 5, NULL);
    xTaskCreate(&blinky, "blinky", 512, NULL, 5, NULL);  
    xTaskCreate(&QueueSenderTask, "qsend_task", 2048, NULL, 5, NULL); 
    xTaskCreate(&QueueReceiverTask, "qrec_task", 2048, NULL, 5, NULL); 
}