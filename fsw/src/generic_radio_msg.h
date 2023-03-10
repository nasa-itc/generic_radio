/*******************************************************************************
** File:
**   generic_radio_msg.h
**
** Purpose:
**  Define GENERIC_RADIO application commands and telemetry messages
**
*******************************************************************************/
#ifndef _GENERIC_RADIO_MSG_H_
#define _GENERIC_RADIO_MSG_H_

#include "cfe.h"
#include "generic_radio_device.h"


/*
** Ground Command Codes
*/
#define GENERIC_RADIO_NOOP_CC                 0
#define GENERIC_RADIO_RESET_COUNTERS_CC       1
#define GENERIC_RADIO_CONFIG_CC               2
#define GNEERIC_RADIO_PROXIMITY_CC            3


/* 
** Telemetry Request Command Codes
*/
#define GENERIC_RADIO_REQ_HK_TLM              0


/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} GENERIC_RADIO_NoArgs_cmd_t;


/*
** GENERIC_RADIO write configuration command
*/
typedef struct
{
    uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint32   DeviceCfg;

} GENERIC_RADIO_Config_cmd_t;


/*
** GENERIC_RADIO proximity forward command
*/
typedef struct
{
    uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16   SCID;
    uint8    Payload[GENERIC_RADIO_CFG_PROX_SIZE];

} GENERIC_RADIO_Proximity_cmd_t;


/*
** GENERIC_RADIO housekeeping type definition
*/
typedef struct 
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8   CommandErrorCount;
    uint8   CommandCount;
    uint8   DeviceErrorCount;
    uint8   DeviceCount;
    uint8   ForwardErrorCount;
    uint8   ForwardCount;
    GENERIC_RADIO_Device_HK_tlm_t DeviceHK;

} OS_PACK GENERIC_RADIO_Hk_tlm_t;
#define GENERIC_RADIO_HK_TLM_LNGTH sizeof ( GENERIC_RADIO_Hk_tlm_t )

#endif /* _GENERIC_RADIO_MSG_H_ */
