/*****************************************************************************************
* FILENAME :        myVersion.h          
*
* DESCRIPTION :
*       Header file to define project specific settings
*
* PUBLIC FUNCTIONS :
*       N/A
*
* NOTES :
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
* 
* AUTHOR :    Stephan Wink        START DATE :    01.10.2017
*
*
* REF NO  VERSION DATE    WHO     DETAIL
* 000       16.10         SWI     branched from template
* 001       21.11         SWI     first version with single relay object
* 002       01.12         SWI     first version running with SONOFF switch
* 003       03.12         SWI     added dev short description to config ssid
* 004       03.12         SWI     minor bug fixes
* 005       05.12         SWI     load default params from eeprom before starting 
*                                 config portal
* 006       07.12         SWI     add broadcast command topic       
*                                 add command in dht device to toggle power save mode
* 007       10.12         SWI     rework single relay to support multi relays
*                                 add device list in v1 to support multi devices
* 008       03.01         SWI     changed config ssio
* 009       06.01         SWI     bug fixing
* 010       14.01         SWI     reworked mqtt structure and trace module rework
* 011       14.01         SWI     reworked single relay command structure
*                                 reworked trace commands in gen
* 012       18.01         SWI     implement new generic topics to read/write settings                                
*                                 without wifimanager involvement
* 013       21.01         SWI     inserted room parameter for more identification
* 014       21.01         SWI     set dht update rate to 30secs
* 015       28.01         SWI     rework trace topic structure
* 016       11.02         SWI     add Generic GPIO device and MCP23017 I2C
* 017       15.05         SWI     add capability for 8 channel relay
* 018       30.05         SWI     rework of bme and dht sensors to handle flexible report
*                                 cycle times and removed power save feature from these
*                                 sensors, this is now handled by the PowerSave device
* 019       09.06         SWI     changed mcp 8 reley to double dht, changed pins for                                 
*                                 esp relay 8
* 020       06.07         SWI     integrated Sen0193 capacitive moisture sensor  
* 021       02.08         SWI     add additional pir pin verification because of 
*                                 some wired behaviour of the pir in dev03    
* 022       25.08         SWI     introduce bcast and trace/capability write commands                         
* 023       19.09         SWI     introduce utils and DimLight device
* 024       24.09         SWI     reworked the DimLight device to more standardize
* 025       04.10         SWI     introduced neo pixels support
* 026       07.11         SWI     migrated to platformio, add max digit in DimLight, add
                                    3D printer control device
*****************************************************************************************/
#ifndef MYVERSION_H
#define MYVERSION_H

/****************************************************************************************/
/* Imported header files: */

/****************************************************************************************/
/* Global constant defines: */
#define FW_IDENTIFIER             "00004FW" // Firmware identification
#define FW_VERSION                "026"     // Firmware Version
#define FW_DESCRIPTION            "generic firmware for esp8266"

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */

/****************************************************************************************/
/* Global data allusions (allows type checking, definition in c file): */

/****************************************************************************************/
/* Global function prototypes: */

#endif /* MYVERSION_H */
/****************************************************************************************/

