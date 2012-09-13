// 처음 선언되는 변수의 주소값을 기준으로 16 x 10의 16진수 메모리 맵을 출력한다.
// 포인터 변수의 주소 연산과 포인터 변수가 가리키는 값의 주소를 출력하는 것이 목적이다.
// 보기 좋은 화면을 위해 기준이 되는 변수의 마지막 1바이트 값을 가이드 주소로 출력되게 했다.
// 간단한 파일 출력을 동시에 수행한다. memory_status.txt 라는 파일을 만들어 결과를 저장한다.
//
// 주석을 명료하며 되도록이면 다른 사람이 보고 이해하기 쉽도록 적었다.
// 2012년 5월 11일.
#include <stdio.h>
#include "hex_viewer.h"

int hex_viewer(unsigned char *address)
{
    int i_loop;
    /* int i_num = 0xABCDEF88; */
    int i_cnt;

    FILE *fpout; // 파일 포인터 변수 선언.

    unsigned char *c_num_ptr = (unsigned char *)address;

    fpout = fopen("memory_status.txt", "w"); // fpout 변수에 텍스트파일을 쓰기가 가능하게 불러온다.

    // 가이드 문구 시작.
    printf("--------------------------------------------------------------------------------\n"
           " Address ");

    // 주소 마지막 바이트 출력 시작 : 불러들인 주소의 마지막 한 바이트를 알아보기 쉽게 띄워준다.
    printf("  ");               /* 공백을 맞추기 위함. */
    for(i_cnt = 0;i_cnt <= 15;++i_cnt)
    {
        printf("%02X ", ((unsigned char)c_num_ptr + i_cnt));
        /* printf("%02X ", ((unsigned char)MD(address) + i_cnt)); */
        //fprintf(fpout, "%02X ",((unsigned char)c_num_ptr + i_cnt));
    }
    // 주소 마지막 바이트 출력 끝.

    printf(" ASCII CODE \n");
    //fprintf(fpout, "\n", i_num);
    // 몸체 시작. : 주소와 해당 주소의 실제값을 1 바이트 단위로 출력, 아스키 코드 표시.
    for(i_loop = 0;i_loop <= 10;++i_loop) // 몇 줄의 주소를 띄울 것인가를 i_loop의 비교값으로 결정.
    {
        printf("0x%08x ", c_num_ptr); // 주소 출력
        fprintf(fpout, "%08x ", c_num_ptr); // 텍스트 파일 출력.

        // 1바이트씩 주소 값을 출력 시작. :
        for(i_cnt = 0;i_cnt <= 15;++i_cnt)
        {
            /* printf("%02x ", *(c_num_ptr + i_cnt)); */
            printf("%02x ", MD(address + i_cnt));
            fprintf(fpout, "%02x ", *(c_num_ptr + i_cnt)); // 텍스트 파일 출력.
        }
        // 1바이트씩 주소 값을 출력 끝. :

        // 값에 대한 아스키 코드 출력 시작.
        for(i_cnt = 0;i_cnt <= 15;++i_cnt)
        {
            /* if(' ' < *(((signed char *)c_num_ptr) + i_cnt)) // 제어문자 스페이스의 십진수 값은 32. */
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
