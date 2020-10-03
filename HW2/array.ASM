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

        i            dd ?    ; Номер
        lastPositive dd ?    ; Последний положительный элемент
        tmp          dd ?    ; Текущий элемент
        vec          rd 100  ; Сам массив
        arrLen       dd 0    ; Длина массива

        NULL = 0


section '.code' code readable executable
;--------------------------------------------------------------------------
        start:
         ; 1. Вводим количество элементов в массиве
                call ArrLength
                endArrLength:
                mov eax, [arrLen]
                mov [lastPositive], eax

                push nextString
                call [printf]
         ; 2. Вводим сам массив и получаем позицию lastPositive
                call ArrayInput

         ; 3. Выведем все элементы
                push allElText
                call [printf]

                call ShowTimeAll
                endShowTimeAll:

         ; 4. Выведем подходящие элементы
                push ansElText
                call [printf]

                call ShowSuitable
                endShowSuitable:

                mov eax, [lastPositive]

         ; 5? [ПРОВЕРКА] на пустой результат

                cmp [arrLen], eax
                jne final

                push strAnswNull
                call [printf]

         ; завершаем
         final:
                call [getch]
                push NULL
                call [ExitProcess]
;--------------------------------------------------------------------------
        ArrLength:
                push wArrLength
                call [printf]
                ; Вводим количество элементов в массиве
                push arrLen
                push formatNum
                call [scanf]
                ; [ПРОВЕРКА] Должно быть положительное число
                cmp [arrLen], 0
                jg  endArrLength

                push strERROR
                call [printf]
                jmp final
;--------------------------------------------------------------------------
        ArrayInput:
                xor ecx, ecx            ; ecx = 0
                mov ebx, vec            ; ebx = &vec

            getVecLoop:                  ; | цикл ввода элементов
                mov [tmp], ebx           ; |
                cmp ecx, [arrLen]        ; | Проверка условий цикла
                jge endInputVector       ; |

                ;=========== Номер элемента
                mov [i], ecx
                push ecx
                push strVecElemI
                call [printf]
                add esp, 8
                ;=========== Ввод элемента
                push ebx
                push strScanInt
                call [scanf]
                add esp, 8

                ;=========== Запоминаем последний положительный номер
                cmp dword [ebx], 0
                JL endPositiveCmp
                        mov eax, [i]
                        inc eax
                        mov [lastPositive], eax

                endPositiveCmp:


                ;                          | Сдвигаем элемент дальше
                mov ecx, [i]
                inc ecx
                mov ebx, [tmp]
                add ebx, 4 ; Сдвигаемся на 4 байта
                jmp getVecLoop
            endInputVector:
                ret
;--------------------------------------------------------------------------
        ShowTimeAll:
                xor ecx, ecx            ; ecx = 0
                mov ebx, vec            ; ebx = &vec

            allVecLoop:
                mov [tmp], ebx      ; |
                mov [i], ecx        ; | Условия на цикл
                ; Выход из цикла    ; |
                cmp ecx, [arrLen]   ; |
                je endAllVector     ; |

                push dword [ebx]   ; Корректный вывод значений
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
                mov [i], ecx        ; | Условия на цикл
                mov ebx, [vec+4*ecx]    ; ebx = vec[lastPositive]


            suitVecLoop:            ; [ FOR ]
                mov [i], ecx        ; | Условия на цикл
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

        ; импортируем функции из kernel
        import kernel,\
               ExitProcess, 'ExitProcess'

        ; импортируем функции из msvcrt
        import msvcrt,\
               printf, 'printf',\
               getch, '_getch',\
               scanf, 'scanf'
