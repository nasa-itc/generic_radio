/*******************************************************************************
** File: generic_radio_app.c
**
** Purpose:
**   This file contains the source code for the GENERIC_RADIO application.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_radio_app.h"


/*
** Global Data
*/
GENERIC_RADIO_AppData_t GENERIC_RADIO_AppData;

static CFE_EVS_BinFilter_t  GENERIC_RADIO_EventFilters[] =
{   /* Event ID    mask */
    {GENERIC_RADIO_RESERVED_EID,           0x0000},
    {GENERIC_RADIO_STARTUP_INF_EID,        0x0000},
    {GENERIC_RADIO_LEN_ERR_EID,            0x0000},
    {GENERIC_RADIO_PIPE_ERR_EID,           0x0000},
    {GENERIC_RADIO_SUB_CMD_ERR_EID,        0x0000},
    {GENERIC_RADIO_SUB_REQ_HK_ERR_EID,     0x0000},
    {GENERIC_RADIO_PROCESS_CMD_ERR_EID,    0x0000},
    {GENERIC_RADIO_CMD_ERR_EID,            0x0000},
    {GENERIC_RADIO_CMD_NOOP_INF_EID,       0x0000},
    {GENERIC_RADIO_CMD_RESET_INF_EID,      0x0000},
    {GENERIC_RADIO_CMD_CONFIG_INF_EID,     0x0000},
    {GENERIC_RADIO_CONFIG_INF_EID,         0x0000},
    {GENERIC_RADIO_CONFIG_ERR_EID,         0x0000},
    {GENERIC_RADIO_DEVICE_TLM_ERR_EID,     0x0000},
    {GENERIC_RADIO_REQ_HK_ERR_EID,         0x0000},
    {GENERIC_RADIO_SOCK_OPEN_ERR_EID,      0x0000},
    {GENERIC_RADIO_SOCK_CONNECT_ERR_EID,   0x0000},
    {GENERIC_RADIO_PROX_OPEN_ERR_EID,      0x0000},
    {GENERIC_RADIO_PROX_CONNECT_ERR_EID,   0x0000},
    {GENERIC_RADIO_TASK_REG_ERR_EID,       0x0000},
    {GENERIC_RADIO_TASK_REG_INF_EID,       0x0000},
};


/*
** Application entry point and main process loop
*/
void GENERIC_RADIO_AppMain(void)
{
    int32 status = OS_SUCCESS;

    /*
    ** Register the application with executive services
    */
    CFE_ES_RegisterApp();

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(GENERIC_RADIO_PERF_ID);

    /* 
    ** Perform application initialization
    */
    status = GENERIC_RADIO_AppInit();
    if (status != CFE_SUCCESS)
    {
        GENERIC_RADIO_AppData.RunStatus = CFE_ES_APP_ERROR;
    }

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&GENERIC_RADIO_AppData.RunStatus) == TRUE)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(GENERIC_RADIO_PERF_ID);

        /* 
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        status = CFE_SB_RcvMsg(&GENERIC_RADIO_AppData.MsgPtr, GENERIC_RADIO_AppData.CmdPipe, CFE_SB_PEND_FOREVER);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(GENERIC_RADIO_PERF_ID);

        /*
        ** If the CFE_SB_RcvMsg was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            GENERIC_RADIO_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(GENERIC_RADIO_PIPE_ERR_EID, CFE_EVS_ERROR, "GENERIC_RADIO: SB Pipe Read Error = %d", (int) status);
            GENERIC_RADIO_AppData.RunStatus = CFE_ES_APP_ERROR;
        }
    }

    /*
    ** Clean up interface upon exit
    */
    socket_close(&GENERIC_RADIO_AppData.RadioSocket);

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(GENERIC_RADIO_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(GENERIC_RADIO_AppData.RunStatus);
} 


