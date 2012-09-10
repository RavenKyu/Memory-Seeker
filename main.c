#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define COMMAND_LEN     100

typedef struct _Context
{
    unsigned int efl;
    unsigned int eip;
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int esp;
    unsigned int ebx;
    unsigned int edx;
    unsigned int ecx;
    unsigned int eax;
} Context;

void Register_Display(Context *);
void STST(Context *);

int main()
{
    int i_len;
    char command[COMMAND_LEN];
    
    Context r;
    
    STST(&r);
    Register_Display(&r);
    
    while(1)
    {
        fflush(stdin);
        putchar('>');
        putchar(' ');

        fgets(command, COMMAND_LEN, stdin); /* 메뉴를 입력받는다. */

        /* 대소문자 구별없이 입력받기 위해, 모든 문자를 대문자로 바꾼다. */
        for(i_len = (strlen(command) - 1);i_len >= 0;--i_len) 
        {
            command[i_len] = toupper(command[i_len]);
        }

        if(0 == strcmp(command, "Q\n"))
        {
            return 0;
        }
        else if(0 == strcmp(command, "R\n"))
        {
            Register_Display(&r);
        }
        else if(0 == strcmp(command, "QUIT\n")) /* 대소문자를 가리지 않고,  */
        {
            return 0;
        }
        else
        {
            printf("Show menu\n");
        }
    }
    
    return 0;
}

void Register_Display(Context *r)
{
    printf("EAX :: %08X\n", r -> eax);
    printf("EBX :: %08X\n", r -> ebx);
    printf("ECX :: %08X\n", r -> ecx);
    printf("EDX :: %08X\n", r -> edx);

    printf("ESP :: %08X\n", r -> esp);
    printf("EBP :: %08X\n", r -> ebp);

    printf("ESI :: %08X\n", r -> esp);
    printf("EDI :: %08X\n", r -> edi);

    printf("EIP :: %08X\n", r -> eip);
    printf("EFL :: %08X\n", r -> efl);

    return;
}
