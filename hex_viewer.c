// 처음 선언되는 변수의 주소값을 기준으로 16 x 10의 16진수 메모리 맵을 출력한다.
// 포인터 변수의 주소 연산과 포인터 변수가 가리키는 값의 주소를 출력하는 것이 목적이다.
// 보기 좋은 화면을 위해 기준이 되는 변수의 마지막 1바이트 값을 가이드 주소로 출력되게 했다.
// 간단한 파일 출력을 동시에 수행한다. memory_status.txt 라는 파일을 만들어 결과를 저장한다.
//
// 주석을 명료하며 되도록이면 다른 사람이 보고 이해하기 쉽도록 적었다.
// 2012년 5월 11일.
// 2012년 9월 14일 출력화면 개선
#include <stdio.h>
#include "hex_viewer.h"

int hex_viewer(unsigned char *address, unsigned char *stack_point, int line)
{
    int i_loop;
    int i_cnt;

    FILE *fpout; // 파일 포인터 변수 선언.

    unsigned char *c_num_ptr = (unsigned char *)address;

    fpout = fopen("memory_status.txt", "w"); // fpout 변수에 텍스트파일을 쓰기가 가능하게 불러온다.

    // 가이드 문구 시작.
    printf("--------------------------------------------------------------------------------"
           " Address ");

    // 주소 마지막 바이트 출력 시작 : 불러들인 주소의 마지막 한 바이트를 알아보기 쉽게 띄워준다.
    printf("  ");               /* 공백을 맞추기 위함. */
    for(i_cnt = 0;i_cnt <= 15;++i_cnt)
    {
        printf("%02X ", ((unsigned char)((char)c_num_ptr & 0x0f) + i_cnt)); /* 해당 주소를 쉽게 찾을 수 있게 변경. */
        //fprintf(fpout, "%02X ",((unsigned char)c_num_ptr + i_cnt));
    }
    // 주소 마지막 바이트 출력 끝.
    
    i_loop = (unsigned char)c_num_ptr & 0xf; /* ASCII CODE의 가이드 문자는 첫 번째 번지의 주소를 참조하여 차례로 출력. */
    for(i_cnt = 0; i_cnt <= 15; ++i_cnt)
    {
        if(15 < i_loop)
        {
            i_loop = 0x00;
        }
        printf("%X", i_loop); /* ASCII CODE가 출력되는 곳의 주소가이드 */
        i_loop = i_loop + 1;
    }
    putchar('\n');
    
    // 몸체 시작. : 주소와 해당 주소의 실제값을 1 바이트 단위로 출력, 아스키 코드 표시.
    for(i_loop = 0; i_loop <= line; ++i_loop) // 몇 줄의 주소를 띄울 것인가를 i_loop의 비교값으로 결정.
    {
        printf("0x%08X ", c_num_ptr); // 주소 출력
        fprintf(fpout, "%08X ", c_num_ptr); // 텍스트 파일 출력.

        // 1바이트씩 주소 값을 출력 시작. :
        i_cnt = 0;
        while(1)
        {
            if(i_cnt >= 16)     /* 16개를 출력하면 빠져 나간다. */
            {
                break;
            }
            
            if(c_num_ptr + i_cnt > stack_point) /* 스택 이상의 값은 출력하지 않는다. */
            {
                printf("|  ");  /* 터플 한 칸과 공백 두 칸 */
                
                for(i_cnt = i_cnt + 1; i_cnt <= 15; ++i_cnt) /* 나머지 부분은 빈 공간으로 지나간다. */
                {
                    printf("   "); /* 16진수 두자리와 공백, 총 세칸 */
                }
                break;
            }

            printf("%02x ", MD(address + i_cnt));
            fprintf(fpout, "%02x ", *(c_num_ptr + i_cnt)); // 텍스트 파일 출력.

            ++i_cnt;
        }
        // 1바이트씩 주소 값을 출력 끝. :

        // 값에 대한 아스키 코드 출력 시작.
        for(i_cnt = 0;i_cnt <= 15;++i_cnt)
        {
            if(c_num_ptr + i_cnt > stack_point) /* 스택 이상의 값은 출력하지 않는다. */
            {
                printf("|\n");

                return 0;
            }
                        
            if(' ' < MD(address + i_cnt)) // 제어문자 스페이스의 십진수 값은 32.
            { // 이 값보다 출력 값이 크면 해당 아스키를 뿌리는데 이때
                printf("%c", MD(address + i_cnt)); // 제대로 표현할 확장 아스키를 걸러주기 위해 signed char 캐스팅.
                fprintf(fpout, "%c", *(c_num_ptr + i_cnt)); // 텍스트 파일 출력.
            }
            else
            {
                putchar('.'); // 아스키값이 32 미만이거나 127 이상일 경우에는 점(도트)를 찍어준다.
                fprintf(fpout,"."); // 텍스트 파일 출력.
            }
        }
        printf("\n"); // 한줄이 끝나고 개행(줄바꿈)
        fprintf(fpout, "\n"); // 텍스트 파일 출력.
        
        address = address + 16; // 다음 줄의 올바른 주소값은 위에서 16바이트를 출력했으니 다음줄은 기준이 되고 있는 첫바이트> 보다 16이 많다.
        c_num_ptr = c_num_ptr + 16; // 다음 줄의 올바른 주소값은 위에서 16바이트를 출력했으니 다음줄은 기준이 되고 있는 첫바이트> 보다 16이 많다.
    }
    putchar('\n');
    fclose(fpout); // 파일을 닫아준다.

    return 0;
}