/* 
** Initialize application
*/
int32 GENERIC_RADIO_AppInit(void)
{
    int32 status = OS_SUCCESS;
    
    GENERIC_RADIO_AppData.RunStatus = CFE_ES_APP_RUN;

    /*
    ** Register the events
    */ 
    status = CFE_EVS_Register(GENERIC_RADIO_EventFilters,
                              sizeof(GENERIC_RADIO_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                              CFE_EVS_BINARY_FILTER);    /* as default, no filters are used */
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_RADIO: Error registering for event services: 0x%08X\n", (unsigned int) status);
       return status;
    }

    /*
    ** Create the Software Bus command pipe 
    */
    status = CFE_SB_CreatePipe(&GENERIC_RADIO_AppData.CmdPipe, GENERIC_RADIO_PIPE_DEPTH, "RADIO_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_RADIO_PIPE_ERR_EID, CFE_EVS_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",(unsigned int) status);
       return status;
    }
    
    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(GENERIC_RADIO_CMD_MID, GENERIC_RADIO_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_RADIO_SUB_CMD_ERR_EID, CFE_EVS_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            GENERIC_RADIO_CMD_MID, (unsigned int) status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(GENERIC_RADIO_REQ_HK_MID, GENERIC_RADIO_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_RADIO_SUB_REQ_HK_ERR_EID, CFE_EVS_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            GENERIC_RADIO_REQ_HK_MID, (unsigned int) status);
        return status;
    }


    /* 
    ** Initialize the published HK message - this HK message will contain the 
    ** telemetry that has been defined in the GENERIC_RADIO_HkTelemetryPkt for this app.
    */
    CFE_SB_InitMsg(&GENERIC_RADIO_AppData.HkTelemetryPkt,
                   GENERIC_RADIO_HK_TLM_MID,
                   GENERIC_RADIO_HK_TLM_LNGTH, TRUE);


    /* 
    ** Always reset all counters during application initialization 
    */
    GENERIC_RADIO_ResetCounters();

    /*
    ** Initialize application data
    ** Note that counters are excluded as they were reset in the previous code block
    */
    GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceHK.DeviceCounter = 0;
    GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceHK.DeviceConfig = 0;
    GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceHK.ProxSignal = 0;

    /*
    ** Initialize sockets
    */
    char fsw_ip[] = GENERIC_RADIO_CFG_FSW_IP;
    char radio_ip[] = GENERIC_RADIO_CFG_DEVICE_IP;
    int radio_port = GENERIC_RADIO_CFG_UDP_FSW_TO_RADIO;
    int prox_port = GENERIC_RADIO_CFG_UDP_FSW_TO_PROX;

    GENERIC_RADIO_AppData.RadioSocket.sockfd = -1;
    GENERIC_RADIO_AppData.RadioSocket.port_num = GENERIC_RADIO_CFG_UDP_RADIO_TO_FSW;
    GENERIC_RADIO_AppData.RadioSocket.ip_address = fsw_ip;
    GENERIC_RADIO_AppData.RadioSocket.address_family = ip_ver_4;
    GENERIC_RADIO_AppData.RadioSocket.type = dgram;
    GENERIC_RADIO_AppData.RadioSocket.category = client;
    GENERIC_RADIO_AppData.RadioSocket.block = FALSE;
    GENERIC_RADIO_AppData.RadioSocket.keep_alive = FALSE;
    GENERIC_RADIO_AppData.RadioSocket.created = FALSE;
    GENERIC_RADIO_AppData.RadioSocket.bound = FALSE;
    GENERIC_RADIO_AppData.RadioSocket.listening = FALSE;
    GENERIC_RADIO_AppData.RadioSocket.connected = FALSE;

    status = socket_create(&GENERIC_RADIO_AppData.RadioSocket);
    if (status != SOCKET_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_RADIO_SOCK_OPEN_ERR_EID, CFE_EVS_ERROR, "GENERIC_RADIO: Radio interface create error %d", status);
        return status;
    }
    status = socket_connect(&GENERIC_RADIO_AppData.RadioSocket, radio_ip, radio_port);
    if (status != SOCKET_SUCCESS)
    {   
        CFE_EVS_SendEvent(GENERIC_RADIO_SOCK_CONNECT_ERR_EID, CFE_EVS_ERROR, "GENERIC_RADIO: Radio interface connect error %d", status);
        return status;
    }

    GENERIC_RADIO_AppData.ProxySocket.sockfd = -1;
    GENERIC_RADIO_AppData.ProxySocket.port_num = GENERIC_RADIO_CFG_UDP_PROX_TO_FSW;
    GENERIC_RADIO_AppData.ProxySocket.ip_address = fsw_ip;
    GENERIC_RADIO_AppData.ProxySocket.address_family = ip_ver_4;
    GENERIC_RADIO_AppData.ProxySocket.type = dgram;
    GENERIC_RADIO_AppData.ProxySocket.category = client;
    GENERIC_RADIO_AppData.ProxySocket.block = TRUE;
    GENERIC_RADIO_AppData.ProxySocket.keep_alive = FALSE;
    GENERIC_RADIO_AppData.ProxySocket.created = FALSE;
    GENERIC_RADIO_AppData.ProxySocket.bound = FALSE;
    GENERIC_RADIO_AppData.ProxySocket.listening = FALSE;
    GENERIC_RADIO_AppData.ProxySocket.connected = FALSE;

    status = socket_create(&GENERIC_RADIO_AppData.ProxySocket);
    if (status != SOCKET_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_RADIO_PROX_OPEN_ERR_EID, CFE_EVS_ERROR, "GENERIC_RADIO: Proximity interface create error %d", status);
        return status;
    }
    status = socket_connect(&GENERIC_RADIO_AppData.ProxySocket, radio_ip, prox_port);
    if (status != SOCKET_SUCCESS)
    {   
        CFE_EVS_SendEvent(GENERIC_RADIO_PROX_CONNECT_ERR_EID, CFE_EVS_ERROR, "GENERIC_RADIO: Proxmity interface connect error %d", status);
        return status;
    }

    /* 
    ** Start device task
    */
    status = CFE_ES_CreateChildTask(&GENERIC_RADIO_AppData.DeviceID,
                                    GENERIC_RADIO_DEVICE_NAME,
                                    (void *) GENERIC_RADIO_ProxyTask, 0,
                                    GENERIC_RADIO_DEVICE_STACK_SIZE,
                                    GENERIC_RADIO_DEVICE_PRIORITY, 0);
    if (status != CFE_SUCCESS)
    {
        return status;
    }

    /* 
     ** Send an information event that the app has initialized. 
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(GENERIC_RADIO_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               "GENERIC_RADIO App Initialized. Version %d.%d.%d.%d",
                GENERIC_RADIO_MAJOR_VERSION,
                GENERIC_RADIO_MINOR_VERSION, 
                GENERIC_RADIO_REVISION, 
                GENERIC_RADIO_MISSION_REV);	
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_RADIO: Error sending initialization event: 0x%08X\n", (unsigned int) status);
    }
    return status;
} 


/* 
** Process packets received on the GENERIC_RADIO command pipe
*/
void GENERIC_RADIO_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_GetMsgId(GENERIC_RADIO_AppData.MsgPtr);
    switch (MsgId)
    {
        /*
        ** Ground Commands with command codes fall under the GENERIC_RADIO_CMD_MID (Message ID)
        */
        case GENERIC_RADIO_CMD_MID:
            GENERIC_RADIO_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        */
        case GENERIC_RADIO_REQ_HK_MID:
            GENERIC_RADIO_ProcessTelemetryRequest();
            break;

        /*
        ** All other invalid messages that this app doesn't recognize, 
        ** increment the command error counter and log as an error event.  
        */
        default:
            GENERIC_RADIO_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_RADIO_PROCESS_CMD_ERR_EID,CFE_EVS_ERROR, "GENERIC_RADIO: Invalid command packet, MID = 0x%x", MsgId);
            break;
    }
    return;
} 


