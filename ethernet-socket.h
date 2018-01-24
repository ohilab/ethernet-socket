/*******************************************************************************
 * Ethernet Client/Server Socket with libohiboard
 * Copyright (C) 2017-2018 A. C. Open Hardware Ideas Lab
 *
 * Authors:
 *  Marco Giammarini <m.giammarini@warcomeb.it>
 *  Matteo Civale
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

/**
 * @mainpage Ethernet Client/Server Socket with @a libohiboard
 *
 * This project...
 *
 * @section changelog ChangeLog
 *
 * @li v1.0 of 2017/03/XX - First release
 *
 * @section library External Library
 *
 * The library use the following external library
 * @li libohiboard https://github.com/ohilab/libohiboard a C framework for
 * NXP Kinetis microcontroller
 *
 * @section thanksto Thanks to...
 *
 * @li Marco Giammarini
 * @li Matteo Civale
 *
 */

#ifndef __OHILAB_ETHERNET_SOCKET_H
#define __OHILAB_ETHERNET_SOCKET_H

#define OHILAB_ETHERNET_SOCKET_LIBRARY_VERSION     "1.0.0"
#define OHILAB_ETHERNET_SOCKET_LIBRARY_VERSION_M   1
#define OHILAB_ETHERNET_SOCKET_LIBRARY_VERSION_m   0
#define OHILAB_ETHERNET_SOCKET_LIBRARY_VERSION_bug 0
#define OHILAB_ETHERNET_SOCKET_LIBRARY_TIME        0

#include "libohiboard.h"

/*
 * The user must define these label... TODO
 */
#ifndef __NO_BOARD_H
#include "board.h"
#endif

#ifndef ETHERNET_MAX_SOCKET_CLIENT
#error "Socket Client: maximum number not defined!"
#endif

#ifndef ETHERNET_MAX_SOCKET_SERVER
#error "Socket Server: maximum number not defined!"
#endif

#ifndef ETHERNET_MAX_LISTEN_CLIENT
#error "Socket Server: maximum number of client per server not defined!"
#endif

#ifndef ETHERNET_MAX_SOCKET_BUFFER
#error "Socket Client: maximum buffer dimension not defined!"
#endif

typedef enum
{
    ETHERNETSOCKET_STATUS_EMPTY,
    ETHERNETSOCKET_STATUS_INIT,
    ETHERNETSOCKET_STATUS_WAIT_CONNECTION,
    ETHERNETSOCKET_STATUS_CONNECTED,
    ETHERNETSOCKET_STATUS_DISCONNECTED,
    ETHERNETSOCKET_STATUS_ERROR,
} EthernetSocket_Status;

typedef enum
{
    ETHERNETSOCKET_ERROR_OK,
    ETHERNETSOCKET_ERROR_WRONG_NUMBER,
    ETHERNETSOCKET_ERROR_JUST_CONNECTED,
    ETHERNETSOCKET_ERROR_NOT_CONNECTED,
    ETHERNETSOCKET_ERROR_CONNECTION_FAIL,
    ETHERNETSOCKET_ERROR_DISCONNECTION_FAIL,
    ETHERNETSOCKET_ERROR_BUFFER_FULL,
    ETHERNETSOCKET_ERROR_BUFFER_NO_DATA,
    ETHERNETSOCKET_ERROR_OPEN_FAIL,
} EthernetSocket_Error;

typedef uint32_t (*EthernetSocket_CurrentTick) (void);
typedef void (*EthernetSocket_Delay) (uint32_t);

typedef struct _EthernetSocket_Config
{
    uint32_t (*currentTick) (void);            /**< Callback for basic timing */
    void (*delay) (uint32_t);                /**< Callback for blocking delay */

    uint32_t timeout;            /**< Read and write timeout operations in ms */
} EthernetSocket_Config;


#endif // __OHILAB_ETHERNET_SOCKET_H
