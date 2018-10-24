/*****************************************************************************************
* FILENAME :        Utils.cpp
*
* DESCRIPTION :
*       Class implementation for generic Utils 
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
* AUTHOR :    Stephan Wink        START DATE :    09.09.2018
*
*****************************************************************************************/

/****************************************************************************************/
/* Include Interfaces */
#include "Utils.h"

/****************************************************************************************/
/* Local constant defines */

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for Utils
 * @author    winkste
 * @date      18 Sept. 2018
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Utils::Utils()
{
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Utils::~Utils()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Copies a float value to a character buffer
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     buffer_p    pointer to result buffer string
 * @return    pointer to char buffer
*//*-----------------------------------------------------------------------------------*/
char* Utils::FloatToString(float f, int p, char* pBuffer)
{
    return dtostrf(f, 0, p, pBuffer);       // call the library function
}

/**---------------------------------------------------------------------------------------
 * @brief     Copies a integer value to decimal representive in a character buffer
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     buffer_p    pointer to result buffer string
 * @return    pointer to char buffer
*//*-----------------------------------------------------------------------------------*/
char* Utils::IntegerToDecString(uint32_t i, char* pBuffer)
{
    return itoa(i, pBuffer, 10);       // call the library function
}

/**---------------------------------------------------------------------------------------
 * @brief     Copies a integer value to decimal representive in a character buffer
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     red_u8      decimal red value
 * @param     green_u8    decimal green value
 * @param     blue_u8     decimal blue value
 * @param     buffer_p    pointer to result buffer string
 * @return    pointer to char buffer
*//*-----------------------------------------------------------------------------------*/
char* Utils::RGBToString(uint8_t red_u8, uint8_t green_u8, uint8_t blue_u8, 
                          char* pBuffer_p)
{
    snprintf(pBuffer_p, 20, "%d,%d,%d", red_u8, green_u8, blue_u8);
    return(pBuffer_p);
}


/**---------------------------------------------------------------------------------------
 * @brief     This function helps to build the complete topic including the 
 *              custom device.
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     dev_p       pointer to device string
 * @param     channel_p   pointer to channel string
 * @param     topic_p     pointer to topic string
 * @param     buffer_p    pointer to result buffer string
 * @return    combined topic as char pointer, it uses buffer_p to store the topic
*//*-----------------------------------------------------------------------------------*/
char* Utils::BuildSendTopic(const char *dev_p, const char *channel_p, 
                                      const char *topic_p, char *buffer_p) 
{
  sprintf(buffer_p, "std/%s/s/%s/%s", dev_p, channel_p, topic_p);
  return buffer_p;
}

/**---------------------------------------------------------------------------------------
 * @brief     This function helps to build the complete topic including the 
 *              custom device.
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     dev_p       pointer to device string
 * @param     topic_p     pointer to topic string
 * @param     buffer_p    pointer to result buffer string
 * @return    combined topic as char pointer, it uses buffer_p to store the topic
*//*-----------------------------------------------------------------------------------*/
char* Utils::BuildSendTopic(const char *dev_p, 
                                      const char *topic_p, char *buffer_p) 
{
  sprintf(buffer_p, "std/%s/%s", dev_p, topic_p);
  return buffer_p;
}

/**---------------------------------------------------------------------------------------
 * @brief     This function helps to build the complete topic including the 
 *              custom device.
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     dev_p       pointer to device string
 * @param     channel_p   pointer to channel string
 * @param     topic_p     pointer to topic string
 * @param     buffer_p    pointer to result buffer string
 * @return    combined topic as char pointer, it uses buffer_p to store the topic
*//*-----------------------------------------------------------------------------------*/
char* Utils::BuildReceiveTopic(const char *dev_p, const char *channel_p, 
                                      const char *topic_p, char *buffer_p) 
{
  sprintf(buffer_p, "std/%s/r/%s/%s", dev_p, channel_p, topic_p);
  return buffer_p;
}

/**---------------------------------------------------------------------------------------
 * @brief     This function calculates the logarithm digits value (0-1023) based on the 
 *              linear  input percentage (0-100%).
 * @author    winkste
 * @date      24 Oct. 2018
 * @param     percent_u8       percentage value 0-100
 * @return    N/A
*//*-----------------------------------------------------------------------------------*/
uint16_t Utils::CalcLogDigitsFromPercent(uint8_t percent_u8)
{
  return(uint16_t) ((1023.0D * log10(max((uint8_t)1U, percent_u8))) 
                      / log10(100.0) + 0.5F);
}

/****************************************************************************************/
/* Private functions: */

