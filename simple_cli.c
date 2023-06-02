#include "string.h"
#include "stdbool.h"
#include "simple_cli.h"

static uint8_t NumCommandsStored = 0;

char* GetNextToken(char* input_string, const char* delimiter)
{
    char* token = strtok(input_string, delimiter); // Tokenize using space as the delimiter
}

cli_command_t cli_command_list [SIMCLI_MAX_COMMANDS];
bool ParseCmdArgs(char **argv, cli_command_t* self)
{
    uint8_t arg_num=0;
    while(argv[arg_num])
    {
        bool arg_found=0;
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
                            memcpy(self->args[i].value,argv[arg_num+1],strlen(argv[arg_num+1]));
                            arg_num+=2;
                        }
                        else
                        {
                            printf("Missing value after %s\n", self->args[i].arg_name);
                            return false;
                        }
                        break;
                    case ARG_INT32:
                        if (argv[arg_num+1])
                            {
                            char* endprt;
                            int32_t arg_val=strtol(argv[arg_num+1],&endprt,10);
                            if((argv[arg_num+1])==endprt)
                            {
                                printf("Must be a correct integer value after %s\n", self->args[i].arg_name);
                                return false;
                            }
                            *(int32_t*)(self->args[i].value)=arg_val;
                            arg_num+=2;
                            }
                        break;
                }    
            }
        }
        if(!arg_found)
        {
            printf("Unknown argument\n");
            return false;
        }
    }
    return true;
}

uint8_t AddNewCommand(cli_command_t new_command)
{
    if(new_command.c_func==NULL)
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
int8_t ProcessCommand(const char* input_str)
{
    char * end_pos = strchr(input_str, '\n');
    char * token=NULL;
    char *arg_list[SIMCLI_MAX_ARGS+2];
    memset(arg_list,0,sizeof(arg_list));


    cli_command_t *command;
    if(!end_pos)
        return -1;
    *end_pos='\0';
    char* duplicate_str = strdup(input_str);   // new string must be freed
    token =  GetNextToken(duplicate_str," ");
    printf("cmd token = %s\n",token);
    if(token)
    {
        
        if(command=FindCmd(token))
        {
            printf("Command found\n");
            int i=0;
            while((token = GetNextToken(NULL," ")))
            {
                arg_list[i] = token;
                ++i;
            }
            command->c_func(arg_list,command);

        }
        else
        {
            printf("Command unknown\n");
        }

    }
    free (duplicate_str);
}
