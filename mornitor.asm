        ;; 함수를 호출하기 전의 레지스터 상태를 보여주는 코드 작성

.386
.MODEL  FLAT
        
PUBLIC  _STST                    ;외부에서 접근이 가능하게 한다. 이때 Under Bar를 붙여줘야 외부에서 볼 수가 있다.
        
  .CODE                           ;CODE영역 알림.
_STST    PROC    NEAR32          ;Procedure 의 시작이라는 것을 알리고, NEAR32은 그냥 일반 주소 체계를 쓰겠다는 말.
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
      
END                            


