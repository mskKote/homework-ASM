format PE console

entry start

include 'win32a.inc'

section '.data' data readable writable
        nextString   db 10,0

        allElText    db 10, '//======ALL ELEMENTS======\\', 10, 10, 0
        ansElText    db 10, '//=========ANSWER=========\\', 10, 10, 0
        strAnswNull  db 10, '     No suitable values', 10, 0

        wArrLength   db 'Write array length: ', 0
        strERROR     db 0dh, 0ah,'[ERROR] array length must be greater than zero',0dh, 0ah, 0ah,0
        strRESULT    db 'Positive value: %d', 10, 0

        proof        db '[%d] = %d', 10, 0


        formatNum    db '%d', 0
        strVecElemI  db '[%d]? ', 0
        strScanInt   db '%d', 0
        strVecElemOut  db '[%d] = %d', 10, 0

        i            dd ?    ; �����
        lastPositive dd ?    ; ��������� ������������� �������
        tmp          dd ?    ; ������� �������
        vec          rd 100  ; ��� ������
        arrLen       dd 0    ; ����� �������

        NULL = 0


section '.code' code readable executable
;--------------------------------------------------------------------------
        start:
         ; 1. ������ ���������� ��������� � �������
                call ArrLength
                endArrLength:
                mov eax, [arrLen]
                mov [lastPositive], eax

                push nextString
                call [printf]
         ; 2. ������ ��� ������ � �������� ������� lastPositive
                call ArrayInput

         ; 3. ������� ��� ��������
                push allElText
                call [printf]

                call ShowTimeAll
                endShowTimeAll:

         ; 4. ������� ���������� ��������
                push ansElText
                call [printf]

                call ShowSuitable
                endShowSuitable:

                mov eax, [lastPositive]

         ; 5? [��������] �� ������ ���������

                cmp [arrLen], eax
                jne final

                push strAnswNull
                call [printf]

         ; ���������
         final:
                call [getch]
                push NULL
                call [ExitProcess]
;--------------------------------------------------------------------------
        ArrLength:
                push wArrLength
                call [printf]
                ; ������ ���������� ��������� � �������
                push arrLen
                push formatNum
                call [scanf]
                ; [��������] ������ ���� ������������� �����
                cmp [arrLen], 0
                jg  endArrLength

                push strERROR
                call [printf]
                jmp final
;--------------------------------------------------------------------------
        ArrayInput:
                xor ecx, ecx            ; ecx = 0
                mov ebx, vec            ; ebx = &vec

            getVecLoop:                  ; | ���� ����� ���������
                mov [tmp], ebx           ; |
                cmp ecx, [arrLen]        ; | �������� ������� �����
                jge endInputVector       ; |

                ;=========== ����� ��������
                mov [i], ecx
                push ecx
                push strVecElemI
                call [printf]
                add esp, 8
                ;=========== ���� ��������
                push ebx
                push strScanInt
                call [scanf]
                add esp, 8

                ;=========== ���������� ��������� ������������� �����
                cmp dword [ebx], 0
                JL endPositiveCmp
                        mov eax, [i]
                        inc eax
                        mov [lastPositive], eax

                endPositiveCmp:


                ;                          | �������� ������� ������
                mov ecx, [i]
                inc ecx
                mov ebx, [tmp]
                add ebx, 4 ; ���������� �� 4 �����
                jmp getVecLoop
            endInputVector:
                ret
;--------------------------------------------------------------------------
        ShowTimeAll:
                xor ecx, ecx            ; ecx = 0
                mov ebx, vec            ; ebx = &vec

            allVecLoop:
                mov [tmp], ebx      ; |
                mov [i], ecx        ; | ������� �� ����
                ; ����� �� �����    ; |
                cmp ecx, [arrLen]   ; |
                je endAllVector     ; |

                push dword [ebx]   ; ���������� ����� ��������
                push [i]
                push proof
                call [printf]


                mov ecx, [i]
                inc ecx
                mov ebx, [tmp]
                add ebx, 4
                jmp allVecLoop
            endAllVector:
                jmp endShowTimeAll
;--------------------------------------------------------------------------
        ShowSuitable:
                mov ecx, [lastPositive] ; ecx = lastPositive
                mov [i], ecx        ; | ������� �� ����
                mov ebx, [vec+4*ecx]    ; ebx = vec[lastPositive]


            suitVecLoop:            ; [ FOR ]
                mov [i], ecx        ; | ������� �� ����
                cmp ecx, [arrLen]   ; |
                jge endSuitVector   ; |
                mov ebx, [vec+4*ecx]; |

                push ebx
                push ecx
                push proof
                call [printf]

                mov ecx, [i]
                inc ecx

               jmp suitVecLoop

            endSuitVector:
                jmp endShowSuitable
;--------------------------------------------------------------------------

section '.idata' import data readable writeable
        library kernel, 'KERNEL32.DLL',\
        msvcrt, 'msvcrt.dll'

        ; ����������� ������� �� kernel
        import kernel,\
               ExitProcess, 'ExitProcess'

        ; ����������� ������� �� msvcrt
        import msvcrt,\
               printf, 'printf',\
               getch, '_getch',\
               scanf, 'scanf'
