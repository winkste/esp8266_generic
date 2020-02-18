/*****************************************************************************************
* FILENAME :        TraceErr.h
*
* DESCRIPTION :
*       Class header for trace serial functionality
*
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

#ifndef TRACEERR_H_
#define TRACEERR_H_

/****************************************************************************************/
/* Imported header files: */
#include "Trace.h"
#include <iostream>
#include <string>
using namespace std;

/****************************************************************************************/
/* Global constant defines: */

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */

/****************************************************************************************/
/* Class definition: */
class TraceErr : public Trace
{
    public:
        /********************************************************************************/
        /* Public data definitions */

        /********************************************************************************/
        /* Public function definitions: */
        TraceErr();
        TraceErr(bool isActive_bol);
        TraceErr(bool isActive_bol, uint8_t channel_u8);
        void PushToChannel();
        void Initialize();
        virtual ~TraceErr();
        void print(uint8_t type_u8, char * msg_pc);
        void println(uint8_t type_u8, char * msg_pc);
        void print(uint8_t type_u8, string msg_str);
        void println(uint8_t type_u8, string msg_str);
        void print(uint8_t type_u8, uint8_t value_u8);
        void println(uint8_t type_u8, uint8_t value_u8);

    protected:
        /********************************************************************************/
        /* Protected data definitions */
        
        /********************************************************************************/
        /* Protected function definitions: */
};

#endif /* TRACEERR_H_ */
