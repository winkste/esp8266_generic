/*****************************************************************************************
* FILENAME :        Trace.h          
*
* DESCRIPTION :
*       Class header for trace functionality
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
*****************************************************************************************/

#ifndef TRACE_H_
#define TRACE_H_

/****************************************************************************************/
/* Imported header files: */
#include "stdint.h"
#include <string>
using namespace std;

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

        /********************************************************************************/
        /* Public function definitions: */
        Trace();
        Trace(bool isActive_bol);
        Trace(bool isActive_bol, uint8_t channel_u8);
        virtual void PushToChannel() = 0;
        virtual void Initialize() = 0;
        virtual ~Trace();
        virtual void print(uint8_t type_u8, char * msg_pc) = 0;
        virtual void println(uint8_t type_u8, char * msg_pc) = 0;
        virtual void print(uint8_t type_u8, string msg_str) = 0;
        virtual void println(uint8_t type_u8, string msg_str) = 0;
        virtual void print(uint8_t type_u8, uint8_t value_u8) = 0;
        virtual void println(uint8_t type_u8, uint8_t value_u8) = 0;

    protected:
        /********************************************************************************/
        /* Protected data definitions */
        bool isActive_bol;
        uint8_t channel_u8;
        string buffer_str;
        
        /********************************************************************************/
        /* Protected function definitions: */
        void prepareMsg(uint8_t type_u8, string msg_str);
};

#endif /* TRACE_H_ */
