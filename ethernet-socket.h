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
 * @mainpage Ethernet Server Socket with @a libohiboard
 *
 * This library is developed in order to manage a simple
 * Ethernet server socket using the @a libohiboard and @a timer libraries.
 *
 * @section changelog ChangeLog
 *
 * @li v1.0.0 of 2018/08/24 - First release
 *
 * @section library External Library
 *
 * The library use the following external library
 * @li libohiboard https://github.com/ohilab/libohiboard a C framework for
 * NXP Kinetis microcontroller
 * @li timer https://github.com/warcomeb/timer a C library to
 * create a timer based on PIT module of libohiboard
 * @li KSX8081RNA https://github.com/Loccioni-Electronic/KSZ8081RNA
 *
 *  * @section Example
 * before starting with the example, which consist only of
 * the main.c file you MUST create <BR>
 * a board.h file in ~/ethernet_serversocket_example/Includes/board.h
 * which contains all macros in order to improve
 * code legibility.
 * <BR>It could be something
 * like this:
 *
 * @code
 * #include "libohiboard.h"
 *
 *  //macros for timer module
 *  #define WARCOMEB_TIMER_NUMBER        0
 *  #define WARCOMEB_TIMER_FREQUENCY     1000
 *  #define WARCOMEB_TIMER_CALLBACK      5
 *
 *  //macros for ethernet-serversocket module
 *  #define ETHERNET_MAX_LISTEN_CLIENT 5
 *  #define ETHERNET_MAX_SOCKET_BUFFER 1023
 *  #define ETHERNET_MAX_SOCKET_CLIENT 5
 *  #define ETHERNET_MAX_SOCKET_SERVER 5
 *
 * @endcode
 * <BR>
 *
 * While the main.c is developed only to test this library so there is small
 * echo algorithm in while(1) <BR>
 * that if neither a or b or c character is received
 * the server replies with the corrisponding uppercase character. <BR>
 * It could be something like this:
 *
 * @code
 *  //Including all needed libraries
 *  #include "libohiboard.h"
 *  #include "timer/timer.h"
 *  #include "KSZ8081RNA/KSZ8081RNA.h"
 *  #include "ethernet-socket/ethernet-serversocket.h"
 *
 *  //declare netif struct type
 *  struct netif nettest;
 *
 *  int main(void)
 *  {
 *      uint32_t fout;
 *      uint32_t foutBus;
 *      Clock_State clockState;
 *
 *      //Declare EthernetSocket_Config struct
 *      EthernetSocket_Config ethernetSocketConfig=
 *      {
 *          .timeout = 3000,
 *          .delay = Timer_delay,
 *          .currentTick = Timer_currentTick,
 *      };
 *
 *      //Declaring ClockConfig struct
 *      Clock_Config clockConfig =
 *      {
 *          .source         = CLOCK_EXTERNAL,
 *          .fext           = 50000000,
 *          .foutSys        = 120000000,
 *          .busDivider     = 2,
 *          .flexbusDivider = 4,
 *          .flashDivider   = 6,
 *      };
 *
 *      // Enable Clock
 *      System_Errors  error = Clock_Init(&clockConfig);
 *
 *      // JUST FOR DEBUG
 *      clockState = Clock_getCurrentState();
 *      fout = Clock_getFrequency (CLOCK_SYSTEM);
 *      foutBus = Clock_getFrequency (CLOCK_BUS);
 *
 *      // Enable all ports
 *      SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK |
 *                   SIM_SCGC5_PORTB_MASK |
 *                   SIM_SCGC5_PORTC_MASK |
 *                   SIM_SCGC5_PORTD_MASK |
 *                   SIM_SCGC5_PORTE_MASK;
 *
 *      // Network configurations
 *      Ethernet_NetworkConfig netConfig;
 *
 *      // fill net config
 *      IP4_ADDR(&netConfig.ip,192,168,1,6);
 *      IP4_ADDR(&netConfig.mask,255,255,255,0);
 *      IP4_ADDR(&netConfig.gateway,192,168,1,1);
 *      ETHERNET_MAC_ADDR(&netConfig.mac,0x00,0xCF,0x52,0x35,0x00,0x01);
 *
 *      netConfig.phyCallback           = KSZ8081RNA_init;
 *      netConfig.timerCallback         = Timer_currentTick;
 *      netConfig.netif_link_callback   = 0;
 *      netConfig.netif_status_callback = 0;
 *
 *      //Just for test
 *      Gpio_config(GPIO_PINS_PTB22,GPIO_PINS_OUTPUT);
 *      Gpio_config(GPIO_PINS_PTE26,GPIO_PINS_OUTPUT);
 *      Gpio_config(GPIO_PINS_PTB21,GPIO_PINS_OUTPUT);
 *
 *      //The default value is clear, in this way RGB led
 *      //will be OFF
 *      Gpio_set(GPIO_PINS_PTB22);
 *      Gpio_set(GPIO_PINS_PTE26);
 *      Gpio_set(GPIO_PINS_PTB21);
 *
 *      //timer initialization
 *      Timer_init();
 *
 *      //Ethernet server socket initialization
 *      Ethernet_networkConfig(&nettest, &netConfig);
 *
 *      //Turn the red LED on, now we can send a character to
 *      //the opened socket
 *      Gpio_clear(GPIO_PINS_PTB22);
 *
 *     while(1)
 *     {
 *         sys_check_timeouts();
 *         //A small delay
 *         Timer_delay(100);
 *
 *         //checking all clients for incoming data
 *         for (uint8_t j = 0; j < ETHERNET_MAX_LISTEN_CLIENT; ++j)
 *         {
 *             if (EthernetServerSocket_available(0,j)>0)
 *             {
 *                  //Read the data from the j client
 *                  EthernetServerSocket_read(0,j,&data);
 *
 *                  //A small echo algorithm that if neither
 *                  //a or b or c character is received the server replies
 *                  //with the corrisponding uppercase character
 *
 *                  switch(data)
 *                  {
 *                  case 'a':
 *                      EthernetServerSocket_write(0,j,'A');
 *                      break;
 *                  case 'b':
 *                      EthernetServerSocket_write(0,j,'B');
 *                      break;
 *                  case 'c':
 *                      EthernetServerSocket_write(0,j,'C');
 *                      break;
 *                  }
 *              }
 *          }
 *      }
 *      return 0;
 *  }
 * @endcode
 *
 * @section thanksto Thanks to...
 *
 * @li Marco Giammarini
 * @li Matteo Civale
 * @li Gianluca Calignano
 *
 */

