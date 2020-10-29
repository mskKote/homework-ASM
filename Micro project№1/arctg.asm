format PE console
entry start
include 'win32a.inc'
include 'arctg_macro.inc'

;--------------------------------------------------------------------------
section '.data' data readable writable

        strT         db 'Input angle <t> for argtg(t), t belongs [-1;1]', 10, 0
        strF         db '%s', 0
        errorMsg     db 10, 'Incorrect input.', 10, 10, '   SELF DESTRUCTION ACTIVATED',10, 0
        taylorMsg    db 10, 'Arctg by Taylor series started', 10, 0
        sumMsg       db 10, 'Summing...', 0
        formatFloat  db '%lf', 0
        formatOUTTT  db '%d',10, 0
        formatTest   db 10,'%lf', 10,0
        ; Для вывода последовательности
        OUTNUM       db '%lf', 0
        PLUS         db ' +', 0
        MINUS        db ' ', 0
        ASNWER       db 10, 10, 'Answer is %lf radians', 10, 10, 0
        nextSTR      db 10, '=====================', 10,0
        
        result       dq 0.0
        prec         dq 0.0005
        currentPrec  dq 1.0
        currValueT   dq ?
        currValue    dq 1.0
        prevResult   dq 1.0
        t            dq ?
        buff         dq 2.0
        sign         dq -1.0
        n            dq 0
        iteration    dq 0
        
        MOne         dq -1.0    
        nullify      dq 0.0
        one          dq 1.0
        two          dq 2.0
;--------------------------------------------------------------------------
section '.code' code readable executable
start:
        FINIT
        invoke printf, strT
        invoke scanf,  formatFloat, t;
        FLD  [t]
        FSTP [currValueT]
        ;============== Проверка на ввод// |t|<=1
        FLD  [t]
        FABS               
        FCOM [one]
        FSTSW AX                    
        SAHF                        
        jbe arctg
                invoke printf, errorMsg
                jmp exit
        arctg:
                TaylorArctg result, t, prec
                invoke printf, sumMsg
                FloatOUTDesc ASNWER, result
        exit:
        ;==============

        invoke getch
        invoke ExitProcess, 0

;-------------------------------
getSign:
        ; Инвертируем sign
        FLD  [sign]                 
        FMUL [MOne]
        FSTP [sign]
        ret
;-------------------------------                                                 
section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll',\
            user32,'USER32.DLL'

include 'api\user32.inc'
include 'api\kernel32.inc'
    import kernel,\
           ExitProcess, 'ExitProcess',\
           HeapCreate,'HeapCreate',\
           HeapAlloc,'HeapAlloc'
    include 'api\kernel32.inc'
    import msvcrt,\
           printf, 'printf',\
           sprintf, 'sprintf',\
           scanf, 'scanf',\
           getch, '_getch'