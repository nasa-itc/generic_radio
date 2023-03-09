/************************************************************************
** File:
**    generic_radio_events.h
**
** Purpose:
**  Define GENERIC_RADIO application event IDs
**
*************************************************************************/

#ifndef _GENERIC_RADIO_EVENTS_H_
#define _GENERIC_RADIO_EVENTS_H_

/* Standard app event IDs */
#define GENERIC_RADIO_RESERVED_EID              0
#define GENERIC_RADIO_STARTUP_INF_EID           1
#define GENERIC_RADIO_LEN_ERR_EID               2
#define GENERIC_RADIO_PIPE_ERR_EID              3
#define GENERIC_RADIO_SUB_CMD_ERR_EID           4
#define GENERIC_RADIO_SUB_REQ_HK_ERR_EID        5
#define GENERIC_RADIO_PROCESS_CMD_ERR_EID       6

/* Standard command event IDs */
#define GENERIC_RADIO_CMD_ERR_EID               10
#define GENERIC_RADIO_CMD_NOOP_INF_EID          11
#define GENERIC_RADIO_CMD_RESET_INF_EID         12
#define GENERIC_RADIO_CMD_ENABLE_INF_EID        13
#define GENERIC_RADIO_ENABLE_INF_EID            14
#define GENERIC_RADIO_ENABLE_ERR_EID            15
#define GENERIC_RADIO_CMD_DISABLE_INF_EID       16
#define GENERIC_RADIO_DISABLE_INF_EID           17
#define GENERIC_RADIO_DISABLE_ERR_EID           18

/* Device specific command event IDs */
#define GENERIC_RADIO_CMD_CONFIG_INF_EID        20
#define GENERIC_RADIO_CONFIG_INF_EID            21
#define GENERIC_RADIO_CONFIG_ERR_EID            22

/* Standard telemetry event IDs */
#define GENERIC_RADIO_DEVICE_TLM_ERR_EID        30
#define GENERIC_RADIO_REQ_HK_ERR_EID            31

/* Device specific telemetry event IDs */
#define GENERIC_RADIO_REQ_DATA_ERR_EID          32

/* Hardware protocol event IDs */
#define GENERIC_RADIO_UART_INIT_ERR_EID         40
#define GENERIC_RADIO_UART_CLOSE_ERR_EID        41
#define GENERIC_RADIO_UART_READ_ERR_EID         42
#define GENERIC_RADIO_UART_WRITE_ERR_EID        43
#define GENERIC_RADIO_UART_TIMEOUT_ERR_EID      44

#endif /* _GENERIC_RADIO_EVENTS_H_ */
