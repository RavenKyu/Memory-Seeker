#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h> /* oflag를 사용하기 위해서 포함. */
#include <sys/types.h>
#include <sys/stat.h>
#include <malloc.h>
#include <windows.h>

#include "hex_viewer.h"

#define COMMAND_LEN             100 /* 입력을 받을 수 있는 최대 명령어 길이 */
#define MAX_PROGRAM_SIZE        0x10000 /* 적재할 프로그램의 최대 크기의 1/2 */
#define SECTION_SIZE            512     /* 각 메모리 섹션이 가지는 크기 */

enum CommandNum                 /* enum은 숫자를 문자화 시키는 장점을 활용 */
{
    REGISTER_DISPLAY = 100,     /* 초기 숫자를 100으로 초기화. Switch문에서 사용한다. */
    Q,
    QUIT,
    HELP,
    H,
    MD_,
    GO,
    LOAD,
    MC,
    CODE,
    DATA,
    STACK,
    P,
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
int Memory_Display_Status(void *, int);
int Display_Memory_Map(void *, int);
int Memory_Display_Data(void *, int);
int Memory_Display_Stack(void *, int);
int Quit(void *, int);
int Help(void *, int);
int Go(void *, int);
int Load(void *, int);
int Clear_mem(void *, int);

/* 어셈블리 함수 원형 ----------------------------------- */
void STST(Context *);           /* 메모리의 상태를 보여 준다. */
void LDST(Context *);           /* 데이터를 메모리에 적재한다. */

/* char MD(void *); */
unsigned char MD(void *);       /* unsigned 형으로 선언하지 않으면 앞자리가
                                 * 8이라면 부호처리를 시도해서 마이너스가된다. */
static unsigned char *mem;      
static unsigned char *mem_end;

static unsigned char *code;
static unsigned char *data;
static unsigned char *stack;

static Context cpu_info;

int main()
{
    char command[COMMAND_LEN];

    void *vp;                   /* 인수로 사용, 주소 */
    int i_len;
    
    CommandMap *p_map;
    
    CommandMap c_map[] =
        {
            {"R\n",     Register_Display, REGISTER_DISPLAY},
            {"P\n",     Memory_Display_Status, P},
            {"Q\n",     Quit, Q},
            {"QUIT\n",  Quit, Q},
            {"HELP\n",  Help, H},
            {"H\n",     Help, H},
            {"?\n",     Help, H},
            {"MD\n",    Memory_Display, MD_},
            {"GO\n",    Go, GO},
            {"LOAD\n",  Load, LOAD},
            {"MC\n",  Clear_mem, MC},
            {"CODE\n",  Display_Memory_Map, CODE},
            {"DATA\n",  Display_Memory_Map, DATA},
            {"STACK\n",  Display_Memory_Map, STACK},
            {0, 0}
        };

    mem = (unsigned char *)malloc(MAX_PROGRAM_SIZE * 2); /* 128Kb 할당 받았다. */

    if(0 == mem)                                         /* 메모리할당 성공여부를 검사 */
    {
        printf("Failed to allocate enough memory.\n"); 
        return 0;
    }
    mem_end = mem + ((MAX_PROGRAM_SIZE * 2) - 1); /* 동적할당 받은 메모리의 끝이자 스택의 시작 부분 */
    
    code = (unsigned char *)((unsigned int)mem & 0xffff0000) + MAX_PROGRAM_SIZE; /* 포인터는 & 연산을 할 수 없기 때문에 캐스팅으로 바꿔 연산 한다. */
    data = code + 0x2000;       /* data 영역의 주소는 코드에서 2000번지 내려온 곳 */
    stack = mem_end; /* 스택영역은 메모리 제일 밑에서 부터. */

    STST(&cpu_info);           /* 구조체 cpu_info에 레지스터의 주소를 저장  */

    Memory_Display_Status(0, 0); /* 메모리의 주소 범위를 보여준다. */
    
    printf("Enable range of Dynamic Memory Address :: %08X ~ %08X\n", mem, mem_end); /* 0 부터 시작하므로 -1을 한다. */
    
    Register_Display(&cpu_info, 0); /* 레지스터를 출력한다. */
    
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
                vp = &cpu_info;
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

            case GO:
                break;

            case LOAD:
                break;

            case MC:
                break;

            case CODE:
                vp = code;
                break;

            case DATA:
                vp = data;
                break;

            case STACK:
                vp = stack - 0xff;
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
    free(mem);                  /* 동적할당 받은 메모리를 풀어준다. */

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
    scanf_s("%x", &ucp); 

    hex_viewer(ucp);
    return MD(ucp);
}

int Display_Memory_Map(void *vp, int i_not_use) /* 메모리의 CODE 영역의 Hex map을 출력한다. */
{
    hex_viewer(vp);
    return 0;
}

int Go(void *v_not_use, int i_not_use) /* 메모리에 적재된 프로그램을 실행한다. */
{
    Context cpu_temp;
    memset(&cpu_temp, 0, sizeof(Context));
    
    cpu_temp.eax = (unsigned int)&cpu_info;
    cpu_temp.eip = (unsigned int)code;
    cpu_temp.esp = (unsigned int)mem_end - 1;
    
    LDST(&cpu_temp);

    printf("Kernel panic!\n");
    return 0;
}


int Clear_mem(void *v_not_use, int i_not_use) /* 메모리를 초기화 시켜준다. */
{
    memset(code, 0, MAX_PROGRAM_SIZE);

    return 0;
}


int Load(void *v_not_use, int i_not_use)           /* 프로그램을 할당받은 메모리에 적재 시켜준다 */
{
    int Read_Num;               /* 파일을 잘 읽어 오는지 검사용 변수 */
    int Header_Size;

    int infd;                   /* 파일 핸들러 */
    
    IMAGE_DOS_HEADER *dhp;      /* Header의 정보를 가지고 있는 구조체들. */
    IMAGE_NT_HEADERS *php;
    IMAGE_FILE_HEADER *fhp;
    IMAGE_OPTIONAL_HEADER32 *ohp;
    unsigned char File_Name[255]; // 읽어올 파일 이름의 임시 저장소

    Clear_mem(0, 0);            /* 메모리를 깨긋히 비워준다. */

    printf("\nEnter the file name you want to load : "); /* 메모리에 적재할 파일을 불러온다. */
    scanf("%s", File_Name);

    infd = open(File_Name, O_RDONLY | O_BINARY); /* 파일을 읽기전용과 바이너리 모드로 열어서 핸들러에 넣는다. */
    if(0 > infd)                      /* 파일 열기에 실패 했을 경우, 에러 메세지 출력 후, 종료. */
    {
        printf("Sorry, Failed to load the file..\n");
        printf("Check the file is existed or correct file name.\n");

        return 0;
    }
    else                        /* 파일 열기에 성공하면, 파일 끝까지 한번 읽는다. */
    {
        Read_Num = read(infd, code, MAX_PROGRAM_SIZE);
    }

    /* 파일 열기에 성공했을 경우에 파일을 한번 읽는다. */
    if(0 > Read_Num)            /* 파일을 열었으나, 읽어오는 것에 실패 */
    {
        printf("Succeed to open the file, but read.\n");
        close(infd);
        return 0;
    }
    else                        /* 파일을 열고, 읽어오기에 성공하면, PE스펙에 따라 헤더의 전체 사이즈를 구한다. */
    {
        dhp = (IMAGE_DOS_HEADER *)code;
        php = (IMAGE_NT_HEADERS *)(code + (dhp -> e_lfanew));
        fhp = (IMAGE_FILE_HEADER *)((char *)php + sizeof(php -> Signature));
        ohp = (IMAGE_OPTIONAL_HEADER32 *)((char *)fhp + sizeof(IMAGE_FILE_HEADER));
        Header_Size = ohp -> SizeOfHeaders;
        printf("Succeed to open the file and read.\n");
        Clear_mem(0, 0);        /* 헤더크기를 구했으므로 메모리를 비운다. */
    }
    
    if(0 > lseek(infd, Header_Size, SEEK_SET))  /* 헤더 부분을 넘기기 위해 읽어서 보낸다. */
    {
        printf("Failed to skip the header section. \n");
        close(infd);

        return 0;
    }
    else                        
    {
        Read_Num = read(infd, code, SECTION_SIZE); /* 파일에서의 CODE영역을 읽어 메모리상의 CODE 영역에
                                                    * 주어진 SECTION_SIZE 만큼을 적재한다. */
    }

    if(0 > Read_Num)            /* 데이터를 메모리에 잘 적재 했는지를 검사한다. */
    {
        printf("Failed to read.\n");
        close(infd);

        return 0;
    }
    else                        /* 다음은 데이터 영역에 적재한다. */
    {
        Read_Num = read(infd, data, SECTION_SIZE);
    }
    close(infd);

    if (0 > Read_Num)           /* 데이터 영역 적재가 성공했는지를 판단, */
    {
        printf("Succeed to open the file, but read\n");

        return 0;
    }
    else
    {
        printf("Loading the file is done.\n"); /* 모든 데이터를 메모리에 적재 했음을 알린다. */
    }
    
    return 0;
}

int Memory_Display_Status(void *v_not_use, int i_not_use) /* 메모리의 영역 상태를 출력한다. */
{
    printf("Code address  :: 0x%08X\n", code); /* 메모리 주소 영역을 출력 */
    printf("Data address  :: 0x%08X\n", data); 
    printf("Stack address :: 0x%08X\n", stack); 
    
    return 0;
}
