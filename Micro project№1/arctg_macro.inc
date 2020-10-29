macro FloatOUT float {
  invoke printf, formatFloat, dword[float], dword[float+4]
}

macro FloatOUTDesc desc, float {
  invoke printf, desc, dword[float], dword[float+4]
}

macro FloatOUTSign float {
  FLD  [float]                 
  FCOM [nullify]
  FSTSW AX                    
  SAHF                        
  jb printMINUS
    invoke printf, PLUS
    jmp printNUM
  printMINUS:
    invoke printf, MINUS
  printNUM:
  FSTP [float]
  FloatOUT float
}
;----------------------------------------Taylor

; Главный метод
macro TaylorArctg result, t, precision {

  invoke printf, taylorMsg
  invoke printf, nextSTR
  ; С БОЛЬШОЙ ВЕРОЯТНОСТЬЮ НЕ РАБОТАЕТ СРАВНЕНИЕ
  TaylorFOR:                                 ; [ FOR ]
    FLD  [currentPrec]                             ; |
    FCOMP [prec]              ;   Условие на цикл  ; |
    FSTSW AX                                       ; |
    SAHF                                           ; |
    jle endTaylorFOR                               ; |
    ;===================== Подсчёты
    ; Устанавливаем предыдущие значения 
    ; Получаем все части уравнения
    CalcN iteration, n   ; Вычисляем n
    call getSign         ; Получили знак sign     
    CalculateNext currValue, n, currValueT, sign; 
    FloatOUTSign currValue
    
    powT2 currValueT, t  ; Получаем следующую T
    ; Записываю предыдущий result  
    FLD  [result]
    FSTP [prevResult]
    ; Изменяю result
    FLD  [result]
    FADD [currValue]
    FSTP [result] 
    ;===================== Вычисление точности
    FLD  [iteration]                 
    FCOM [nullify]
    FSTSW AX               
    SAHF                        
    jle NoAccure
      CalcAccuracy prevResult, result, currentPrec
      ; FloatOUTDesc formatTest, iteration
      ; FloatOUTDesc formatTest, currentPrec
    NoAccure:

    ;===================== Вывод 
    FLD  [iteration]
    FADD [one]
    FSTP [iteration]
    invoke printf, nextSTR 
    jmp TaylorFOR
  endTaylorFOR:
}

; Вычисляем N
macro CalcN iteration, n  { ; 2n + 1
  ; В обще можно было просто 2-ку прибавлять
  FLD  [iteration]
  FMUL [two]
  FADD [one]
  FSTP [n]
}

; Вычисляем степень t
macro powT2 resultT, T {;, N, previousN {
  FLD  [resultT]
  FMUL [T]
  FMUL [T]
  FSTP [resultT]
}

; Собираем воедино
macro CalculateNext nextValue, valueByN, nextT, sign {
  ; nextValue = [-+]sign * nextT /valueByN

  ; FloatOUTDesc formatTest, nextT
  ; FloatOUTDesc formatTest, sign
  ; FloatOUTDesc formatTest, valueByN

  FLD  [nextT]
  FMUL [sign]
  FDIV [valueByN]
  FSTP [nextValue]
}

; Работаем, пока не сравнялись с точность === НЕ работает
macro CalcAccuracy prev, curr, currentPrec {
  ; Получаем разницу текущего и предыдущего
  ; FloatOUTDesc formatTest, prev
  ; FloatOUTDesc formatTest, curr

  FLD  [curr]
  FSUB [prev]
  FABS
  FDIV [prev]
  FSTP [currentPrec]
}
