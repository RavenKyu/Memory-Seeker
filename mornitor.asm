        ;; �Լ��� ȣ���ϱ� ���� �������� ���¸� �����ִ� �ڵ� �ۼ�

.386
.MODEL  FLAT
        
PUBLIC  _STST                    ;�ܺο��� ������ �����ϰ� �Ѵ�. �̶� Under Bar�� �ٿ���� �ܺο��� �� ���� �ִ�.
PUBLIC  _MD
        
.CODE                           ;CODE���� �˸�.
_STST   PROC    NEAR32          ;Procedure �� �����̶�� ���� �˸���, NEAR32�� �׳� �Ϲ� �ּ� ü�踦 ���ڴٴ� ��.
        ;; Entry Point

        push    ebp             ;main �Լ��� ebp���� ����� �д�. ::    Entry Code
        mov     ebp,    esp     ;ebp�� ���� ����Ű�� �ִ�       ::      Entry Code

        ;; push    eax
        ;; push    ebx             ;ebx ���� �ǵ����� ���� ������ �д�.
        
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
        
        mov     esp,    ebp     ;esp�� �����
        pop     ebp             ;Stack�� �ִ� return adress�� �ٽ� ebp�� �ִ´�.
        
                
        ret                     ;return ���� EAX �� �� 

_STST    ENDP                    ;�Լ��� ���� �˸�.

        
_MD    PROC    NEAR32           ;Memory_Display
        ;; Entry Point

        push    ebp  
        mov     ebp,    esp

        push    ebx             ;ebx�� ����ϱ� ���ؼ� STACK�� �ø���.
        ;; ------------------------------------------------------------

        ;; ���ϰ��� EAX�� ����ؾ� ��.
        mov     ebx,    [ebp + 8] ;ebx�� ���ڷ� ���� �ּҸ� ����.

        mov     eax,    0       ;eax �� 0���� �ʱ�ȭ
        mov     al,     [ebx]   
        
        ;; ------------------------------------------------------------
        ;; exit point
        pop     ebx             ;ebx�� �������� ���´�.
        
        mov     esp,    ebp
        pop     ebp        
        
        ret                     ;return ���� EAX �� �� 

_MD    ENDP                    ;�Լ��� ���� �˸�.
      
END                            


