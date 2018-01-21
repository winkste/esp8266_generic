/*****************************************************************************************
* FILENAME :        gensettings.h          
*
* DESCRIPTION :
*       Header file to define general/template settings
*
* Copyright (c) [2017] [Stephan Wink]
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
vAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*****************************************************************************************/
#ifndef GENSETTINGS_H
#define GENSETTINGS_H

/****************************************************************************************/
/* Imported header files: */

/****************************************************************************************/
/* Global constant defines: */
#define CONFIG_SSID               "ESP_CFG_V7_" // SSID of the configuration mode
#define MAX_AP_TIME               300 // restart eps after 300 sec in config mode
#define CONNECT_RETRIES           5 

//#define MSG_BUFFER_SIZE         60  // mqtt messages max char size
#define MQTT_DEFAULT_DEVICE       "devXX" // default room device 

#define MQTT_PUB_FW_IDENT         "/s/gen/fwident" //firmware identification
#define MQTT_PUB_FW_VERSION       "/s/gen/fwversion" //firmware version
#define MQTT_PUB_FW_DESC          "/s/gen/desc" //firmware description
#define MQTT_PUB_DEV_ROOM         "/s/gen/room" //firmware description
#define MQTT_PUB_CAP              "/s/gen/cap"  // send capability
#define MQTT_PUB_TRACE            "/s/gen/trac" // send trace channel
#define MQTT_PUB_PARAM            "/s/gen/par"  // send all parameter 
#define MQTT_SUB_COMMAND          "/r/gen/cmd" // command message for generic read commands
#define MQTT_SUB_CAP              "/r/gen/cap" // write message for capability
#define MQTT_SUB_TRACE            "/r/gen/trac" // write message for capability 
#define MQTT_SUB_BCAST            "bcast/r/gen/cmd" // broadcast command message
#define MQTT_CLIENT               MQTT_DEFAULT_DEVICE // just a name used to talk to MQTT broker
#define MQTT_PAYLOAD_CMD_INFO     "INFO"
#define MQTT_PAYLOAD_CMD_SETUP    "SETUP"
#define MQTT_PAYLOAD_CMD_CAP      "CAP"
#define MQTT_PAYLOAD_CMD_TRAC     "TRACE"
#define MQTT_PAYLOAD_CMD_PAR      "PAR"
#define PUBLISH_TIME_OFFSET       200     // ms timeout between two publishes

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */
// Buffer to hold data from the WiFi manager for mqtt login
typedef struct mqttData_tag{ //xx byte
  char login[16];
  char pw[16];
  char dev_short[6];
  char room[16];
  char cap[4]; // capability
  char chan[4]; // capability
  char server_ip[16];
  char server_port[6];
}mqttData_t;

/****************************************************************************************/
/* Global data allusions (allows type checking, definition in c file): */

/****************************************************************************************/
/* Global function prototypes: */

#endif /* GENSETTINGS_H */
/****************************************************************************************/
