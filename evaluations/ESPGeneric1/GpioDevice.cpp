/*****************************************************************************************
* FILENAME :        GpioDevice.cpp
*
* DESCRIPTION :
*       Abstract Gpio class
*
* NOTES :
*
* Copyright (c) [2018] [Stephan Wink]
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
* AUTHOR :    Stephan Wink        START DATE :    06. Feb. 2018
*
*****************************************************************************************/

/****************************************************************************************/
/* Include Interfaces */
#include "GpioDevice.h"
#include "Trace.h"

/****************************************************************************************/
/* Local constant defines */

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Static Data instantiation */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructur for GpioDevice
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace   trace object for debug output
 * @return    a GpioDevice object
*//*-----------------------------------------------------------------------------------*/
GpioDevice::GpioDevice(Trace *p_trace)
{
    this->p_trace = p_trace;
    this->pin_u8 = 0;
    this->dir_u8 = gpioDevice_INPUT;
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructur for GpioDevice
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace   trace object for debug output
 * @param     pin_u8    corresponding pin
 * @return    a GpioDevice object
*//*-----------------------------------------------------------------------------------*/
GpioDevice::GpioDevice(Trace *p_trace, uint8_t pin_u8)
{
    this->p_trace = p_trace;
    this->pin_u8 = pin_u8;
    this->dir_u8 = gpioDevice_INPUT;
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructur for GpioDevice
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace   trace object for debug output
 * @param     pin_u8    corresponding pin
 * @param     dir_u8    corresponding pin direction
 * @return    a GpioDevice object
*//*-----------------------------------------------------------------------------------*/
GpioDevice::GpioDevice(Trace *p_trace, uint8_t pin_u8, uint8_t dir_u8)
{
    this->p_trace = p_trace;
    this->pin_u8 = pin_u8;
    this->dir_u8 = dir_u8;
}
/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
GpioDevice::~GpioDevice()
{
}

/**---------------------------------------------------------------------------------------
 * @brief     Get the pin number id
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
uint8_t GpioDevice::GetPinNumber_u8(void)
{
    return (this->pin_u8);
}

/****************************************************************************************/
/* Private functions: */