#ifndef __OHILAB_ETHERNET_SOCKET_H
#define __OHILAB_ETHERNET_SOCKET_H

#define OHILAB_ETHERNET_SOCKET_LIBRARY_VERSION     "1.0.0"
#define OHILAB_ETHERNET_SOCKET_LIBRARY_VERSION_M   1
#define OHILAB_ETHERNET_SOCKET_LIBRARY_VERSION_m   0
#define OHILAB_ETHERNET_SOCKET_LIBRARY_VERSION_bug 0
#define OHILAB_ETHERNET_SOCKET_LIBRARY_TIME        1535124886

#include "libohiboard.h"

/*
 * The user must define these label... TODO
 */
#ifndef __NO_BOARD_H
#include "board.h"
#endif

/**
 * @defgroup functions Ethernet server socket functions
 * The HTTP server function group
 */

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

/**
 * @ingroup functions
 */
typedef enum
{
    ///Empty
    ETHERNETSOCKET_STATUS_EMPTY,
    ///Server initialized
    ETHERNETSOCKET_STATUS_INIT,
    ///Waiting for connection
    ETHERNETSOCKET_STATUS_WAIT_CONNECTION,
    ///Connected
    ETHERNETSOCKET_STATUS_CONNECTED,
    ///Disconnected
    ETHERNETSOCKET_STATUS_DISCONNECTED,
    ///Error
    ETHERNETSOCKET_STATUS_ERROR,
} EthernetSocket_Status;

/**
 * @ingroup functions
 */
typedef enum
{
    ///Everything gone well
    ETHERNETSOCKET_ERROR_OK,
    ///Wrong socket number
    ETHERNETSOCKET_ERROR_WRONG_SOCKET_NUMBER,
    ///Wrong client number
    ETHERNETSOCKET_ERROR_WRONG_CLIENT_NUMBER,
    ///Just connected
    ETHERNETSOCKET_ERROR_JUST_CONNECTED,
    ///Not connected
    ETHERNETSOCKET_ERROR_NOT_CONNECTED,
    ///Connection fail
    ETHERNETSOCKET_ERROR_CONNECTION_FAIL,
    ///Disconnection fail
    ETHERNETSOCKET_ERROR_DISCONNECTION_FAIL,
    ///Buffer is full
    ETHERNETSOCKET_ERROR_BUFFER_FULL,
    ///Buffer no data
    ETHERNETSOCKET_ERROR_BUFFER_NO_DATA,
    ///Open fail
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
