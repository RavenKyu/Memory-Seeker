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
    P,
    MC,
    MM,
    CODE,
    DATA,
    STACK,

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
int Memory_Modify(void *, int);
int Memory_Display_Status(void *, int);
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

static int add_address = 0;

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
            {"MM\n",    Memory_Modify, MM},            
            {"GO\n",    Go, GO},
            {"LOAD\n",  Load, LOAD},
            {"MC\n",  Clear_mem, MC},
            {"CODE\n",  Memory_Display, CODE},
            {"DATA\n",  Memory_Display, DATA},
            {"STACK\n",  Memory_Display, STACK},
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
        for(i_len = strlen(command) - 1; i_len >= 0; --i_len) 
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

        if(0x0A == command[0])  /* 엔터키만 눌러 졌을 때  */
        {
            if((mem <= vp) && (mem_end >= vp)) /* 메모리가 동적할당 받은 범위를 벗어나서 런타임 에러를 일으키지 않게 한다. */
            {
                add_address = add_address + 256; /* 16 x 16 으로 다음 페이지 만큼 넣어둔다. */
                printf("Next page of memory.\n");
                Memory_Display(vp, add_address); /* 인자로 인자로 받았던 값과 추가할 값을 넣는다. */
            }
        }
        else if((p_map -> cmdCommand == 0) && (0x0A != command[0])) /* 잘못된 명령어를 받았을 때 Help함수를 호출한다. */
        {
            Help(0, 0);
        }
        
        if(0 != p_map -> cmdCommand) /* 유효한 명령을 입력 했는가? 를 검사 */
        {
            switch(p_map -> cmd_num) /* 함수 인자를 설정한다. */
            {
            case REGISTER_DISPLAY:
                vp = &cpu_info;
                i_len = 0;
                break;
        
            case QUIT:
                vp = 0;
                i_len = 0;
                break;
                
            case Q:
                vp = 0;
                i_len = 0;
                break;

            case H:
                vp = 0;
                i_len = 0;
                break;

            case HELP:
                vp = 0;
                i_len = 0;
                break;

            case MD_:
                i_len = -1;
                break;

            case GO:
                vp = 0;
                i_len = 0;
                break;

            case LOAD:
                vp = 0;
                i_len = 0;
                break;

            case MC:
                vp = 0;
                i_len = 0;
                break;

            case CODE:
                i_len = 0;
                vp = code;
                break;

            case DATA:
                vp = data;
                i_len = 0;
                break;

            case STACK:
                vp = stack - 0xff;
                i_len = 0;
                break;
            }
            
            add_address = -1;                       /* 초기화 */
            vp = (*(p_map -> cmdfp))(vp, i_len); /* 함수 호출 */            
        }
    } 
    
    return 0;
}

int Register_Display(void *r, int not_use)
{
    printf("EAX :: %08X\n", ((Context *)r) -> eax);
    printf("ECX :: %08X\n", ((Context *)r) -> ecx);
    printf("EDX :: %08X\n", ((Context *)r) -> edx);
    printf("EBX :: %08X\n", ((Context *)r) -> ebx);
    
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

int Help(void *v_not_use, int i_not_use) /* 도움말을 출력한다. */
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

int Memory_Display(void *vp, int add_address) /* 입력받은 위치의 메모리 맵을 보여 준다. */
{
    if(0 > add_address)			/* add_address가 음수라면 주소값을 받도록 한다. MD 기능을 사용하게 한다. */
    {
        vp = 0;
        while(1)
        {
            if((mem <= vp) && (mem_end >= vp)) /* 범위내의 값을 입력 받을 때까지 계속적으로 입력 요구 */
            {
                break;
            }
            printf("Enter Address you want to display. '0' for Cancel.\n\(0x%08X ~ 0x%08X\) : ", mem, mem_end);
            scanf_s("%x", &vp);
            
            if(0 == vp)         /* 숫자 0을 입력 받으면 명령을 취소 할 수 있다. */
            {
                return 0;
            }
            fflush(stdin);      /* 키보드 버퍼를 비움으로서 무한루프를 막는다. */
        }
        add_address = 0;		/* 다음 페이지로 넘어가기 위해서 0으로 설정 */
    }
    hex_viewer((unsigned char *)((int)vp + add_address), 15); /* 메모리 맵을 출력한다. */
	
    return vp;
}

int Memory_Modify(void *vp, int i_not_use) /* 입력 받은 주소의 값을 바꿔준다. */
{
    unsigned int val = 0;
    unsigned int address = 1;

    while(1)                    /* 허용된 범위 안의 주소값을 받을 때까지 입력문을 무한루프 돌린다. */
    {
        if((mem <= address) && (mem_end >= address)) 
        {
            break;
        }
        printf("Enter the address where you want to modify the value. '0' for Cancel.\n\(0x%08X ~ 0x%08X\)  : ", mem, mem_end);
        scanf_s("%x", &address);    /* 주소를 입력 받는다. */

        if(0 == address)        /* 숫자 0 을 입력 받으면 기능 취소 */
        {
            return 0;
        }
        fflush(stdin);      /* 키보드 버퍼를 비움으로서 무한루프를 막는다. */
    }
    
    hex_viewer(address, 0);        /* 입력받은 주소의 메모리 맵을 출력한다. */

    printf("Put a value in HEX you want to change at 0x%08X : ", address);
    scanf_s("%x", &val);        /* 바꿀 값을 HEX 코드로 입력 받는다. */

    memset(address, val, 1);    /* 해당 위치의 값을 바꾼다. */

    hex_viewer(address, 0); /* 바뀐 값을 확인 할 수 있도록 맵을 출력한다. */
    printf("Modified the value, [%x] in 0x%08X\n", val, address);
    
    return vp;
}

int Go(void *v_not_use, int i_not_use) /* 메모리에 적재된 프로그램을 실행한다. */
{
    Context cpu_temp;
    memset(&cpu_temp, 0, sizeof(Context));
    
    cpu_temp.eax = (unsigned int)&cpu_info;
    cpu_temp.eip = (unsigned int)code;
    cpu_temp.esp = (unsigned int)mem_end - 1;

    if(0x00 == *code)           /* code 영역의 첫번째 번지에 값이 0일 경우 프로그램이 로드되지 않은것 으로 간주 */
    {
        printf("There is no program to run. You should load a program first. ex) t1.bin\n");
        return 0;               /* 에러 메세지를 출력하고, 프로그램이 강제 종료되는 것을 막는다. */
    }
    LDST(&cpu_temp);

    printf("Kernel panic!\n");  /* 프로그램은 올라갔으나 제대로 수행하지 못하고 나왔을 경우, 커널패닉. */
    return 0;
}


int Clear_mem(void *v_not_use, int i_not_use) /* 메모리를 초기화 시켜준다. */
{
    memset(code, 0, MAX_PROGRAM_SIZE);

    printf("Memory Initialization is done.\n");
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

    printf("Enter the file name you want to load : "); /* 메모리에 적재할 파일을 불러온다. */
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
