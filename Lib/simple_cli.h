

/*
 * simple_cli.h
 *
 * Description: Brief description of the contents of the header file.
 *              This file is licensed under the MIT License.
 *              For more information, please refer to the LICENSE file.
 */

/*
 * MIT License
 *
 * Copyright (c) [2023] [Alex Trusk]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SIMPLE_CLI_H
#define SIMPLE_CLI_H


#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"

/*Forward declarations*/
struct cli_command_t;
typedef struct cli_command_t cli_command_s; 

#ifndef USE_STATIC_ALLOCATION                     		
    #define USE_STATIC_ALLOCATION 		0					/*Use static memory allocation only. Command length will be limited to SIMCLI_MAX_CMD_LEN*/
#endif

#if (USE_STATIC_ALLOCATION==0)
	#define SIMCLI_MAX_CMD_LEN 			128          	    /*Max length of a single command line with arguments*/
#endif

#ifndef SIMCLI_MAX_COMMANDS                     			/*Max commands number that can be is your system*/
    #define SIMCLI_MAX_COMMANDS 		10
#endif

#define SIMCLI_MAX_ARGS 				8              		/*Max number of arguments in a single command*/
#define CLI_STACK_SIZE  				4					/*Max number of CLI context levels*/

#define SIMCLI_ARGS_DELIMITER 			" "					/*Symbols that separate arguments in command line*/

#define SIMPLE_CLI_DEF(_name)                           \
    static Context_t _context_;							\
	static CliContextManager_t _name = {		       		\
							.contextOwner=&_context_}				

	
/**
 * @brief List of possible types of arguments value
 * 
 */
typedef enum
{
      ARG_ONLY = 0         			/*Plain argument without values*/
    , ARG_INT32           			/*Argument must be followed by int32 format value*/
    , ARG_STRING     				/*Argument must be followed by string format value*/
}arg_type_t;

/**
 * @brief Simple CLI error return values
 * 
 */
typedef enum
{
	  SIM_CLI_OK =0					/*Success*/
	, SIM_CLI_ARG_UNKNOWN			/*Parsed argument not from command args list*/
	, SIM_CLI_ARG_BAD_VALUE			/*Bad value is passed after argument*/
	, SIM_CLI_ARG_MISSING_VALUE		/*No value after argument*/
}sim_cli_error;

/**
 * @brief Contains info about command argument
 * 
 */
typedef struct
{
    char 		arg_name[10];       /*Name of the argument: Ex: -n or -help*/
    arg_type_t 	arg_type;        	/*Type of the value that expected to be after argument. */
    void* 		value;            	/*Place for the pointer to command argument value.*/
}cmd_arg_t;

/**
 * @brief Context handler function
 * @param data 		Pointer to data array
 * @param length 	Number of bytes in data array
 * @param _context 	Pointer to CliContextManager_t object
 * @return True - OK, False - context handler failed
*/
typedef bool (*context_handler_f)(char *data, size_t length, void * _context);

/**
* @brief Context handler structure
*/ 
typedef struct 
{
	char 				Name[16];			/*Context handler name*/
	context_handler_f 	context_handler;	/*Context handler function*/
}Context_t;

/**
* @brief Context handlers stack. Used to save context switching operations
*/
typedef struct
{
	uint8_t 	currentSize;					/*Number of elements in stack*/
	Context_t* 	stackbuf[CLI_STACK_SIZE];		/*Array of saved context_t-type pointers*/
}context_stack_t;

typedef uint32_t (*stdout_f)(const char *data, size_t length);


/**
* @brief Context controller stucture
*/
typedef struct 
{
	uint8_t 		context_level;   	/*Current level in context stack*/
	Context_t* 		contextOwner;		/*Current context owner*/
	Context_t*		ParentOwner;		/*Pointer to higher level context handler*/ 
	context_stack_t	CallStack;			/*Context handlers stack*/
	stdout_f		stdoutFunc;			/*Function that handles stdout data transfer*/
}CliContextManager_t;

/**
 * @brief Command function. 
 * @param argv 		Pointer to (char*) array containing parsed arguments
 * @param self 		Pointer to cli_command_t struct of current command
 * @param _context 	Pointer to CliContextManager_t object
 * @return True - OK, False - command run error
*/
typedef bool (*cmd_function_t)(char **argv, cli_command_s * self,CliContextManager_t * _context);

/**
 * @brief Describes each CLI command 
 * 
 */
typedef struct cli_command_t
{
    char 			cmd_name[16];                      	/*Name of the command.*/
    uint8_t 		args_num;                       	/*Actual number of possible arguments that are stored in args[] array */
    cmd_arg_t 		args[SIMCLI_MAX_ARGS];        		/*Array containing all function arguments*/
    cmd_function_t 	c_func;                  			/*Pointer to function thar will be called*/
    char 			cmd_info[64];                      	/*Text description of the command*/
    uint8_t 		cmd_ID;                         	/*Command ID. 1..255. Should be an unique value.*/
	Context_t 		cmd_context;						/*Associated data flow context. Can be NULL if command doesn't handle data flow*/

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
sim_cli_error ParseCmdArgs(char **argv, cli_command_t* self);


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
int8_t ProcessCommand(const char* input_str,CliContextManager_t * _context);


/**
 * @brief Find command by its ID in the list
 * 
 *
 * @param cmdID [in] Command ID
 * @return Pointer to cli_command_t object, or NULL if ID not found
 */
cli_command_t* FindCmdByID(uint8_t cmdID);


/**
 * @brief Calling this func running command receives data flow context and manages all incoming data
 * 
 *
 * @param context_ptr[in] Pointer to CLI context control object
 * @param context_ptr[in] Pointer to context object that will manage the data flow
 * @return True - context handling transferred, False - Context transfer failed. Bad arguments are passed
 */
bool AcquireContext(CliContextManager_t *context_ptr, Context_t *_context);


/**
 * @brief Calling this func running command releases data flow context. 
 *       Context is transferred to previous context manager which managed data flow before AcquireContext() was called
 * 
 * @param CLI_cont[in] Pointer to CLI context control object
 * @return True - context handling transferred, False - Context transfer failed. Bad argument
 */
bool ReleaseContext(CliContextManager_t *CLI_cont);


/**
 * @brief Initializes context manager. 
 *
 * 
 * @param ctrl_context_ptr[in,out] 	Pointer to CLI context control object. Must be allocated before the function call
 * @param handlerfunc[in] 			Pointer to main context handler function
 * @param stdout_func[in] 			Pointer to function that handles stdout data transfer
 * @param context_name[in] 			Pointer to function that handles stdout data transfer
 * @return True - success, False - Invalid arguments
 */
bool InitCLIcontext(CliContextManager_t* ctrl_context_ptr,context_handler_f handlerfunc, stdout_f stdout_func, char* context_name);


/**
 * @brief Calls current context manager function
 *
 * @param ctrl_context_ptr[in] 	Pointer to CLI context control object. Must be initialized using InitCLIcontext()
 * @param data 					Pointer to data array
 * @param length 				Number of bytes in data array
 * @return True - success, False - invalid arguments
 */
bool CallContextHandler(CliContextManager_t *ctrl_context_ptr, char *data, size_t length);

#endif
