/*******************************************************************************
** File: generic_radio_app.h
**
** Purpose:
**   This is the main header file for the GENERIC_RADIO application.
**
*******************************************************************************/
#ifndef _GENERIC_RADIO_APP_H_
#define _GENERIC_RADIO_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "generic_radio_device.h"
#include "generic_radio_events.h"
#include "generic_radio_platform_cfg.h"
#include "generic_radio_perfids.h"
#include "generic_radio_msg.h"
#include "generic_radio_msgids.h"
#include "generic_radio_version.h"
#include "hwlib.h"


/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define GENERIC_RADIO_PIPE_DEPTH            32


/*
** Enabled and Disabled Definitions
*/
#define GENERIC_RADIO_DEVICE_DISABLED       0
#define GENERIC_RADIO_DEVICE_ENABLED        1


/*
** GENERIC_RADIO global data structure
** The cFE convention is to put all global app data in a single struct. 
** This struct is defined in the `generic_radio_app.h` file with one global instance 
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    GENERIC_RADIO_Hk_tlm_t   HkTelemetryPkt;   /* GENERIC_RADIO Housekeeping Telemetry Packet */
    
    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_SB_MsgPtr_t MsgPtr;             /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;            /* Pipe Id for HK command pipe */
    uint32 RunStatus;                   /* App run status for controlling the application state */

    /*
	** Device data 
	*/
	uint32 DeviceID;		            /* Device ID provided by CFS on initialization */
    GENERIC_RADIO_Device_tlm_t DevicePkt;      /* Device specific data packet */

    /* 
    ** Device protocol
    ** TODO: Make specific to your application
    */ 
    uart_info_t Generic_radioUart;             /* Hardware protocol definition */

} GENERIC_RADIO_AppData_t;


/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern GENERIC_RADIO_AppData_t GENERIC_RADIO_AppData; /* GENERIC_RADIO App Data */


/*
**
** Local function prototypes.
**
** Note: Except for the entry point (GENERIC_RADIO_AppMain), these
**       functions are not called from any other source module.
*/
void  GENERIC_RADIO_AppMain(void);
int32 GENERIC_RADIO_AppInit(void);
void  GENERIC_RADIO_ProcessCommandPacket(void);
void  GENERIC_RADIO_ProcessGroundCommand(void);
void  GENERIC_RADIO_ProcessTelemetryRequest(void);
void  GENERIC_RADIO_ReportHousekeeping(void);
void  GENERIC_RADIO_ReportDeviceTelemetry(void);
void  GENERIC_RADIO_ResetCounters(void);
void  GENERIC_RADIO_Enable(void);
void  GENERIC_RADIO_Disable(void);
int32 GENERIC_RADIO_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 expected_length);

#endif /* _GENERIC_RADIO_APP_H_ */
