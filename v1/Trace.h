/*****************************************************************************************
* FILENAME :        Trace.h          
*
* DESCRIPTION :
*       Class header for trace functionality
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
*****************************************************************************************/

#ifndef TRACE_H_
#define TRACE_H_

/****************************************************************************************/
/* Imported header files: */
#include <ESP8266WiFi.h>

/****************************************************************************************/
/* Global constant defines: */
#define trace_PURE_MSG  0u
#define trace_INFO_MSG  1u
#define trace_WARN_MSG  2u
#define trace_ERROR_MSG 3u

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */

/****************************************************************************************/
/* Class definition: */
class Trace
{
    public:
        /********************************************************************************/
        /* Public data definitions */
        const uint8_t PURE_MSG  = 0;
        const uint8_t INFO_MSG  = 1;
        const uint8_t WARN_MSG  = 2;
        const uint8_t ERROR_MSG = 3;

        /********************************************************************************/
        /* Public function definitions: */
        Trace();
        Trace(bool isActive_bol);
        Trace(bool isActive_bol, uint8_t channel_u8);
        void PushToChannel();
        void Initialize();
        virtual ~Trace();
        void print(uint8_t type_u8, char * msg_pc);
        void println(uint8_t type_u8, char * msg_pc);
        void print(uint8_t type_u8, String msg_str);
        void println(uint8_t type_u8, String msg_str);
        void print(uint8_t type_u8, uint8_t value_u8);
        void println(uint8_t type_u8, uint8_t value_u8);

    private:
        /********************************************************************************/
        /* Private data definitions */
        bool isActive_bol;
        uint8_t channel_u8;
        String buffer_str;
        
        /********************************************************************************/
        /* Private function definitions: */
        void prepareMsg(uint8_t type_u8, String msg_str);
};

#endif /* TRACE_H_ */
