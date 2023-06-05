#ifndef SIMPLE_CLI_H
#define SIMPLE_CLI_H

#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"
/*Forward declaration*/
struct cli_command_t;
typedef struct cli_command_t cli_command_s; 


#define SIMCLI_MAX_CMD_LEN 128                  /*Max length of a single command */
#ifndef SIMCLI_MAX_COMMANDS                     /*Max command \number that can be is you system*/
    #define SIMCLI_MAX_COMMANDS 10
#endif
#define SIMCLI_MAX_ARGS 8                       /*Max number of arguments in a single command*/

/**
 * @brief List of possible types of value of argument
 * 
 */
typedef enum
{
    ARG_ONLY = 0,           /*Plain argument without values*/
    ARG_INT32,              /*Argument must be followed by int32 format value*/
    ARG_STRING              /*Argument must be followed by string format value*/
}arg_type_t;

/**
 * @brief Contains info about argument
 * 
 */
typedef struct
{
    char arg_name[10];          /*Name of the argument: Ex: -n or -help*/
    arg_type_t arg_type;        /*Type of the value that expected to be after argument. */
    void *value;                /*Place for the pointer to command argument value.*/
}cmd_arg_t;

/**
 * @brief Command function type 
 * 
 */
typedef bool (*cmd_function_t)(char **argv, cli_command_s * self);

/**
 * @brief Describes CLI command 
 * 
 */
typedef struct cli_command_t
{
    char cmd_name[16];                      /*Name of the command.*/
    uint8_t args_num;                       /*Actual number of possible arguments that are stored in args[] array */
    cmd_arg_t args[SIMCLI_MAX_ARGS];        /*Array containing all function arguments*/
    cmd_function_t c_func;                  /*Pointer to function thar will be called*/
    char cmd_info[64];                      /*Text description of the command*/
    uint8_t cmd_ID;                         /*Command ID. 1..255. Should be an unique value.*/


}cli_command_t;

/**
 * @brief Parsing received arguments in command. Can be call in 
 * command function to set arguments.
 *
 * @param argv  [in] List of received arguments
 * @param self  [in] Pointer to current command object
 * @retval      true - all arguments parsed successfully 
 * @retval      false - error parsing 
 */
bool ParseCmdArgs(char **argv, cli_command_t* self);

/**
 * @brief Interface for adding new command to system list
 * The whole cli_command_t object will be copied 
 * 
 *
 * @param new_command [in] Command description to add
 * @return 0 - new_command doesn't have command function instance assigned.
 * 1 - 255 - the number of commands in the list
 */
uint8_t AddNewCommand(cli_command_t new_command);

/**
 * @brief Command parsing function.
 * 
 *
 * @param input_str [i] Pointer to string with command name and arguments
 * @return #ID of executed command, 0 - if error
 */
int8_t ProcessCommand(const char* input_str);

/**
 * @brief Find command by its ID
 * 
 *
 * @param cmdID [in] Command ID
 * @return Pointer to cli_comand_t object, or NULL if ID not found
 */
cli_command_t* FindCmdByID(uint8_t cmdID);


#endif