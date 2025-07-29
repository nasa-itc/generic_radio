/*******************************************************************************
** File: generic_radio_checkout.c
**
** Purpose:
**   This checkout can be run without cFS and is used to quickly develop and
**   test functions required for a specific component.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_radio_checkout.h"

/*
** Global Variables
*/
uart_info_t                     Generic_radioUart;
GENERIC_RADIO_Device_HK_tlm_t   Generic_radioHK;
GENERIC_RADIO_Device_Data_tlm_t Generic_radioData;

/*
** Component Functions
*/
void print_help(void)
{
    printf(PROMPT "command [args]\n"
                  "---------------------------------------------------------------------\n"
                  "help                               - Display help                    \n"
                  "exit                               - Exit app                        \n"
                  "noop                               - No operation command to device  \n"
                  "  n                                - ^                               \n"
                  "hk                                 - Request device housekeeping     \n"
                  "  h                                - ^                               \n"
                  "generic_radio                             - Request generic_radio data  "
                  "           \n"
                  "  s                                - ^                               \n"
                  "cfg #                              - Send configuration #            \n"
                  "  c #                              - ^                               \n"
                  "\n");
}

int get_command(const char *str)
{
    int  status = CMD_UNKNOWN;
    char lcmd[MAX_INPUT_TOKEN_SIZE];
    strncpy(lcmd, str, MAX_INPUT_TOKEN_SIZE);

    /* Convert command to lower case */
    to_lower(lcmd);

    if (strcmp(lcmd, "help") == 0)
    {
        status = CMD_HELP;
    }
    else if (strcmp(lcmd, "exit") == 0)
    {
        status = CMD_EXIT;
    }
    else if (strcmp(lcmd, "noop") == 0)
    {
        status = CMD_NOOP;
    }
    else if (strcmp(lcmd, "n") == 0)
    {
        status = CMD_NOOP;
    }
    else if (strcmp(lcmd, "hk") == 0)
    {
        status = CMD_HK;
    }
    else if (strcmp(lcmd, "h") == 0)
    {
        status = CMD_HK;
    }
    else if (strcmp(lcmd, "generic_radio") == 0)
    {
        status = CMD_GENERIC_RADIO;
    }
    else if (strcmp(lcmd, "s") == 0)
    {
        status = CMD_GENERIC_RADIO;
    }
    else if (strcmp(lcmd, "cfg") == 0)
    {
        status = CMD_CFG;
    }
    else if (strcmp(lcmd, "c") == 0)
    {
        status = CMD_CFG;
    }
    return status;
}

int process_command(int cc, int num_tokens, char *tokens)
{
    int32_t status      = OS_SUCCESS;
    int32_t exit_status = OS_SUCCESS;
    int     config;

    /* Process command */
    switch (cc)
    {
        case CMD_HELP:
            print_help();
            break;

        case CMD_EXIT:
            exit_status = OS_ERROR;
            break;

        case CMD_NOOP:
            if (check_number_arguments(num_tokens, 1) == OS_SUCCESS)
            {
                status = GENERIC_RADIO_CommandDevice(Generic_radioUart.handle, GENERIC_RADIO_DEVICE_NOOP_CMD, 0);
                if (status == OS_SUCCESS)
                {
                    OS_printf("NOOP command success\n");
                }
                else
                {
                    OS_printf("NOOP command failed!\n");
                }
            }
            break;

        case CMD_HK:
            if (check_number_arguments(num_tokens, 1) == OS_SUCCESS)
            {
                status = GENERIC_RADIO_RequestHK(Generic_radioUart.handle, &Generic_radioHK);
                if (status == OS_SUCCESS)
                {
                    OS_printf("GENERIC_RADIO_RequestHK command success\n");
                }
                else
                {
                    OS_printf("GENERIC_RADIO_RequestHK command failed!\n");
                }
            }
            break;

        case CMD_GENERIC_RADIO:
            if (check_number_arguments(num_tokens, 1) == OS_SUCCESS)
            {
                status = GENERIC_RADIO_RequestData(Generic_radioUart.handle, &Generic_radioData);
                if (status == OS_SUCCESS)
                {
                    OS_printf("GENERIC_RADIO_RequestHK command success\n");
                }
                else
                {
                    OS_printf("GENERIC_RADIO_RequestHK command failed!\n");
                }
            }
            break;

        case CMD_CFG:
            if (check_number_arguments(num_tokens, 1) == OS_SUCCESS)
            {
                config = atoi(&tokens[0]);
                status = GENERIC_RADIO_CommandDevice(Generic_radioUart.handle, GENERIC_RADIO_DEVICE_CFG_CMD, config);
                if (status == OS_SUCCESS)
                {
                    OS_printf("Configuration command success with value %d\n", config);
                }
                else
                {
                    OS_printf("Configuration command failed!\n");
                }
            }
            break;

        default:
            OS_printf("Invalid command format, type 'help' for more info\n");
            break;
    }
    return exit_status;
}

