#include "stdio.h"
#include "simple_cli.h"
#include "cli_command_set.h"


int main()
{
    initSimpleCliSet();
    printf("Prog started\n");
    char str1[]="sendfile -n 100000 -o -f feriX1.tct\n";
    char str2[]="sendfil -n 100000 -o -f feriX1.tct\n";
    char str3[]="sendfile -o -f feriX1.tct\n";
    char str4[]="sendfile -n -o -f feriX1.tct\n";
    char str5[]="sendfile -f -n 100 -o\n";
    char str6[]="sendfile\n";
    char str7[]="sendfile      -n    100000      -o   -f    feriX1.tct\n";
    char str8[]="sendfileg      -n    100000      -o   -f    feriX1.tct\n";
    char str9[]="mountsd\n";
    char str10[]="mountsd      -n    100000      -o   -f    feriX1.tct\n";
    printf("String 1= %s",str1);
    ProcessCommand(str1);

    printf("\nString 2= %s",str2);
    ProcessCommand(str2);

    printf("\nString 3= %s",str3);
    ProcessCommand(str3);

    printf("\nString 4= %s",str4);
    ProcessCommand(str4);

    printf("\nString 5= %s",str5);
    ProcessCommand(str5);

    printf("\nString 6= %s",str6);
    ProcessCommand(str6);

    printf("\nString 7= %s",str7);
    ProcessCommand(str7);

    printf("\nString 8= %s",str8);
    ProcessCommand(str8);

    printf("\nString 9= %s",str9);
    ProcessCommand(str9);

    printf("\nString 10= %s",str10);
    ProcessCommand(str10);
    return 0;
}