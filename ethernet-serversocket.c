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

#include "ethernet-serversocket.h"

typedef struct _EthernetServerSocket_Device
{
    uint8_t number;

    uint16_t port;

    struct tcp_pcb *pcb;

    uint8_t connectedClients;            /**< The number of connected clients */

    EthernetSocket_Status status;
} EthernetServerSocket_Device;

typedef struct _EthernetServerSocket_Client
{
    struct tcp_pcb *clientpcb;

    uint8_t rxBuffer[ETHERNET_MAX_SOCKET_BUFFER+1];
    uint16_t rxBufferTail;
    uint16_t rxBufferHead;

    err_t tcpError;                 /**< TCP error from error handle function */

    EthernetSocket_Status status;

    EthernetServerSocket_Device* server;
} EthernetServerSocket_Client;

static EthernetServerSocket_Device EthernetServerSocket_socket[ETHERNET_MAX_SOCKET_SERVER];

static EthernetServerSocket_Client EthernetServerSocket_listenClients[ETHERNET_MAX_SOCKET_SERVER*ETHERNET_MAX_LISTEN_CLIENT];

static EthernetSocket_CurrentTick EthernetServerSocket_currentTick;
static EthernetSocket_Delay EthernetServerSocket_delay;

static uint32_t EthernetServerSocket_timeout = 0;

static bool EthernetServerSocket_isInit = FALSE;

err_t EthernetServerSocket_receiveHandle (void *arg,
                                          struct tcp_pcb *pcb,
                                          struct pbuf *p,
                                          err_t err)
{
    uint16_t plen;
    uint8_t count = 0;
    uint8_t* payload;
    EthernetServerSocket_Client *dev = (EthernetServerSocket_Client *)arg;

    if((err == ERR_OK) && (p != NULL))
    {
        // Get buffer dimension
        plen = p->len;
        // Save payload pointer
        payload = (uint8_t *)p->payload;

        // Store all byte received into socket buffer
        for (uint16_t i = 0; i < plen; i++)
        {
            if ((dev->rxBufferTail + 1) != dev->rxBufferHead)
            {
                dev->rxBuffer[dev->rxBufferTail] = *payload;
                dev->rxBufferTail++;
                dev->rxBufferTail &= ETHERNET_MAX_SOCKET_BUFFER;
                payload++;
            }
            else
            {
                // FIXME: error!
            }
        }
        // Acknowledge of data processed
        tcp_recved(pcb,plen);
        count = pbuf_free(p);
        return ERR_OK;
    }
    else
    {
        // FIXME!
        tcp_arg(pcb,NULL);
        tcp_sent(pcb,NULL);
        tcp_recv(pcb,NULL);
        return ERR_BUF;
    }
}

void EthernetServerSocket_errorHandle (void *arg,
                                      err_t err)
{
    EthernetServerSocket_Client *dev = (EthernetServerSocket_Client *)arg;
    dev->server->status = ETHERNETSOCKET_STATUS_ERROR;
    // FIXME
    // Save error type!
    dev->tcpError = err;

    // TODO:
}

err_t EthernetServerSocket_connectionHandle (void *arg,
                                             struct tcp_pcb *pcb,
                                             err_t err)
{
    // Change the status of the socket
    EthernetServerSocket_Device *dev = (EthernetServerSocket_Device *)arg;

    // Set the status of server to "connect"
    dev->status = ETHERNETSOCKET_STATUS_CONNECTED;

    if (dev->connectedClients < ETHERNET_MAX_LISTEN_CLIENT)
    {
        uint8_t currentClient = 0;
        for (uint8_t i = 0; i < ETHERNET_MAX_LISTEN_CLIENT; i++)
        {
            currentClient = (dev->number * ETHERNET_MAX_LISTEN_CLIENT) + i;
            if (EthernetServerSocket_listenClients[currentClient].status !=
                    ETHERNETSOCKET_STATUS_CONNECTED)
                break;
        }

        // Save server pointer
        EthernetServerSocket_listenClients[currentClient].server = dev;
        // Save current PCB
        EthernetServerSocket_listenClients[currentClient].clientpcb = pcb;
        // Save into PCB the current client pointer
        tcp_arg(EthernetServerSocket_listenClients[currentClient].clientpcb,
                &EthernetServerSocket_listenClients[currentClient]);

        // Save status
        EthernetServerSocket_listenClients[currentClient].status =
                ETHERNETSOCKET_STATUS_CONNECTED;

        // Setup callback
        // Connect all handle!
        tcp_recv(EthernetServerSocket_listenClients[currentClient].clientpcb,
                 EthernetServerSocket_receiveHandle);
        tcp_err(EthernetServerSocket_listenClients[currentClient].clientpcb,
                EthernetServerSocket_errorHandle);

        // Update connected clients
        dev->connectedClients++;
        return ERR_OK;
    }
    else
    {
        // Too much clients
        return ERR_MEM;
    }
}