int main(int argc, char *argv[])
{
    int     status = OS_SUCCESS;
    char    input_buf[MAX_INPUT_BUF];
    char    input_tokens[MAX_INPUT_TOKENS][MAX_INPUT_TOKEN_SIZE];
    int     num_input_tokens;
    int     cmd;
    char   *token_ptr;
    uint8_t run_status = OS_SUCCESS;

    /* Open device specific protocols */
    Generic_radioUart.deviceString = GENERIC_RADIO_CFG_STRING;
    Generic_radioUart.handle       = GENERIC_RADIO_CFG_HANDLE;
    Generic_radioUart.isOpen       = PORT_CLOSED;
    Generic_radioUart.baud         = GENERIC_RADIO_CFG_BAUDRATE_HZ;
    status                         = uart_init_port(&Generic_radioUart);
    if (status == OS_SUCCESS)
    {
        printf("UART device %s configured with baudrate %d \n", Generic_radioUart.deviceString, Generic_radioUart.baud);
    }
    else
    {
        printf("UART device %s failed to initialize! \n", Generic_radioUart.deviceString);
        run_status = OS_ERROR;
    }

    /* Main loop */
    print_help();
    while (run_status == OS_SUCCESS)
    {
        num_input_tokens = -1;
        cmd              = CMD_UNKNOWN;

        /* Read user input */
        printf(PROMPT);
        fgets(input_buf, MAX_INPUT_BUF, stdin);

        /* Tokenize line buffer */
        token_ptr = strtok(input_buf, " \t\n");
        while ((num_input_tokens < MAX_INPUT_TOKENS) && (token_ptr != NULL))
        {
            if (num_input_tokens == -1)
            {
                /* First token is command */
                cmd = get_command(token_ptr);
            }
            else
            {
                strncpy(input_tokens[num_input_tokens], token_ptr, MAX_INPUT_TOKEN_SIZE);
            }
            token_ptr = strtok(NULL, " \t\n");
            num_input_tokens++;
        }

        /* Process command if valid */
        if (num_input_tokens >= 0)
        {
            /* Process command */
            run_status = process_command(cmd, num_input_tokens, token_ptr);
        }
    }

    // Close the device
    uart_close_port(Generic_radioUart.handle);

    OS_printf("Cleanly exiting generic_radio application...\n\n");
    return 1;
}

/*
** Generic Functions
*/
int check_number_arguments(int actual, int expected)
{
    int status = OS_SUCCESS;
    if (actual != expected)
    {
        status = OS_ERROR;
        OS_printf("Invalid command format, type 'help' for more info\n");
    }
    return status;
}

void to_lower(char *str)
{
    char *ptr = str;
    while (*ptr)
    {
        *ptr = tolower((unsigned char)*ptr);
        ptr++;
    }
    return;
}
