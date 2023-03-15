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
    #define GENERIC_RADIO_CFG_PROX_SIZE        64
    #define GENERIC_RADIO_CFG_FSW_IP           "0.0.0.0"
    #define GENERIC_RADIO_CFG_DEVICE_IP        "0.0.0.0"
    #define GENERIC_RADIO_CFG_UDP_FSW_TO_PROX  5012
    #define GENERIC_RADIO_CFG_UDP_PROX_TO_FSW  5013
    #define GENERIC_RADIO_CFG_UDP_FSW_TO_RADIO 5014
    #define GENERIC_RADIO_CFG_UDP_RADIO_TO_FSW 5015
    /* Note: Debug flag disabled (commented out) by default */
    #define GENERIC_RADIO_CFG_DEBUG
#endif

#endif /* _GENERIC_RADIO_PLATFORM_CFG_H_ */
