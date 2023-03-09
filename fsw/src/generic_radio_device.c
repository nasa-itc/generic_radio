/*******************************************************************************
** File: generic_radio_device.c
**
** Purpose:
**   This file contains the source code for the GENERIC_RADIO device.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_radio_device.h"


/* 
** Generic read data from device
*/
int32_t GENERIC_RADIO_ReadData(int32_t handle, uint8_t* read_data, uint8_t data_length)
{
    int32_t status = OS_SUCCESS;
    int32_t bytes = 0;
    int32_t bytes_available = 0;
    uint8_t ms_timeout_counter = 0;

    /* Wait until all data received or timeout occurs */
    bytes_available = uart_bytes_available(handle);
    while((bytes_available < data_length) && (ms_timeout_counter < GENERIC_RADIO_CFG_MS_TIMEOUT))
    {
        ms_timeout_counter++;
        OS_TaskDelay(1);
        bytes_available = uart_bytes_available(handle);
    }

    if (ms_timeout_counter < GENERIC_RADIO_CFG_MS_TIMEOUT)
    {
        /* Limit bytes available */
        if (bytes_available > data_length)
        {
            bytes_available = data_length;
        }
        
        /* Read data */
        bytes = uart_read_port(handle, read_data, bytes_available);
        if (bytes != bytes_available)
        {
            #ifdef GENERIC_RADIO_CFG_DEBUG
                OS_printf("  GENERIC_RADIO_ReadData: Bytes read != to requested! \n");
            #endif
            status = OS_ERROR;
        } /* uart_read */
    }
    else
    {
        status = OS_ERROR;
    } /* ms_timeout_counter */

    return status;
}


/* 
** Generic command to device
** Note that confirming the echoed response is specific to this implementation
*/
int32_t GENERIC_RADIO_CommandDevice(int32_t handle, uint8_t cmd_code, uint32_t payload)
{
    int32_t status = OS_SUCCESS;
    int32_t bytes = 0;
    uint8_t write_data[GENERIC_RADIO_DEVICE_CMD_SIZE] = {0};
    uint8_t read_data[GENERIC_RADIO_DEVICE_DATA_SIZE] = {0};

    payload = CFE_MAKE_BIG32(payload);

    /* Prepare command */
    write_data[0] = GENERIC_RADIO_DEVICE_HDR_0;
    write_data[1] = GENERIC_RADIO_DEVICE_HDR_1;
    write_data[2] = cmd_code;
    write_data[3] = payload >> 24;
    write_data[4] = payload >> 16;
    write_data[5] = payload >> 8;
    write_data[6] = payload;
    write_data[7] = GENERIC_RADIO_DEVICE_TRAILER_0;
    write_data[8] = GENERIC_RADIO_DEVICE_TRAILER_1;

    /* Flush any prior data */
    status = uart_flush(handle);
    if (status == UART_SUCCESS)
    {
        /* Write data */
        bytes = uart_write_port(handle, write_data, GENERIC_RADIO_DEVICE_CMD_SIZE);
        #ifdef GENERIC_RADIO_CFG_DEBUG
            OS_printf("  GENERIC_RADIO_CommandDevice[%d] = ", bytes);
            for (uint32_t i = 0; i < GENERIC_RADIO_DEVICE_CMD_SIZE; i++)
            {
                OS_printf("%02x", write_data[i]);
            }
            OS_printf("\n");
        #endif
        if (bytes == GENERIC_RADIO_DEVICE_CMD_SIZE)
        {
            status = GENERIC_RADIO_ReadData(handle, read_data, GENERIC_RADIO_DEVICE_CMD_SIZE);
            if (status == OS_SUCCESS)
            {
                /* Confirm echoed response */
                bytes = 0;
                while ((bytes < (int32_t) GENERIC_RADIO_DEVICE_CMD_SIZE) && (status == OS_SUCCESS))
                {
                    if (read_data[bytes] != write_data[bytes])
                    {
                        status = OS_ERROR;
                    }
                    bytes++;
                }
            } /* GENERIC_RADIO_ReadData */
            else
            {
                #ifdef GENERIC_RADIO_CFG_DEBUG
                    OS_printf("GENERIC_RADIO_CommandDevice - GENERIC_RADIO_ReadData returned %d \n", status);
                #endif
            }
        } 
        else
        {
            #ifdef GENERIC_RADIO_CFG_DEBUG
                OS_printf("GENERIC_RADIO_CommandDevice - uart_write_port returned %d, expected %d \n", bytes, GENERIC_RADIO_DEVICE_CMD_SIZE);
            #endif
        } /* uart_write */
    } /* uart_flush*/
    return status;
}


