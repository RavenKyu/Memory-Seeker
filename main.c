#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define COMMAND_LEN     100

enum CommandNum                 /* enum은 숫자를 문자화 시키는 장점을 활용 */
{
    REGISTER_DISPLAY = 100,     /* 초기 숫자를 100으로 초기화 */
    Q,
    QUIT,
    HELP,
    H,
    EXIT_NUM,
};

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

typedef struct _CommandMap
{
    char *cmdCommand;
    int (*cmdfp)(void *, int);
    enum CommandNum cmd_num;
} CommandMap;

int Register_Display(void *, int);
int Quit(void *, int);
int Help(void *, int);
void STST(Context *);


int main()
{
    int i_len;
    char command[COMMAND_LEN];
    void *vp;
    
    Context r;
    CommandMap *p_map;
    
    CommandMap c_map[] =
        {
            {"R\n",     Register_Display, REGISTER_DISPLAY},
            {"Q\n",     Quit, Q},
            {"QUIT\n",  Quit, Q},
            {"HELP\n",  Help, H},
            {"H\n",     Help, H},
            {"?\n",     Help, H},
            {0, 0}
        };
    
    STST(&r);
    Register_Display(&r, 0);
    
    while(1)
    {
        fflush(stdin);
        putchar('>');
        putchar(' ');

        fgets(command, COMMAND_LEN, stdin); /* 메뉴를 입력받는다. */

        /* 대소문자 구별없이 입력받기 위해, 모든 문자를 대문자로 바꾼다. */
        for(i_len = strlen(command) - 1;i_len >= 0;--i_len) 
        {
            command[i_len] = toupper(command[i_len]);
        }
        
        for(p_map = c_map; 0 != p_map -> cmdCommand; ++p_map) /* 구조체를 한번 훓어 주는 역활 */
        {
            if(0 == strcmp(command, p_map -> cmdCommand)) /* 일치하는 명령문이 있는지 찾는다. */
            {
                break;
            }
        }
        
        if(0 != p_map -> cmdCommand) /* 유효한 명령을 입력 했는가? 를 검사 */
        {
            vp = 0;
            i_len = 0;
            
            switch(p_map -> cmd_num) /* 함수 인자를 설정한다. */
            {
            case REGISTER_DISPLAY:
                vp = &r;
                break;
        
            case QUIT:
                break;
                
            case Q:
                break;

            case H:
                break;

            case HELP:
                break;
            }
            
            (*(p_map -> cmdfp))(vp, i_len); /* 함수 호출 */
        }
    } 
    
    return 0;
}

int Register_Display(void *r, int not_use)
{
    printf("EAX :: %08X\n", ((Context *)r) -> eax);
    printf("EBX :: %08X\n", ((Context *)r) -> ebx);
    printf("ECX :: %08X\n", ((Context *)r) -> ecx);
    printf("EDX :: %08X\n", ((Context *)r) -> edx);

    printf("ESP :: %08X\n", ((Context *)r) -> esp);
    printf("EBP :: %08X\n", ((Context *)r) -> ebp);

    printf("ESI :: %08X\n", ((Context *)r) -> esp);
    printf("EDI :: %08X\n", ((Context *)r) -> edi);

    printf("EIP :: %08X\n", ((Context *)r) -> eip);
    printf("EFL :: %08X\n", ((Context *)r) -> efl);

    return 0;
}

int Quit(void *V_not_use, int i_not_use)
{
    /* Require Free Code */
    exit(0);
    return 0;
}

int Help(void *v_not_use, int i_not_use)
{
    printf("R         : Register Value Display\n");
    printf("P         : Memory Status Display\n");
    printf("MC        : Memory Clear\n");
    printf("MM        : Memory Modify\n");
    printf("MD        : Memory Display\n");
    printf("LOAD      : Program Load\n");
    printf("GO        : Loaded Program Execute\n");
    printf("CODE      : Code Area Display\n");
    printf("DATA      : Data Area Display\n");
    printf("STACK     : Stack Area Display\n");
    printf("HELP      : Help Message Display\n");
    printf("QUIT(Q)   : Exit Program\n");
    
    return 0;
}
