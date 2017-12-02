/*****************************************************************************************
* FILENAME :        Trace.cpp         
*
* DESCRIPTION :
*       File to support simple wemos relay shield
*
* NOTES :
*             
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
*****************************************************************************************/

/*****************************************************************************************
 * Include Interfaces
*****************************************************************************************/
#include <ESP8266WiFi.h>
#include "Trace.h"

/*****************************************************************************************
 * Local constant defines
*****************************************************************************************/

/*****************************************************************************************
 * Local function like makros 
*****************************************************************************************/

/*****************************************************************************************
 * Local type definitions (enum, struct, union):
*****************************************************************************************/

/*****************************************************************************************
* Public functions (unlimited visibility): 
*****************************************************************************************/

/**---------------------------------------------------------------------------------------
 * @brief     Default constructor   
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Trace::Trace()
{
    this->isActive_bol = true;
    this->channel_u8 = 0;

}

/**---------------------------------------------------------------------------------------
 * @brief     Default constructor for channel 0
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     isActive_bol     trace active or not
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Trace::Trace(bool isActive_bol)
{
    this->isActive_bol = isActive_bol;
    this->channel_u8 = 0;

}

/**---------------------------------------------------------------------------------------
 * @brief     Full constructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     isActive_bol     trace active or not
 * @param     channel_u16      channel identifier to allow later different trace channels
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Trace::Trace(bool isActive_bol, uint8_t channel_u8)
{
    this->isActive_bol = isActive_bol;
    this->channel_u8 = channel_u8;
}

/**---------------------------------------------------------------------------------------
 * @brief     Full destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Trace::~Trace()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the trace interface selected with the constructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::Initialize()
{
    // init the serial
    Serial.begin(115200);
    Serial.println("");
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print function for String parameter w/o new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_str         message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::print(uint8_t type_u8, String msg_str)
{
  prepareMsg(type_u8, msg_str);
  Serial.print(buffer_str);
  
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print line function for String parameter with new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_str         message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::println(uint8_t type_u8, String msg_str)
{
  prepareMsg(type_u8, String(msg_str));
  Serial.println(buffer_str);
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print function for char pointer parameter w/o new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_pc          message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::print(uint8_t type_u8, char *msg_pc)
{
  prepareMsg(type_u8, String(msg_pc));
  Serial.print(buffer_str);
  
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print line function for char pointer parameter with new line 
 *              at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_pc          message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::println(uint8_t type_u8, char *msg_pc)
{
  prepareMsg(type_u8, String(msg_pc));
  Serial.println(buffer_str);
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print function for uint8_t parameter w/o new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     value_u8        data value
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::print(uint8_t type_u8, uint8_t value_u8)
{
  prepareMsg(type_u8, String(value_u8));
  Serial.print(buffer_str);
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print line function for uint8_t parameter with new line 
 *              at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     value_u8        data value
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::println(uint8_t type_u8, uint8_t value_u8)
{
  prepareMsg(type_u8, String(value_u8));
  Serial.println(buffer_str);
}

/**---------------------------------------------------------------------------------------
 * @brief     For asynchronous data channels, this function can be used to send 
 *            queued messages to the channel at a dedicated time.
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::PushToChannel()
{
    // not needed in this implementation
}

/****************************************************************************************/
/* Private functions: */
/****************************************************************************************/
/**---------------------------------------------------------------------------------------
 * @brief     Trace preparation function to combine message type and message 
 *              at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_str         message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::prepareMsg(uint8_t type_u8, String msg_str)
{
  //String temp_str = String(p_pc);
  switch(type_u8)
  {
    case trace_PURE_MSG:
      buffer_str = String(msg_str);
      break;
    case trace_INFO_MSG:
      buffer_str = String("[INFO]" + msg_str);
      break;
    case trace_WARN_MSG:
      buffer_str = String("[WARN]" + msg_str);
      break;
    case trace_ERROR_MSG:
    buffer_str = String("[ERROR]" + msg_str);
      break;
    default:
      buffer_str = String(msg_str);
      break;
  }
}

