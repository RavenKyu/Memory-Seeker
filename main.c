#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "hex_viewer.h"

#define COMMAND_LEN     100

enum CommandNum                 /* enum은 숫자를 문자화 시키는 장점을 활용 */
{
    REGISTER_DISPLAY = 100,     /* 초기 숫자를 100으로 초기화. Switch문에서 사용한다. */
    Q,
    QUIT,
    HELP,
    H,
    MD_,
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
    int (*cmdfp)(void *, int);  /* 함수 포인터 사용 */
    enum CommandNum cmd_num;
} CommandMap;

/* C 언어 함수 원형 ------------------------------------- */
int Register_Display(void *, int);
int Memory_Display(void *, int);
int Quit(void *, int);
int Help(void *, int);

/* 어셈블리 함수 원형 ----------------------------------- */
void STST(Context *);           /* 메모리의 상태를 보여 준다. */
unsigned char MD(void *);

int main()
{
    char command[COMMAND_LEN];

    void *vp;                   /* 인수로 사용, 주소 */
    int i_len;
    
    Context registers;
    CommandMap *p_map;
    
    CommandMap c_map[] =
        {
            {"R\n",     Register_Display, REGISTER_DISPLAY},
            {"Q\n",     Quit, Q},
            {"QUIT\n",  Quit, Q},
            {"HELP\n",  Help, H},
            {"H\n",     Help, H},
            {"?\n",     Help, H},
            {"MD\n",    Memory_Display, MD_},
            {0, 0}
        };
    
    STST(&registers);           /* 구조체 registers에 레지스터의 주소를 저장  */
    Register_Display(&registers, 0); /* 레지스터를 출력한다. */
    
    while(1)
    {
        fflush(stdin);          /* 키보드 버퍼를 비운다. */

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
                vp = &registers;
                break;
        
            case QUIT:
                break;
                
            case Q:
                break;

            case H:
                break;

            case HELP:
                break;

            case MD_:
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
    /* 나중에 동적할당 받은 부분을 해제 해 줄 코드가 필요하다. */
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

int Memory_Display(void *vp, int i_not_use) /* 입력받은 위치의 메모리 맵을 보여 준다. */
{
    unsigned char *ucp;
    
    printf("Enter Address you want as Hex : ");
    scanf("%x", (int *)&ucp);

    /* Debuggig Code -------------------------------------------------- */
    printf("C Language  :: %02X\n", *ucp); /* C로 직접 나타낼때 */
    printf("Assembler   :: %02X\n", MD(ucp)); /* Assembly 함수를 사용할때 */
    /* Debuggig Code -------------------------------------------------- */
    hex_viewer(ucp);
    return MD(ucp);
}
