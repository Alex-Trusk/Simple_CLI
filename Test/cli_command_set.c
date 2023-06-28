#include "string.h"
#include "stdbool.h"
#include "simple_cli.h"
#include "stdio.h"

static const uint8_t minAsciiArr[] = {33, 35, 45, 65, 97};      // Array of min values of ASCII codes
static const uint8_t maxAsciiArr[] = {33, 41, 57, 90, 122};     // Array of max values of ASCII codes
static const int numRanges = 5;                         		// Number of ranges in arrays
static size_t file_size_global;
#ifndef UNUSED_PARAMETER
	#define UNUSED_PARAMETER(x)    ((void)(x))
#endif
uint8_t checkAllowedCharacters(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        bool found = false;
        
        for (int j = 0; j < numRanges; j++) {
            if (str[i] >= minAsciiArr[j] && str[i] <= maxAsciiArr[j]) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            return false;
        }
    }
    
    return true;
}

uint8_t test_write_buffered(char* data,uint16_t length)
{
	static size_t bytesWritten=0;
	if(data==NULL)
		return 0;
	if((bytesWritten+length)<file_size_global)
	{
		printf("%d bytes have been written\n", length);				/*Just for test purposes*/
		bytesWritten+=length;
		return 1;
	}
	else
	{
		printf("%d bytes have been written\n", length);	
		bytesWritten=0;
		return 0;
	}
	
	
}

void SDC_drv_fopen(const char *file_name, uint8_t attributes, size_t file_size)
{
	file_size_global=file_size;
	UNUSED_PARAMETER(file_name);
	UNUSED_PARAMETER(attributes);
}

/**
 * @brief This command is used to begin file sending sequence. 
 * The arguments are: 	-n - size of file in bytes,
 * 						-f - file name.
 * If the command received without arguments default value are used.
 * Command acquires context and controls data flow to write incoming information
 * to file on disk. For testing purposes file writing stage is ignored.
 * 
 */
bool sendfile_cmd(char **argv, cli_command_t* self, CliContextManager_t * _context)
{
	sim_cli_error ret_err;
	const char *err_msg_list[]={	"Unknown argument\n" ,
									"Bad argument value\n",
									"Missing argument value\n",
									"Restricted character in filename\n"};
    /*default values*/
    uint32_t file_size=1024;
    uint8_t overwrite_flag=0;
    char file_name[32]= "Default";
    /*Here we MUST initialize  ALL possible command attributes with therefore used addresses of variables*/
    self->args[0].value=&file_size;
    self->args[1].value=&overwrite_flag;
    self->args[2].value=file_name;
	ret_err= ParseCmdArgs(argv,self);		//main part of all command functions

	switch (ret_err)
	{
			case SIM_CLI_OK:
				break;
			case SIM_CLI_ARG_UNKNOWN:
				_context->stdoutFunc(err_msg_list[0],strlen(err_msg_list[0]));
				return false;
			case SIM_CLI_ARG_BAD_VALUE:
				_context->stdoutFunc(err_msg_list[1],strlen(err_msg_list[1]));
				return false;
			case SIM_CLI_ARG_MISSING_VALUE:
				_context->stdoutFunc(err_msg_list[2],strlen(err_msg_list[2]));
				return false;
			default:
				break;
	}
	
    if(!checkAllowedCharacters(file_name))
    {
		printf("File name should contain only allowed character\n");
		_context->stdoutFunc(err_msg_list[3],strlen(err_msg_list[3]));
        return false;
    }
	
	if(file_size== 1024)
		{printf("Will open file named %s. Size is not set\n",file_name);}
	else
		{printf("Will open file named %s with the size of %u bytes\n",file_name,file_size);}
    
	/*!!! Data flow management transferred to sendfile command context*/		
	if(!AcquireContext(_context,&self->cmd_context))					
	{
		printf("sendfile_cmd: Context not acquired!\n");	
		return false;
	}
	printf("Context acquired\n");
	SDC_drv_fopen(file_name, 0,file_size);	/*Simulating file open procedure*/
	return true;
}

/**
 * @brief This is a context handler function of sendfile command.
 * It manages all incoming information from data flow. After receiving determined length 
 * context handler stops its work and release context.
 */
bool sendfile_context_handler(char *data, size_t length,void * _context)
{
	/*Checking for end line sequence.*/
	if((*(data+length-1)==0x00)&&(*(data+length-2)==0x0A)&&(*(data+length-3)==0x0D))
		length-=3;
	
	if (test_write_buffered(data,length)==true)
	{
		return true;
	}
	else
	{
		file_size_global=0;
		ReleaseContext(_context);
		printf("Context released\n");
		return false;
	}
}


bool SDC_driver_mount()
{
	/*SD card mount simulation*/
	return true;
}
bool mountSD_cmd(char **argv, cli_command_t* self, CliContextManager_t * _context)
{
	const char err_arg_msg[]	=	"Command doesn't have arguments\n";
	const char err_SD_fail_msg[]=	"Mount failed\n";
	const char err_SD_pass_msg[]=	"Mount success\n";
	if ((argv[0])&&(!self->args_num)) /*Checking if there any arguments are passed*/
    {
        printf("%s",err_arg_msg);
		_context->stdoutFunc(err_arg_msg,strlen(err_arg_msg));
        return false;
    }
	if(SDC_driver_mount())
	{
		_context->stdoutFunc(err_SD_pass_msg,strlen(err_SD_pass_msg));
		return 1;
	}
	else
	{
		_context->stdoutFunc(err_SD_fail_msg,strlen(err_SD_fail_msg));
		return 0;
	}
}

void initSimpleCliSet(void)
{
	/* First command*/
	Context_t s_f_context;
	s_f_context.context_handler=sendfile_context_handler;
	cli_command_t sendfile=
    {
        .cmd_name = "sendfile",
        .args_num = 3,
        .args = {   {.arg_name="-n", .arg_type=ARG_INT32    }, 				// args[0]
                    {.arg_name="-o", .arg_type=ARG_ONLY     }, 				// args[1]
                    {.arg_name="-f", .arg_type=ARG_STRING, .value=NULL   }  // args[2]
                },
        .c_func = sendfile_cmd,
        .cmd_info = "Sends file over UART",
        .cmd_ID= 0x01,
		.cmd_context = s_f_context					/*Command with context handler*/
    };
    AddNewCommand(sendfile);
    
	/*Second command*/
    cli_command_t mountSD =
    {
        .cmd_name = "mountsd",
        .args_num = 0,
        .c_func = mountSD_cmd,
        .cmd_info = "Initializes SD card interface",
        .cmd_ID= 0x02,
		.cmd_context={{0}}					/*Command without context handler. Double brackets for GCC compiler (GCC bug # 53119)  */
    };
    AddNewCommand(mountSD);
}
