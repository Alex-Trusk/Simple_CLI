#include "string.h"
#include "simple_cli.h"

static uint8_t NumCommandsStored = 0;
cli_command_t cli_command_list [SIMCLI_MAX_COMMANDS];

#ifndef UNUSED_PARAMETER
	#define UNUSED_PARAMETER(X)  ((void)(X))
#endif

#define CLI_CHECK_NULL(_ret_code)   	\
	if(_ret_code==0)					\
		return false;					

char* GetNextArgument(char* input_string, const char* delimiter)
{
    return strtok(input_string, delimiter); // Tokenize using delimiter
}


sim_cli_error ParseCmdArgs(char **argv, cli_command_t* self)
{
    uint8_t arg_num=0;
    bool arg_found;
    while(argv[arg_num])
    {
        arg_found=0;
        for(int i=0;i<self->args_num;++i)
        {
            if(strcmp(argv[arg_num],self->args[i].arg_name)==0)
            {
                arg_found=1;
                switch (self->args[i].arg_type)
                {
                    case ARG_ONLY:
                        *(uint8_t*)(self->args[i].value)=1;
                        arg_num+=1;
                        break;
                    case ARG_STRING:
                        if (argv[arg_num+1]&&(*argv[arg_num+1]!='-'))
                        {
							memcpy((uint32_t*)self->args[i].value,argv[arg_num+1],strlen(argv[arg_num+1])+1);
                            arg_num+=2;
                        }
                        else
                        {
                            			//TODO: debug message
							return SIM_CLI_ARG_MISSING_VALUE;
                        }
                        break;
                    case ARG_INT32:
                        if (argv[arg_num+1])
                            {
                            char* endptr;
                            int32_t arg_val=strtol(argv[arg_num+1],&endptr,10);
                            if((argv[arg_num+1])==endptr)
                            {
                                			//TODO: debug message
								return SIM_CLI_ARG_BAD_VALUE;
                            }
                            *(int32_t*)(self->args[i].value)=arg_val;
                            arg_num+=2;
                            }
                        break; //exit switch
                }    
            break;  //exit for cycle
            }
        }
        if(!arg_found)
        {
			//TODO: debug message
            return SIM_CLI_ARG_UNKNOWN;
        }
    }
    return SIM_CLI_OK;
}

uint8_t AddNewCommand(cli_command_t new_command)
{
    if(FindCmdByID(new_command.cmd_ID)||(new_command.c_func==NULL)){
        return 0;}
	if(NumCommandsStored>=SIMCLI_MAX_COMMANDS)
		return 0;
    cli_command_list[NumCommandsStored++]=new_command;
    return ++NumCommandsStored;
}

cli_command_t* FindCmd(char* cmd_name)
{
    for(int i =0; i<NumCommandsStored;++i)
    {
        if(strcmp(cli_command_list[i].cmd_name, cmd_name)==0)
            return &cli_command_list[i];
    }
    return NULL;
}

int8_t ProcessCommand(const char* input_str, CliContextManager_t * _context)
{
    char * token=NULL;
    char *arg_list[SIMCLI_MAX_ARGS+2];
	cli_command_t *command;
#if (USE_STATIC_ALLOCATION==1)
	char* duplicate_str[SIMCLI_MAX_CMD_LEN];
#else
    memset(arg_list,0,sizeof(arg_list));
	char* duplicate_str=malloc(strlen(input_str)+2);
#endif
    strcpy(duplicate_str,input_str);
	
    char * end_pos = strchr(duplicate_str, '\0');
    if(*(end_pos-1)=='\n'||*(end_pos-1)=='\r')
	{
		*(end_pos-1)='\0';
		if(*(end_pos-2)=='\n'||*(end_pos-2)=='\r')
			*(end_pos-2)='\0';
	}
	
    token =  GetNextArgument(duplicate_str,SIMCLI_ARGS_DELIMITER);
    if(token)
    {
        command=FindCmd(token);								/*Looking for element in list of stored commands*/
        if(command)
        {
            			//TODO: debug message
			int i=0;
            token = GetNextArgument(NULL," ");
            while(token)
            {
                arg_list[i] = token;
                ++i;
            token = GetNextArgument(NULL,SIMCLI_ARGS_DELIMITER);
            }
            if(command->c_func(arg_list,command,_context))  /*Calling command function*/
			{
#if (USE_STATIC_ALLOCATION==0)		
				free (duplicate_str);
#endif
                return (int8_t)command->cmd_ID;
			}
        }
    }

#if (USE_STATIC_ALLOCATION==0)
    free (duplicate_str);
#endif
    return 0;
}

cli_command_t* FindCmdByID(uint8_t cmdID)
{
    for(int i = 0; i<NumCommandsStored;++i)
        if(cli_command_list[i].cmd_ID==cmdID)
            return &cli_command_list[i];
    return NULL;
}

Context_t * PullContextStack(context_stack_t *_stack)
{
	CLI_CHECK_NULL(_stack);
	CLI_CHECK_NULL(_stack->currentSize);
	return _stack->stackbuf[_stack->currentSize--];
}

bool ReleaseContext( CliContextManager_t *CLI_cont)
{
	
	CLI_CHECK_NULL(CLI_cont);
	CLI_CHECK_NULL(CLI_cont->ParentOwner);
	CLI_cont->context_level-=1;
	CLI_cont->contextOwner=CLI_cont->ParentOwner;
	CLI_cont->ParentOwner=PullContextStack(&CLI_cont->CallStack);
				//TODO: debug message
	return true;
}

uint32_t default_stdout(const char *data, size_t length)
{
	UNUSED_PARAMETER(data);
	UNUSED_PARAMETER(length);
	return 0;
}

bool InitCLIcontext( CliContextManager_t *ctrl_context_ptr,context_handler_f handlerfunc, stdout_f stdout_func, char* context_name)
{
	CLI_CHECK_NULL(ctrl_context_ptr);
    CLI_CHECK_NULL(context_name);

	strcpy(ctrl_context_ptr->contextOwner->Name,context_name);
	ctrl_context_ptr->contextOwner->context_handler=handlerfunc;
	ctrl_context_ptr->CallStack.currentSize=0;
	ctrl_context_ptr->ParentOwner=NULL;
	ctrl_context_ptr->context_level=0;
	if(stdout_func)
		ctrl_context_ptr->stdoutFunc=stdout_func;
	else
		ctrl_context_ptr->stdoutFunc=default_stdout;
	return true;
}

bool CallContextHandler(CliContextManager_t *ctrl_context_ptr, char *data, size_t length)
{
	CLI_CHECK_NULL(ctrl_context_ptr);
	CLI_CHECK_NULL(data);
	ctrl_context_ptr->contextOwner->context_handler(data,length, ctrl_context_ptr);
	return true;
}

bool PushContextStack(context_stack_t *_stack, Context_t *_context)
{
	if(_stack->currentSize+1>=CLI_STACK_SIZE)
		return false;
	CLI_CHECK_NULL(_stack);
	CLI_CHECK_NULL(_context);
	_stack->stackbuf[++_stack->currentSize]=_context;
	return true;
}

bool AcquireContext(CliContextManager_t *context_ptr, Context_t *_context)
{
	CLI_CHECK_NULL(context_ptr);
	CLI_CHECK_NULL(_context);
	PushContextStack(&context_ptr->CallStack,context_ptr->ParentOwner);
	context_ptr->ParentOwner=context_ptr->contextOwner;
	context_ptr->contextOwner=_context;
	context_ptr->context_level++;
	return true;
}