/*
** Process ground commands
*/
void GENERIC_RADIO_ProcessGroundCommand(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgPtr_t prox_msg;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case
    */
    CFE_SB_MsgId_t MsgId = CFE_SB_GetMsgId(GENERIC_RADIO_AppData.MsgPtr);   

    /*
    ** Ground Commands, by definition, have a command code (_CC) associated with them
    ** Pull this command code from the message and then process
    */
    uint16 CommandCode = CFE_SB_GetCmdCode(GENERIC_RADIO_AppData.MsgPtr);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case GENERIC_RADIO_NOOP_CC:
            /*
            ** First, verify the command length immediately after CC identification 
            ** Note that VerifyCmdLength handles the command and command error counters
            */
            if (GENERIC_RADIO_VerifyCmdLength(GENERIC_RADIO_AppData.MsgPtr, sizeof(GENERIC_RADIO_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                /* Second, send EVS event on successful receipt ground commands*/
                CFE_EVS_SendEvent(GENERIC_RADIO_CMD_NOOP_INF_EID, CFE_EVS_INFORMATION, "GENERIC_RADIO: NOOP command received");
                /* Third, do the desired command action if applicable, in the case of NOOP it is no operation */
            }
            break;

        /*
        ** Reset Counters Command
        */
        case GENERIC_RADIO_RESET_COUNTERS_CC:
            if (GENERIC_RADIO_VerifyCmdLength(GENERIC_RADIO_AppData.MsgPtr, sizeof(GENERIC_RADIO_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_RADIO_CMD_RESET_INF_EID, CFE_EVS_INFORMATION, "GENERIC_RADIO: RESET counters command received");
                GENERIC_RADIO_ResetCounters();
            }
            break;

        /*
        ** Set Configuration Command
        ** Note that this is an example of a command that has additional arguments
        */
        case GENERIC_RADIO_CONFIG_CC:
            if (GENERIC_RADIO_VerifyCmdLength(GENERIC_RADIO_AppData.MsgPtr, sizeof(GENERIC_RADIO_Config_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_RADIO_CMD_CONFIG_INF_EID, CFE_EVS_INFORMATION, "GENERIC_RADIO: Configuration command received");
                /* Command device to send HK */
                status = GENERIC_RADIO_SetConfiguration(&GENERIC_RADIO_AppData.RadioSocket,
                     ((GENERIC_RADIO_Config_cmd_t*) GENERIC_RADIO_AppData.MsgPtr)->DeviceCfg);
                if (status == OS_SUCCESS)
                {
                    GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceCount++;
                }
                else
                {
                    GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceErrorCount++;
                }
            }
            break;

        /*
        ** Radio Proximity Forward Command
        ** Note that no verifications are performed prior to forwarding
        */
        case GNEERIC_RADIO_PROXIMITY_CC:
            prox_msg = ((CFE_SB_MsgPtr_t) ((GENERIC_RADIO_Proximity_cmd_t*) GENERIC_RADIO_AppData.MsgPtr)->Payload);
            status = GENERIC_RADIO_ProximityForward(&GENERIC_RADIO_AppData.ProxySocket, 
                ((GENERIC_RADIO_Proximity_cmd_t*) GENERIC_RADIO_AppData.MsgPtr)->SCID,
                ((GENERIC_RADIO_Proximity_cmd_t*) GENERIC_RADIO_AppData.MsgPtr)->Payload,
                CFE_SB_GetTotalMsgLength(prox_msg));
            if (status == OS_SUCCESS)
            {
                GENERIC_RADIO_AppData.HkTelemetryPkt.ForwardCount++;
            }
            else
            {
                GENERIC_RADIO_AppData.HkTelemetryPkt.ForwardErrorCount++;
            }
            break;


        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            GENERIC_RADIO_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_RADIO_CMD_ERR_EID, CFE_EVS_ERROR, 
                "GENERIC_RADIO: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", MsgId, CommandCode);
            break;
    }
    return;
} 


/*
** Process Telemetry Request - Triggered in response to a telemetery request
*/
void GENERIC_RADIO_ProcessTelemetryRequest(void)
{
    int32 status = OS_SUCCESS;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_SB_MsgId_t MsgId = CFE_SB_GetMsgId(GENERIC_RADIO_AppData.MsgPtr);   

    /* Pull this command code from the message and then process */
    uint16 CommandCode = CFE_SB_GetCmdCode(GENERIC_RADIO_AppData.MsgPtr);
    switch (CommandCode)
    {
        case GENERIC_RADIO_REQ_HK_TLM:
            GENERIC_RADIO_ReportHousekeeping();
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            GENERIC_RADIO_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_RADIO_DEVICE_TLM_ERR_EID, CFE_EVS_ERROR, 
                "GENERIC_RADIO: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", MsgId, CommandCode);
            break;
    }
    return;
}


/* 
** Report Application Housekeeping
*/
void GENERIC_RADIO_ReportHousekeeping(void)
{
    int32 status = OS_SUCCESS;

    status = GENERIC_RADIO_RequestHK(&GENERIC_RADIO_AppData.RadioSocket, (GENERIC_RADIO_Device_HK_tlm_t*) &GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceHK);
    if (status == OS_SUCCESS)
    {
        GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceCount++;
    }
    else
    {
        GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(GENERIC_RADIO_REQ_HK_ERR_EID, CFE_EVS_ERROR, 
                "GENERIC_RADIO: Request device HK reported error %d", status);
    }

    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &GENERIC_RADIO_AppData.HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &GENERIC_RADIO_AppData.HkTelemetryPkt);
    return;
}


