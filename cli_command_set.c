#include "string.h"
#include "stdbool.h"
#include "simple_cli.h"
#include "stdio.h"

static const uint8_t minAsciiArr[] = {33, 35, 45, 65, 97};      // Массив минимальных значений ASCII-кодов (пробел, A, a)
static const uint8_t maxAsciiArr[] = {33, 41, 57, 90, 122};     // Массив максимальных значений ASCII-кодов (~, Z, z)
static const int numRanges = 5;                          // Количество разрешенных диапазонов символов

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
bool sendfile_cmd(char **argv, cli_command_t* self)
{
    if ((argv[0])&&(!self->args_num))
    {
        printf("Command doesn't have arguments\n");
        return false;
    }
    /*default values*/
    uint32_t file_size=1024;
    uint8_t overwrite_flag=0;
    char file_name[32]="Default";

    /*Here we MUST initialize  ALL the command parameters with therefore used var addresses */
    self->args[0].value=&file_size;
    self->args[1].value=&overwrite_flag;
    self->args[2].value=&file_name[0];
    if(!ParseCmdArgs(argv,self))            //main part of all command functions
        return false;
    if(!checkAllowedCharacters(file_name))
    {
        printf("File name should contain only character\n");
        return false;
    }
    printf("Will open file named %s with the size of %u bytes\n",file_name,file_size);
    return true;
    //fileOpen(&file ,size,name)
}

bool mountSD_cmd(char **argv, cli_command_t* self)
{

}


void initSimpleCliSet(void)
{
    cli_command_t sendfile =
    {
        .cmd_name = "sendfile",
        .args_num = 3,
        .args = {   {.arg_name="-n", .arg_type=ARG_INT32    }, 
                    {.arg_name="-o", .arg_type=ARG_ONLY     }, 
                    {.arg_name="-f", .arg_type=ARG_STRING   }
                },
        .c_func = sendfile_cmd,
        .cmd_info = "Sends file over BLE NUS"
    };
    AddNewCommand(sendfile);
    
    cli_command_t mountSD =
    {
        .cmd_name = "mountsd",
        .args_num = 0,
        .args = {},
        .c_func = mountSD_cmd,
        .cmd_info = "Initializes SD card interface"
    };
    AddNewCommand(mountSD);

}