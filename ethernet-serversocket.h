/*
 * Ethernet Client/Server Socket with libohiboard
 * Copyright (C) 2017-2018 A. C. Open Hardware Ideas Lab
 *
 * Authors:
 *  Marco Giammarini <m.giammarini@warcomeb.it>
 *  Matteo Civale
 *  Gianluca Calignano <g.calignano97@gmail.com>
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
 */

/**
 * @mainpage Ethernet/ServerSocket library with @a libohiboard
 *
 * This project...
 *
 * @section changelog ChangeLog
 *
 * @li v1.0 of 2018/08/XX - First release
 *
 * @section library External Library
 *
 * The library use the following external library
 * @li libohiboard https://github.com/ohilab/libohiboard a C framework for
 * NXP Kinetis microcontroller
 *
 * @section thanksto Thanks to...
 * @li Marco Giammarini
 * @li Matteo Civale
 * @li Gianluca Calignano
 */

#ifndef __OHILAB_ETHERNET_SERVERSOCKET_H
#define __OHILAB_ETHERNET_SERVERSOCKET_H

#include "ethernet-socket.h"

/**
 * @defgroup functions Ethernet server socket functions
 * The HTTP server function group
 *
 * @defgroup macros Ethernet server socket macros
 * @ingroup functions
 * Macros MUST be defined to enable the library to work properly.
 * They could be defined in board.h but there is a check in http-server.h which
 * avoid silly problems if macros are not defined.
 */

/**
 * @ingroup functions
 * This function initializes all possible sockets
 * @param config The pointer to the ethernet fonfig
 */
void EthernetServerSocket_init (EthernetSocket_Config* config);

/**
 * @ingroup functions
 * This function enable connections to the selected socket.
 * @param number Socket number.
 * @param port Port number.
 * @return ETHERNETSOCKET_ERROR_OK if everything gone well
 * other errors otherwise.
 */
EthernetSocket_Error EthernetServerSocket_connect (uint8_t number,
                                                   uint16_t port);

/**
 * @ingroup functions
 * This function checks if the selected client is connect.
 * @param[in] number Socket number.
 * @param[in] client Client number.
 * @return TRUE if the client is connected, FALSE otherwise.
 */
bool EthernetServerSocket_isConnected (uint8_t number,
                                       uint8_t client);

/**
 * @ingroup functions
 * This function closes the selected server socket.
 * @param number The number of server
 * @return ETHERNETSOCKET_ERROR_OK if everything gone well
 * other errors otherwise.
 */
EthernetSocket_Error EthernetServerSocket_disconnect (uint8_t number);

/**
 * @ingroup functions
 * This funcion closes the selected client at the selected server socket
 * @param number The number of socket
 * @param client The number of client
 * @return ETHERNETSOCKET_ERROR_OK if everything gone well
 * other errors otherwise.
 */
EthernetSocket_Error EthernetServerSocket_disconnectClient (uint8_t number, uint8_t client);

/**
 * @ingroup functions
 * This function checks if new data is available in the selected client.
 * @param[in] number
 * @param[in] client
 * @param[out] available The number of byte in the receive buffer of the
 * selected connection,
 * @return ETHERNETSOCKET_ERROR_NOT_CONNECTED if the client is not connected
 * ETHERNETSOCKET_ERROR_OK otherwise.
 */
EthernetSocket_Error EthernetServerSocket_available (uint8_t number,
                                                     uint8_t client,
                                                     int16_t* available);

/**
 * @ingroup functions
 * This function reads the last char present in the circular buffer
 * @param[in] number The number of server
 * @param[in] client The number of the client connected to the server
 * @param[out] data The pointer where the function save the byte read
 * @return ETHERNETSOCKET_ERROR_NOT_CONNECTED if the client is not connected
 * ETHERNETSOCKET_ERROR_OK otherwise.
 */
EthernetSocket_Error EthernetServerSocket_read (uint8_t number,
                                                uint8_t client,
                                                uint8_t* data);

/**
 * @ingroup functions
 * This function reads multiple bytes from the circular buffer
 * @param[in] number The number of server
 * @param[in] client The number of the client connected to the server
 * @param[out] buffer The pointer to the array where the function save the bytes read
 * @param[in] length The maximum number of bytes to read
 * @param[out] read The number of bytes read
 * @return
 */
EthernetSocket_Error EthernetServerSocket_readBytes (uint8_t number,
                                                     uint8_t client,
                                                     uint8_t buffer[],
                                                     uint16_t length,
                                                     uint16_t* read);

/**
 * @ingroup functions
 * This function connect the serve to a client
 * @param[in] number
 * @param[out] clients
 * @return ETHERNETSOCKET_ERROR_NOT_CONNECTED if the socket is not connected
 * ETHERNETSOCKET_ERROR_OK otherwise.
 */
EthernetSocket_Error EthernetServerSocket_clients (uint8_t number,
                                                   uint8_t* clients);

/**
 * @ingroup functions
 * This function writes a char to the selected client.
 * @param[in] number The number of server
 * @param[in] client The number of the client connected to the server
 * @param[in] data The byte must be written
 * @return ETHERNETSOCKET_ERROR_NOT_CONNECTED if the socket is not connected
 * ETHERNETSOCKET_ERROR_OK otherwise.
 */
EthernetSocket_Error EthernetServerSocket_write (uint8_t number,
                                                 uint8_t client,
                                                 uint8_t data);

/**
 * @ingroup functions
 * This function writes multiple bytes to the selected client.
 * @param[in] number The number of server
 * @param[in] client The number of the client connected to the server
 * @param[in] buffer The pointer to the array with data must be written
 * @param[in] length The maximum number of bytes to write
 * @param[out] wrote The number of bytes wrote
 * @return ETHERNETSOCKET_ERROR_NOT_CONNECTED if the client is not connected
 * ETHERNETSOCKET_ERROR_OK otherwise.
 */
EthernetSocket_Error EthernetServerSocket_writeBytes (uint8_t number,
                                                      uint8_t client,
                                                      uint8_t buffer[],
                                                      uint16_t length,
                                                      uint16_t* wrote);

#endif // __OHILAB_ETHERNET_SERVERSOCKET_H
