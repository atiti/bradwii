/* 
Copyright 2013 Brad Quick

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// This library manages serial ports.  It sets up input and output buffers and sends and receieves using interrupts.
// To use this code, define which ports to include and the size of their buffers in projectsettings.h
// Then use the library like this:
//
// lib_serial_initport(2,9600); // init serial port 2 to 9600 baud
// lib_serial_sendstring(2,"Send this string");
// lib_serial_sendchar(2,'.');
// lib_serial_senddata(2,"This is Data",4); // sends the first 4 characters of the string
// int count=lib_serial_numcharsavailable(2);
// if (count>0)
//      {
//      lib_serial_getdata(2,data,count);
//      }

#include "hal.h"
#include "lib_serial.h"
#include "projectsettings.h"

// for semihosting
static const int SYS_WRITEC = 0x03;
static const int SYS_WRITE0 = 0x04;

static serialPort_t *lib_serial_getport(unsigned char serialportnumber)
{
    switch (serialportnumber) {
        case 1:
            return NULL; //FIXME: &serialPort1;
        case 2:
            return NULL; //FIXME: &serialPort2;
        default:
            return NULL;
    }
}

int lib_serial_availableoutputbuffersize(unsigned char serialportnumber)
{
    // returns how many more bytes can fit in the outputbuffer
    return 128; // TODO who cares
}

void lib_serial_setrxcallback(unsigned char serialportnumber, serialcallbackfunctptr callback)
{
    serialPort_t *port = lib_serial_getport(serialportnumber);

    port->callback = callback;
}

void lib_serial_initport(unsigned char serialportnumber, long baud)
{ 
    // initialize the serial port and set up a read buffer and interrupts so that we don't lose any data from reading too slowly
    switch (serialportnumber) {
        case 1:
            serialOpen(USART1, NULL, baud, MODE_RXTX);
            break;
        case 2:
            serialOpen(USART2, NULL, baud, MODE_RX);
            break;
    }
}

void lib_serial_sendchar(unsigned char serialportnumber, unsigned char c)
{
    if (serialportnumber == SEMIHOSTPORTNUMBER) {
        asm volatile ("mov r0, %0\n\t"
                      "mov r1, %1\n\t"
                      "bkpt 0xAB" : :"I" (SYS_WRITEC), "l" ((int)&c): );
        return;
    }
    // add a character to the send buffer
    serialPort_t *port = lib_serial_getport(serialportnumber);
    uartWrite(port, c);
}
   
void lib_serial_sendstring(unsigned char serialportnumber, char *string)
{
    if (serialportnumber == SEMIHOSTPORTNUMBER) {
        asm volatile ("mov r0, %0\n\t"
                      "mov r1, %1\n\t"
                      "bkpt 0xAB" : :"I" (SYS_WRITE0), "l" (string): );
        return;
    }
    // adds the string to the output buffer.
    while (*string) 
        lib_serial_sendchar(serialportnumber, *string++);
}

void lib_serial_senddata(unsigned char serialportnumber, unsigned char *data, int datalength)
{
    // send datalength bytes of data to the serial port
    while (datalength-- >0) 
        lib_serial_sendchar(serialportnumber, *data++);
}

void lib_serial_sendhex(unsigned char serialportnumber, void *data) {
   char string[]="0123456789abcdef";
   lib_serial_sendstring(serialportnumber, "0x");
   lib_serial_sendchar(serialportnumber, string[((int)data >> 28) & 0xF]);
   lib_serial_sendchar(serialportnumber, string[((int)data >> 24) & 0xF]);
   lib_serial_sendchar(serialportnumber, string[((int)data >> 20) & 0xF]);
   lib_serial_sendchar(serialportnumber, string[((int)data >> 16) & 0xF]);
   lib_serial_sendchar(serialportnumber, string[((int)data >> 12) & 0xF]);
   lib_serial_sendchar(serialportnumber, string[((int)data >> 8) & 0xF]);
   lib_serial_sendchar(serialportnumber, string[((int)data >> 4) & 0xF]);
   lib_serial_sendchar(serialportnumber, string[((int)data >> 0) & 0xF]);
}

int lib_serial_numcharsavailable(unsigned char serialportnumber)
{
    if (serialportnumber == SEMIHOSTPORTNUMBER) {
        return 0;
    }
    // returns number of characters available in the rx buffer
    serialPort_t *port = lib_serial_getport(serialportnumber);
    return uartAvailable(port);
}

unsigned char lib_serial_getchar(unsigned char serialportnumber)
{
    if (serialportnumber == SEMIHOSTPORTNUMBER) {
        return 0;
    }
    // get the next character from the serial port
    serialPort_t *port = lib_serial_getport(serialportnumber);
    return uartRead(port);
}

void lib_serial_getdata(unsigned char serialportnumber, unsigned char *data, int numchars)
{
    int x;
    if (serialportnumber == SEMIHOSTPORTNUMBER) return;

    for (x = 0; x < numchars; ++x)
        *data++ = lib_serial_getchar(serialportnumber);
}
