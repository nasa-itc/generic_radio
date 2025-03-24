/*******************************************************************************
** File: generic_radio_checkout.h
**
** Purpose:
**   This is the header file for the GENERIC_RADIO checkout.
**
*******************************************************************************/
#ifndef _GENERIC_RADIO_CHECKOUT_H_
#define _GENERIC_RADIO_CHECKOUT_H_

/*
** Includes
*/
#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "device_cfg.h"
#include "generic_radio_device.h"
#include "hwlib.h"

#if TGTNAME == cpu1
#include "nos_link.h"
#endif

/*
** Standard Defines
*/
#define PROMPT "generic_radio> "
#define MAX_INPUT_BUF 512
#define MAX_INPUT_TOKENS 64
#define MAX_INPUT_TOKEN_SIZE 50
#define TELEM_BUF_LEN 8

/*
** Command Defines
*/
#define CMD_UNKNOWN -1
#define CMD_HELP 0
#define CMD_EXIT 1
#define CMD_HK 3
#define CMD_PROX_FORWARD 4
#define CMD_CFG 5

/*
** Prototypes
*/
void print_help(void);
int get_command(const char *str);
int main(int argc, char *argv[]);

/*
** Generic Prototypes
*/
int check_number_arguments(int actual, int expected);
void to_lower(char *str);

#endif /* _GENERIC_RADIO_CHECKOUT_H_ */