/*
** Request housekeeping command
*/
int32_t GENERIC_RADIO_RequestHK(int32_t handle, GENERIC_RADIO_Device_HK_tlm_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[GENERIC_RADIO_DEVICE_HK_SIZE] = {0};

    /* Command device to send HK */
    status = GENERIC_RADIO_CommandDevice(handle, GENERIC_RADIO_DEVICE_REQ_HK_CMD, 0);
    if (status == OS_SUCCESS)
    {
        /* Read HK data */
        status = GENERIC_RADIO_ReadData(handle, read_data, sizeof(read_data));
        if (status == OS_SUCCESS)
        {
            #ifdef GENERIC_RADIO_CFG_DEBUG
                OS_printf("  GENERIC_RADIO_RequestHK = ");
                for (uint32_t i = 0; i < sizeof(read_data); i++)
                {
                    OS_printf("%02x", read_data[i]);
                }
                OS_printf("\n");
            #endif

            /* Verify data header and trailer */
            if ((read_data[0]  == GENERIC_RADIO_DEVICE_HDR_0)     && 
                (read_data[1]  == GENERIC_RADIO_DEVICE_HDR_1)     && 
                (read_data[14] == GENERIC_RADIO_DEVICE_TRAILER_0) && 
                (read_data[15] == GENERIC_RADIO_DEVICE_TRAILER_1) )
            {
                data->DeviceCounter  = read_data[2] << 24;
                data->DeviceCounter |= read_data[3] << 16;
                data->DeviceCounter |= read_data[4] << 8;
                data->DeviceCounter |= read_data[5];

                data->DeviceConfig  = read_data[6] << 24;
                data->DeviceConfig |= read_data[7] << 16;
                data->DeviceConfig |= read_data[8] << 8;
                data->DeviceConfig |= read_data[9];

                data->DeviceStatus  = read_data[10] << 24;
                data->DeviceStatus |= read_data[11] << 16;
                data->DeviceStatus |= read_data[12] << 8;
                data->DeviceStatus |= read_data[13];

                #ifdef GENERIC_RADIO_CFG_DEBUG
                    OS_printf("  Header  = 0x%02x%02x  \n", read_data[0], read_data[1]);
                    OS_printf("  Counter = 0x%08x      \n", data->DeviceCounter);
                    OS_printf("  Config  = 0x%08x      \n", data->DeviceConfig);
                    OS_printf("  Status  = 0x%08x      \n", data->DeviceStatus);
                    OS_printf("  Trailer = 0x%02x%02x  \n", read_data[14], read_data[15]);
                #endif
            }
            else
            {
                #ifdef GENERIC_RADIO_CFG_DEBUG
                    OS_printf("  GENERIC_RADIO_RequestHK: GENERIC_RADIO_ReadData reported error %d \n", status);
                #endif 
                status = OS_ERROR;
            }
        } /* GENERIC_RADIO_ReadData */
    }
    else
    {
        #ifdef GENERIC_RADIO_CFG_DEBUG
            OS_printf("  GENERIC_RADIO_RequestHK: GENERIC_RADIO_CommandDevice reported error %d \n", status);
        #endif 
    }
    return status;
}


/*
** Request data command
*/
int32_t GENERIC_RADIO_RequestData(int32_t handle, GENERIC_RADIO_Device_Data_tlm_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[GENERIC_RADIO_DEVICE_DATA_SIZE] = {0};

    /* Command device to send HK */
    status = GENERIC_RADIO_CommandDevice(handle, GENERIC_RADIO_DEVICE_REQ_DATA_CMD, 0);
    if (status == OS_SUCCESS)
    {
        /* Read HK data */
        status = GENERIC_RADIO_ReadData(handle, read_data, sizeof(read_data));
        if (status == OS_SUCCESS)
        {
            #ifdef GENERIC_RADIO_CFG_DEBUG
                OS_printf("  GENERIC_RADIO_RequestData = ");
                for (uint32_t i = 0; i < sizeof(read_data); i++)
                {
                    OS_printf("%02x", read_data[i]);
                }
                OS_printf("\n");
            #endif

            /* Verify data header and trailer */
            if ((read_data[0]  == GENERIC_RADIO_DEVICE_HDR_0)     && 
                (read_data[1]  == GENERIC_RADIO_DEVICE_HDR_1)     && 
                (read_data[12] == GENERIC_RADIO_DEVICE_TRAILER_0) && 
                (read_data[13] == GENERIC_RADIO_DEVICE_TRAILER_1) )
            {
                data->DeviceCounter  = read_data[2] << 24;
                data->DeviceCounter |= read_data[3] << 16;
                data->DeviceCounter |= read_data[4] << 8;
                data->DeviceCounter |= read_data[5];

                data->DeviceDataX  = read_data[6] << 8;
                data->DeviceDataX |= read_data[7];

                data->DeviceDataY  = read_data[8] << 8;
                data->DeviceDataY |= read_data[9];
                
                data->DeviceDataZ  = read_data[10] << 8;
                data->DeviceDataZ |= read_data[11];

                #ifdef GENERIC_RADIO_CFG_DEBUG
                    OS_printf("  Header  = 0x%02x%02x  \n", read_data[0], read_data[1]);
                    OS_printf("  Counter = 0x%08x      \n", data->DeviceCounter);
                    OS_printf("  Data X  = 0x%04x, %d  \n", data->DeviceDataX, data->DeviceDataX);
                    OS_printf("  Data Y  = 0x%04x, %d  \n", data->DeviceDataY, data->DeviceDataY);
                    OS_printf("  Data Z  = 0x%04x, %d  \n", data->DeviceDataZ, data->DeviceDataZ);
                    OS_printf("  Trailer = 0x%02x%02x  \n", read_data[10], read_data[11]);
                #endif
            }
        } 
        else
        {
            #ifdef GENERIC_RADIO_CFG_DEBUG
                OS_printf("  GENERIC_RADIO_RequestData: Invalid data read! \n");
            #endif 
            status = OS_ERROR;
        } /* GENERIC_RADIO_ReadData */
    }
    else
    {
        #ifdef GENERIC_RADIO_CFG_DEBUG
            OS_printf("  GENERIC_RADIO_RequestData: GENERIC_RADIO_CommandDevice reported error %d \n", status);
        #endif 
    }
    return status;
}
