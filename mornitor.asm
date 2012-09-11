        ;; 함수를 호출하기 전의 레지스터 상태를 보여주는 코드 작성

.386
.MODEL  FLAT
        
PUBLIC  _STST                    ;외부에서 접근이 가능하게 한다. 이때 Under Bar를 붙여줘야 외부에서 볼 수가 있다.
PUBLIC  _MD
;; PUBLIC  _LDST
      
.CODE                           ;CODE영역 알림.
_STST   PROC    NEAR32          ;Procedure 의 시작이라는 것을 알리고, NEAR32은 그냥 일반 주소 체계를 쓰겠다는 말.
        ;; Entry Point

        push    ebp             ;main 함수의 ebp값을 기록해 둔다. ::    Entry Code
        mov     ebp,    esp     ;ebp가 현재 가리키고 있는       ::      Entry Code

        ;; push    eax
        ;; push    ebx             ;ebx 값을 되돌리기 위해 저장해 둔다.
        
        ;; ------------------------------------------------------------

        pushfd
        mov     esp,    [ebp + 8]
        add     esp,    40

        pushad

        push    [ebp + 4]
        push    [ebp - 4]

        add     esp,    24      

        mov     eax,    ebp
        add     eax,    12
        push    eax
        
        push    [ebp]
        
        
        ;; exit point
        ;; pop     efl
        ;; pop     ebx
        ;; pop     eax
        
        mov     esp,    ebp     ;esp를 끌어내림
        pop     ebp             ;Stack에 있던 return adress를 다시 ebp에 넣는다.
        
                
        ret                     ;return 값은 EAX 에 들어감 

_STST    ENDP                    ;함수의 끝을 알림.

        
_MD    PROC    NEAR32           ;Memory_Display
        ;; Entry Point

        push    ebp  
        mov     ebp,    esp

        push    ebx             ;ebx를 사용하기 위해서 STACK에 올린다.
        ;; ------------------------------------------------------------

        ;; 리턴값은 EAX를 사용해야 함.
        mov     ebx,    [ebp + 8] ;ebx에 인자로 받은 주소를 대입.

        mov     eax,    0       ;eax 를 0으로 초기화
        mov     al,     [ebx]   
        
        ;; ------------------------------------------------------------
        ;; exit point
        pop     ebx             ;ebx를 복구시켜 놓는다.
        
        mov     esp,    ebp
        pop     ebp        
        
        ret                     ;return 값은 EAX 에 들어감 

_MD    ENDP                    ;함수의 끝을 알림.

_LDST   PROC    NEAR32           ;레지스터 값을 다시 CPU에 넣는다.

        mov     esp,    [esp + 4] ;esp가 context를 가리킨다.

        popfd                   ;efl을 pop한다.
        pop     eax             ;eip를 eax에 넣는다.

        mov     ebx,    [esp + 12] ;esp_main의 주소를 ebx에 넣는다.
        mov     [ebx - 4],      eax ;esp_main의 값을 가지고 있는 ebx의 값에서 4를 뺀 위치에
                                ;eip의 값을 가지고 있는 eax를 넣는다.

        ;; popad                ;edi부터 역순으로 다시 레지스터에 넣는다.
        pop     edi
        pop     esi
        pop     ebp
        ;; pop     esp
        add     esp,    4
        pop     ebx
        pop     edx
        pop     ecx
        pop     eax

        mov     esp,    [esp - 20] ;esp의 위치는 현재 context의 가장 아래이고, esp에다가 현재 위치에서 -20한 위치에 있는
                                ;esp_main의 주소를 가리키게 한다.
        sub     esp,    4       ;main의 esp를 return address를 가리키게 하기 위해 4를 빼준다.
        
                
        ret                     ;return 값은 EAX 에 들어감 

_LDST   ENDP                    ;함수의 끝을 알
      
END                            


