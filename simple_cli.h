#ifndef SIMPLE_CLI_H
#define SIMPLE_CLI_H

#include "stdlib.h"
#include "stdint.h"
/*Forward declaration*/
struct cli_command_t;
typedef struct cli_command_t cli_command_t; 


#define SIMCLI_MAX_CMD_LEN 128
#ifndef SIMCLI_MAX_COMMANDS 
    #define SIMCLI_MAX_COMMANDS 10
#endif
#define SIMCLI_MAX_ARGS 8



typedef struct
{
    char arg_name[10];
    uint8_t arg_type;
    void *value;
}cmd_arg_t;

typedef bool (*cmd_function_t)(char **argv, cli_command_t * self);

typedef struct cli_command_t
{
    char cmd_name[16];
    uint8_t args_num;
    cmd_arg_t args[SIMCLI_MAX_ARGS];
    cmd_function_t c_func;
    char cmd_info[64];


}cli_command_t;

typedef enum
{
    ARG_ONLY = 0,           /*Plain argument without values*/
    ARG_INT32,              /*Argument must be followed by int32 format value*/
    ARG_STRING              /*Argument must be followed by string format value*/
}arg_type_t;


uint8_t checkAllowedCharacters(const char* str);

bool ParseCmdArgs(char **argv, cli_command_t* self);

uint8_t AddNewCommand(cli_command_t new_command);

int8_t ProcessCommand(const char* input_str);



#endif