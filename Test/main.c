#include "stdio.h"
#include "simple_cli.h"
#include "cli_command_set.h"
#include "string.h"

SIMPLE_CLI_DEF (MainC);         /*Use SIMPLE_CLI_DEF to create Main CLI context object*/

/*This function implements data output routines. It receives data of sizeof(length)
from command content handler function. For example it could be code that sends 
data over TX pin of UART interface */
uint32_t StdOutWrite(const char* data, size_t length)
{
	for(size_t i=0;i<length;++i)
        printf("%c", data[i]);
	return 0;
}

/*Highest level context handler. Starts command parse sequence.*/
bool MainContextHandler(char *data, size_t length, void * _context)
{
	const char unknown_cmd[]="Command unknown\n";
    ((void)(length));
	int8_t com_res=ProcessCommand(data,(SimpleCliContext_t*)_context);
	printf("Command : #%d\n",com_res);
	if(!com_res)
    {
        StdOutWrite(unknown_cmd, sizeof(unknown_cmd));
        return false;
    } 						
	return true;
}

int main()
{
    
    InitCLIcontext(&MainC,MainContextHandler,StdOutWrite,"Main_context");
    /*Initializing commands set*/
    initSimpleCliSet();

    char str1[]="sendfile -n 10 -o -f feriX1.tct\r\n";
    char str2[]="sendfile -n 100";
    char str3[]="mountsd";
    char str4[]="mount_sd";
    char str5[]="sendfile -f -n 100";

    bool ret_res;
    printf("\nString 1= %s\n",str1);
    ret_res=CallContextHandler(&MainC,str1,strlen(str1));  //Command received
    /*Data flow simulation*/
    if(ret_res)
        CallContextHandler(&MainC,"0123456789",strlen("0123456789")); 
    
    printf("\nString 2= %s\n",str2);
    CallContextHandler(&MainC,str2,strlen(str2));   //Command received
    /*Data flow simulation. 100 bytes*/
    if(ret_res)
    {
        for(int i=0;i<10;++i)
            CallContextHandler(&MainC,"0123456789",strlen("0123456789")); 
    }

    printf("\nString 3= %s\n",str3);
    CallContextHandler(&MainC,str3,strlen(str3));   //Command received

    printf("\nString 4= %s\n",str4);
    ret_res=CallContextHandler(&MainC,str4,strlen(str4));   //Command received
    if(!ret_res)
        printf("Command unknown\n");

    printf("\nString 5= %s\n",str5);
    ret_res=CallContextHandler(&MainC,str5,strlen(str5));   //Command received
    if(!ret_res)
        printf("Command unknown\n");

    return 0;
}