/************************************************************************
** File:
**   $Id: generic_radio_platform_cfg.h  $
**
** Purpose:
**  Define generic_radio Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _GENERIC_RADIO_PLATFORM_CFG_H_
#define _GENERIC_RADIO_PLATFORM_CFG_H_

/*
** Default GENERIC_RADIO Configuration
*/
#ifndef GENERIC_RADIO_CFG
    /* Notes: 
    **   NOS3 uart requires matching handle and bus number
    */
    #define GENERIC_RADIO_CFG_STRING           "usart_6"
    #define GENERIC_RADIO_CFG_HANDLE           6 
    #define GENERIC_RADIO_CFG_BAUDRATE_HZ      115200
    #define GENERIC_RADIO_CFG_MS_TIMEOUT       50            /* Max 255 */
    /* Note: Debug flag disabled (commented out) by default */
    //#define GENERIC_RADIO_CFG_DEBUG
#endif

#endif /* _GENERIC_RADIO_PLATFORM_CFG_H_ */
