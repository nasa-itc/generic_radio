#ifndef _SAMPLE_CHECKOUT_DEVICE_CFG_H_
#define _SAMPLE_CHECKOUT_DEVICE_CFG_H_

/*
** Default GENERIC_RADIO Configuration
*/
#ifndef GENERIC_RADIO_CFG
    #define GENERIC_RADIO_CFG_PROX_DATA_SIZE   64
    #define GENERIC_RADIO_CFG_FSW_IP           "0.0.0.0"
    #define GENERIC_RADIO_CFG_DEVICE_IP        "0.0.0.0"
    #define GENERIC_RADIO_CFG_DEVICE_DELAY_MS  10
    #define GENERIC_RADIO_CFG_UDP_PROX_TO_FSW  7010
    #define GENERIC_RADIO_CFG_UDP_FSW_TO_PROX  7011
    #define GENERIC_RADIO_CFG_UDP_FSW_TO_RADIO 5014
    #define GENERIC_RADIO_CFG_UDP_RADIO_TO_FSW 5015
    /* Note: Debug flag disabled (commented out) by default */
    //#define GENERIC_RADIO_CFG_DEBUG
#endif

#endif /* _SAMPLE_CHECKOUT_DEVICE_CFG_H_ */
