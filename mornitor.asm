        ;; �Լ��� ȣ���ϱ� ���� �������� ���¸� �����ִ� �ڵ� �ۼ�

.386
.MODEL  FLAT
        
PUBLIC  _STST                    ;�ܺο��� ������ �����ϰ� �Ѵ�. �̶� Under Bar�� �ٿ���� �ܺο��� �� ���� �ִ�.
        
  .CODE                           ;CODE���� �˸�.
_STST    PROC    NEAR32          ;Procedure �� �����̶�� ���� �˸���, NEAR32�� �׳� �Ϲ� �ּ� ü�踦 ���ڴٴ� ��.
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
      
END                            