/*
** Reset all global counter variables
*/
void GENERIC_RADIO_ResetCounters(void)
{
    GENERIC_RADIO_AppData.HkTelemetryPkt.CommandErrorCount = 0;
    GENERIC_RADIO_AppData.HkTelemetryPkt.CommandCount = 0;
    GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceErrorCount = 0;
    GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceCount = 0;
    GENERIC_RADIO_AppData.HkTelemetryPkt.ForwardErrorCount = 0;
    GENERIC_RADIO_AppData.HkTelemetryPkt.ForwardCount = 0;
    return;
} 


/*
** Verify command packet length matches expected
*/
int32 GENERIC_RADIO_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 expected_length)
{     
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t msg_id = 0xFFFF;
    uint16 cmd_code = 0xFFFF;
    uint16 actual_length = CFE_SB_GetTotalMsgLength(msg);

    if (expected_length == actual_length)
    {
        /* Increment the command counter upon receipt of an invalid command */
        GENERIC_RADIO_AppData.HkTelemetryPkt.CommandCount++;
    }
    else
    {
        msg_id = CFE_SB_GetMsgId(msg);
        cmd_code = CFE_SB_GetCmdCode(msg);

        CFE_EVS_SendEvent(GENERIC_RADIO_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              msg_id, cmd_code, actual_length, expected_length);

        status = OS_ERROR;

        /* Increment the command error counter upon receipt of an invalid command */
        GENERIC_RADIO_AppData.HkTelemetryPkt.CommandErrorCount++;
    }
    return status;
} 