void EthernetServerSocket_init (EthernetSocket_Config* config)
{
	if (EthernetServerSocket_isInit == TRUE)
		return;

    // Save callback for current tick informations
    EthernetServerSocket_currentTick = config->currentTick;

    // Save callback for blocking delay function
    EthernetServerSocket_delay = config->delay;

    // Save timeout information
    if (config->timeout == 0)
        EthernetServerSocket_timeout = 100; // 100 ms - default timeout
    else
        EthernetServerSocket_timeout = config->timeout;

    for (uint8_t i = 0; i < ETHERNET_MAX_SOCKET_SERVER; ++i)
    {
        EthernetServerSocket_socket[i].number = i;
        EthernetServerSocket_socket[i].status = ETHERNETSOCKET_STATUS_INIT;
    }

    for (uint8_t i = 0; i < (ETHERNET_MAX_SOCKET_SERVER*ETHERNET_MAX_LISTEN_CLIENT); ++i)
    {
        // Init buffer pointer
        EthernetServerSocket_listenClients[i].rxBufferHead = 0;
        EthernetServerSocket_listenClients[i].rxBufferTail = 0;

        EthernetServerSocket_listenClients[i].status = ETHERNETSOCKET_STATUS_INIT;
    }

    EthernetServerSocket_isInit = TRUE;
}

EthernetSocket_Error EthernetServerSocket_connect (uint8_t number,
                                                   uint16_t port)
{
    err_t error;

    // Check if the socket exist
    if (number >= ETHERNET_MAX_SOCKET_SERVER)
        return ETHERNETSOCKET_ERROR_WRONG_SOCKET_NUMBER;

    // Check if the socket is just in use!
    if (EthernetServerSocket_socket[number].status == ETHERNETSOCKET_STATUS_CONNECTED)
        return ETHERNETSOCKET_ERROR_JUST_CONNECTED;

    EthernetServerSocket_Device *dev = &EthernetServerSocket_socket[number];

    // Save connection data
    dev->port = port;

    // Initialize process control block for application
    // and select TCP as protocol
    dev->pcb = tcp_new();
    if (dev->pcb)
    {
        error = tcp_bind(dev->pcb, IP_ADDR_ANY, port);
        if (error != ERR_OK)
        {
            tcp_abort(dev->pcb);
            return ETHERNETSOCKET_ERROR_BUFFER_NO_DATA;
        }

        // Set up the local port to listen for incoming connections
        dev->pcb = tcp_listen(dev->pcb);
        tcp_arg(dev->pcb,dev);
        tcp_setprio(dev->pcb, TCP_PRIO_NORMAL);
        // It's ready for incoming connections
        tcp_accept(dev->pcb, EthernetServerSocket_connectionHandle);

        // Set server to wait connection status
        dev->status = ETHERNETSOCKET_STATUS_WAIT_CONNECTION;
        return ETHERNETSOCKET_ERROR_OK;
    }

    dev->status = ETHERNETSOCKET_STATUS_ERROR;
    return ETHERNETSOCKET_ERROR_CONNECTION_FAIL;
    // FIXME: parse other error!!
}

