/*****************************************************************************************
* FILENAME :        EspGpio.cpp
*
* DESCRIPTION :
*       Implementation of the ESP internal GPIOs as GpioDevice for standard interface.
*
* NOTES :
*
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
* AUTHOR :    Stephan Wink        START DATE :    01.10.2017
*
*****************************************************************************************/

/****************************************************************************************/
/* Include Interfaces */
#include "EspGpio.h"
#include "Trace.h"

/****************************************************************************************/
/* Local constant defines */

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for the EspGpio device
 * @author    winkste
 * @date      06. Feb. 2018
 * @param     p_trace     trace object for info and error messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
EspGpio::EspGpio(Trace *p_trace) : GpioDevice(p_trace)
{
    this->p_trace->println(trace_INFO_MSG, "<<espGpio>> Constructor of ESPDevice called");
    this->stat_u8 = 0;
    this->value_u16 = 0;
    this->Initialize();
    this->PinMode();
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructur for EspGpio
 * @author    winkste
 * @date      06. Feb. 2018
 * @param     p_trace   trace object for debug output
 * @param     pin_u8    corresponding pin
 * @return    a GpioDevice object
*//*-----------------------------------------------------------------------------------*/
EspGpio::EspGpio(Trace *p_trace, uint8_t pin_u8) : GpioDevice(p_trace, pin_u8)
{
    this->p_trace->println(trace_INFO_MSG, "<<espGpio>> Constructor of ESPDevice called");
    this->stat_u8 = 0;
    this->value_u16 = 0;
    this->Initialize();
    this->PinMode();

}

/**---------------------------------------------------------------------------------------
 * @brief     Constructur for EspGpio
 * @author    winkste
 * @date      06. Feb. 2018
 * @param     p_trace   trace object for debug output
 * @param     pin_u8    corresponding pin
 * @param     dir_u8    direction of pin
 * @return    a GpioDevice object
*//*-----------------------------------------------------------------------------------*/
EspGpio::EspGpio(Trace *p_trace, uint8_t pin_u8, uint8_t dir_u8) : GpioDevice(p_trace, pin_u8, dir_u8)
{
    this->p_trace->println(trace_INFO_MSG, "<<espGpio>> Constructor of ESPDevice called");
    this->stat_u8 = 0;
    this->value_u16 = 0;
    this->Initialize();
    this->PinMode();
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      06. Feb. 2018
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
EspGpio::~EspGpio()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to set the direction of of the pin
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     dir_u8     direction of pin
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void EspGpio::PinMode(uint8_t dir_u8)
{
    //this->p_trace->println(trace_INFO_MSG, "<<espgpio>> pinMode called");
    this->PrintPinStat();
    this->dir_u8 = dir_u8;
    this->PinMode();
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to set output pin to dedicated value
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     state_u8  state of the output
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void EspGpio::DigitalWrite(uint8_t state_u8)
{
    //this->p_trace->println(trace_INFO_MSG, "<<espgpio>> digitalWrite of ESPDevice called");
    this->stat_u8 = state_u8;
    digitalWrite(this->pin_u8, this->stat_u8);
    this->value_u16 = 1023 * this->stat_u8;
    this->PrintPinStat();
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to set output pin to dedicated value
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     pin_u8    pin selection
 * @return    state of the digital pin
*//*-----------------------------------------------------------------------------------*/
uint8_t EspGpio::DigitalRead()
{
    //this->p_trace->println(trace_INFO_MSG, "<<espgpio>> digitalRead of ESPDevice called");
    this->stat_u8 = digitalRead(this->pin_u8);
    this->value_u16 = 1023 * this->stat_u8;
    this->PrintPinStat();
    return(this->stat_u8);
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to set output pin to dedicated analog value
 * @author    winkste
 * @date      12 Sep. 2018
 * @param     value_u16  analog value (0-1023)
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void EspGpio::AnalogWrite(uint16_t value_u16)
{
    //this->p_trace->println(trace_INFO_MSG, "<<espgpio>> analogWrite of ESPDevice called");
    this->value_u16 = value_u16;
    analogWrite(this->pin_u8, value_u16);
    this->PrintPinStat();
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to get analog output pin value
 * @author    winkste
 * @date      12. Sep. 2018
 * @return    analog value (0-1023)
*//*-----------------------------------------------------------------------------------*/
uint16_t EspGpio::AnalogRead()
{
    //this->p_trace->println(trace_INFO_MSG, "<<espgpio>> analogRead of ESPDevice called");
    this->value_u16 = analogRead(this->pin_u8);
    this->PrintPinStat();
    return(this->value_u16);
}

/****************************************************************************************/
/* Private functions: */
/**---------------------------------------------------------------------------------------
 * @brief     Function call to set output pin to dedicated value
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    ---
*//*-----------------------------------------------------------------------------------*/
void EspGpio::PrintPinStat()
{
    /*this->p_trace->print(trace_INFO_MSG, "<<espgpio>> Pin Status: ");
    this->p_trace->print(trace_PURE_MSG, this->pin_u8);
    this->p_trace->print(trace_PURE_MSG, " / ");
    this->p_trace->print(trace_PURE_MSG, this->dir_u8);
    this->p_trace->print(trace_PURE_MSG, " / ");
    this->p_trace->print(trace_PURE_MSG, this->stat_u8);
    this->p_trace->print(trace_PURE_MSG, " / ");
    this->p_trace->println(trace_PURE_MSG, this->value_u16);*/
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the ESP gpio device
 * @author    winkste
 * @date      06. Feb. 2018
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void EspGpio::Initialize()
{
    this->p_trace->println(trace_INFO_MSG, "<<espgpio>> initialized");
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the pin mode
 * @author    winkste
 * @date      06. Feb. 2018
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void EspGpio::PinMode()
{
    pinMode(this->pin_u8, this->dir_u8);
    this->p_trace->println(trace_INFO_MSG, "<<espgpio>> pin mode changed");
}