/*
** Proxy Read Task
*/
int32 GENERIC_RADIO_ProxyTask(void)
{
    int32 status = OS_SUCCESS;
    uint8 read_data[GENERIC_RADIO_CFG_PROX_SIZE] = {0};
    size_t bytes = 0;

    /*
    ** Register the device task with Executive Services
    */
    status = CFE_ES_RegisterChildTask();
    if(status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_RADIO_TASK_REG_ERR_EID, CFE_EVS_ERROR, "GENERIC_RADIO: Register device task error %d", status);
        CFE_ES_ExitChildTask();
        return status;
    }
    else
    {
        CFE_EVS_SendEvent(GENERIC_RADIO_TASK_REG_INF_EID, CFE_EVS_INFORMATION, "GENERIC_RADIO: Device task registration complete");
    }

    /*
    ** Device Run Loop
    */
    while (CFE_ES_RunLoop(&GENERIC_RADIO_AppData.RunStatus) == TRUE)
    {
        /* Zero read data */
        CFE_PSP_MemSet(read_data, 0x00, GENERIC_RADIO_CFG_PROX_SIZE);

        /* Read */
        status = socket_recv(&GENERIC_RADIO_AppData.ProxySocket, read_data, GENERIC_RADIO_CFG_PROX_SIZE, &bytes);
        
            //#ifdef GENERIC_RADIO_CFG_DEBUG
                OS_printf("GENERIC_RADIO_ProxyTask reported status %d and received: ", status);
                for(int i = 0; i < (int) bytes; i++)
                {
                    OS_printf("0x%02x ", read_data[i]);
                }
                OS_printf("\n");
            //#endif
        
        if (status == OS_SUCCESS)
        {

            /* Publish on software bus assuming all received data is correctly formatted */
            CFE_SB_SendMsg((CFE_SB_Msg_t *) read_data);
        }

        /* Delay between loops */
        OS_TaskDelay(GENERIC_RADIO_DEVICE_MS_LOOP_DELAY);
    }

    return status;
}