bool EthernetServerSocket_isConnected (uint8_t number,
                                       uint8_t client)
{
    // Check if the socket exist
    if (number >= ETHERNET_MAX_SOCKET_SERVER)
        return ETHERNETSOCKET_ERROR_WRONG_SOCKET_NUMBER;

    // Check if the client exist
    if (client >= ETHERNET_MAX_LISTEN_CLIENT)
        return ETHERNETSOCKET_ERROR_WRONG_CLIENT_NUMBER;

    // Check if the socket is connected!
    if (EthernetServerSocket_socket[number].status != ETHERNETSOCKET_STATUS_CONNECTED)
        return FALSE;

    // Check if the client is connected
    if (EthernetServerSocket_listenClients[(number * ETHERNET_MAX_LISTEN_CLIENT) + client].status !=
        ETHERNETSOCKET_STATUS_CONNECTED)
        return FALSE;

    return TRUE;
}

EthernetSocket_Error EthernetServerSocket_disconnect (uint8_t number)
{
    // Check if the socket exist
    if (number >= ETHERNET_MAX_SOCKET_CLIENT)
        return ETHERNETSOCKET_ERROR_WRONG_SOCKET_NUMBER;

    // Check if the socket is connected!
    if (EthernetServerSocket_socket[number].status != ETHERNETSOCKET_STATUS_CONNECTED)
        return ETHERNETSOCKET_ERROR_NOT_CONNECTED;

    // Close all client connections
    // FIXME: I don't know if OK!
    for (uint8_t i = 0; i < ETHERNET_MAX_LISTEN_CLIENT; ++i)
    {
        uint8_t tmpClient = (number * ETHERNET_MAX_LISTEN_CLIENT) + i;
        if (EthernetServerSocket_listenClients[tmpClient].status ==
            ETHERNETSOCKET_STATUS_CONNECTED)
        {
            // Close the connection with the client
            struct tcp_pcb * pcb = EthernetServerSocket_listenClients[tmpClient].clientpcb;
            tcp_arg(pcb,NULL);
            tcp_sent(pcb,NULL);
            tcp_recv(pcb,NULL);

            err_t error = tcp_close(pcb);
            if (error == ERR_OK)
            {
                EthernetServerSocket_listenClients[tmpClient].status =
                        ETHERNETSOCKET_STATUS_DISCONNECTED;
            }
        }
    }

    // Close the server socket
    EthernetServerSocket_Device *dev = &EthernetServerSocket_socket[number];

    // Delete all callback and other
    tcp_arg(dev->pcb,NULL);
    tcp_sent(dev->pcb,NULL);
    tcp_recv(dev->pcb,NULL);

    err_t error = tcp_close(dev->pcb);
    if (error == ERR_OK)
    {
        dev->status = ETHERNETSOCKET_STATUS_DISCONNECTED;
        dev->connectedClients = 0;
        return ETHERNETSOCKET_ERROR_OK;
    }
    return ETHERNETSOCKET_ERROR_DISCONNECTION_FAIL;
}

EthernetSocket_Error EthernetServerSocket_disconnectClient (uint8_t number, uint8_t client)
{
    // Check if the socket exist
    if (number >= ETHERNET_MAX_SOCKET_SERVER)
        return ETHERNETSOCKET_ERROR_WRONG_SOCKET_NUMBER;

    // Check if the client exist
    if (client >= ETHERNET_MAX_LISTEN_CLIENT)
        return ETHERNETSOCKET_ERROR_WRONG_CLIENT_NUMBER;

    EthernetServerSocket_Device *dev = &EthernetServerSocket_socket[number];

    uint8_t tmpClient = (number * ETHERNET_MAX_LISTEN_CLIENT) + client;
    if (EthernetServerSocket_listenClients[tmpClient].status ==
        ETHERNETSOCKET_STATUS_CONNECTED)
    {
        // Close the connection with the client
        struct tcp_pcb * pcb = EthernetServerSocket_listenClients[tmpClient].clientpcb;
        tcp_arg(pcb,NULL);
        tcp_sent(pcb,NULL);
        tcp_recv(pcb,NULL);
        tcp_err(pcb,NULL);

        err_t error = tcp_close(pcb);
        if (error == ERR_OK)
        {
            EthernetServerSocket_listenClients[tmpClient].status =
                    ETHERNETSOCKET_STATUS_DISCONNECTED;
            dev->connectedClients--;
        }
    }
}

