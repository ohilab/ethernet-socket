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

#ifndef __OHILAB_ETHERNET_SERVERSOCKET_H
#define __OHILAB_ETHERNET_SERVERSOCKET_H

#include "ethernet-socket.h"

void EthernetServerSocket_init (EthernetSocket_Config* config);

EthernetSocket_Error EthernetServerSocket_connect (uint8_t number,
                                                   uint16_t port);

/**
 * @param[in] number
 * @param[in] client
 * @return TRUE if the client is connected, FALSE otherwise.
 */
bool EthernetServerSocket_isConnected (uint8_t number,
                                       uint8_t client);

/**
 * This function close the selected server socket.
 *
 * @param number The number of server
 */
EthernetSocket_Error EthernetServerSocket_disconnect (uint8_t number);

/**
 * This funcion close the selected client at the selected server socket
 * @param number The number of server
 * @param client The number of client
 */
EthernetSocket_Error EthernetServerSocket_disconnectClient (uint8_t number, uint8_t client);

/**
 *
 * @param[in] number
 * @param[in] client
 * @return The number of byte in the receive buffer of the selected connection,
 * -1 in case of any problems.
 */
int16_t EthernetServerSocket_available (uint8_t number, uint8_t client);

/**
 *
 * @param[in] number The number of server
 * @param[in] client The number of the client connected to the server
 * @param[out] data The pointer where the function save the byte read
 * @return
 */
EthernetSocket_Error EthernetServerSocket_read (uint8_t number,
                                                uint8_t client,
                                                uint8_t* data);

/**
 *
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
 * @param[in] number
 * @param[out] clients
 * @return
 */
EthernetSocket_Error EthernetServerSocket_clients (uint8_t number,
                                                   uint8_t* clients);

/**
 *
 * @param[in] number The number of server
 * @param[in] client The number of the client connected to the server
 * @param[in] data The byte must be written
 * @return
 */
EthernetSocket_Error EthernetServerSocket_write (uint8_t number,
                                                 uint8_t client,
                                                 uint8_t data);

/**
 *
 * @param[in] number The number of server
 * @param[in] client The number of the client connected to the server
 * @param[in] buffer The pointer to the array with data must be written
 * @param[in] length The maximum number of bytes to write
 * @param[out] wrote The number of bytes wrote
 * @return
 */
EthernetSocket_Error EthernetServerSocket_writeBytes (uint8_t number,
                                                      uint8_t client,
                                                      uint8_t buffer[],
                                                      uint16_t length,
                                                      uint16_t* wrote);

#endif // __OHILAB_ETHERNET_SERVERSOCKET_H