EthernetSocket_Error EthernetServerSocket_available (uint8_t number,
                                                     uint8_t client,
                                                     int16_t* available)
{
    if (EthernetServerSocket_isConnected(number,client) == FALSE)
        return ETHERNETSOCKET_ERROR_NOT_CONNECTED;

    uint8_t tmpClient = (number * ETHERNET_MAX_LISTEN_CLIENT) + client;

    *available = (EthernetServerSocket_listenClients[tmpClient].rxBufferTail -
            EthernetServerSocket_listenClients[tmpClient].rxBufferHead);

    return ETHERNETSOCKET_ERROR_OK;
}

EthernetSocket_Error EthernetServerSocket_read (uint8_t number,
                                                uint8_t client,
                                                uint8_t* data)
{
    // Clear data
    *data = 0;

    if (EthernetServerSocket_isConnected(number,client) == FALSE)
        return ETHERNETSOCKET_ERROR_NOT_CONNECTED;

    uint8_t tmpClient = (number * ETHERNET_MAX_LISTEN_CLIENT) + client;

    // Save a pointer of the requested client
    EthernetServerSocket_Client *dev = &EthernetServerSocket_listenClients[tmpClient];

    // Read the buffer
    if (dev->rxBufferTail != dev->rxBufferHead)
    {
        *data = dev->rxBuffer[dev->rxBufferHead++];
        dev->rxBufferHead &= ETHERNET_MAX_SOCKET_BUFFER;
        return ETHERNETSOCKET_ERROR_OK;
    }
    return ETHERNETSOCKET_ERROR_BUFFER_NO_DATA;
}

EthernetSocket_Error EthernetServerSocket_clients (uint8_t number, uint8_t* clients)
{
    // default value
    *clients = 0;

    // Check if the socket exist
    if (number >= ETHERNET_MAX_SOCKET_SERVER)
        return ETHERNETSOCKET_ERROR_WRONG_SOCKET_NUMBER;

    // Check if the socket is just in use!
    if (EthernetServerSocket_socket[number].status != ETHERNETSOCKET_STATUS_CONNECTED)
        return ETHERNETSOCKET_ERROR_NOT_CONNECTED;

    *clients = EthernetServerSocket_socket[number].connectedClients;
    return ETHERNETSOCKET_ERROR_OK;
}

EthernetSocket_Error EthernetServerSocket_write (uint8_t number,
                                                 uint8_t client,
                                                 uint8_t data)
{
    if (EthernetServerSocket_isConnected(number,client) == FALSE)
        return ETHERNETSOCKET_ERROR_NOT_CONNECTED;

    uint16_t wrote = 0;
    return EthernetServerSocket_writeBytes(number,client,&data,1,&wrote);

}

EthernetSocket_Error EthernetServerSocket_writeBytes (uint8_t number,
                                                      uint8_t client,
                                                      uint8_t buffer[],
                                                      uint16_t length,
                                                      uint16_t* wrote)
{
    if (EthernetServerSocket_isConnected(number,client) == FALSE)
        return ETHERNETSOCKET_ERROR_NOT_CONNECTED;

    uint8_t tmpClient = (number * ETHERNET_MAX_LISTEN_CLIENT) + client;

    // Save a pointer of the requested client
    EthernetServerSocket_Client *dev = &EthernetServerSocket_listenClients[tmpClient];

    uint16_t maxByte = tcp_sndbuf(dev->clientpcb);
    // Check the available space on the tx buffer
    if(maxByte < length)
    {
        // Set the maximum message length to the available space on the buffer
        length = maxByte;
    }

    // Enqueues the data pointed to by buffer
    if(tcp_write(dev->clientpcb, buffer, length, TCP_WRITE_FLAG_COPY) == ERR_OK)
    {
        tcp_output(dev->clientpcb);
        *wrote = length;
        return ETHERNETSOCKET_ERROR_OK;
    }
    else
    {
        return ETHERNETSOCKET_ERROR_BUFFER_FULL;
    }
}
