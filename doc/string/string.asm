;  String module
; this is part of the libdisasm project
;
; author: Dmitry Podgorny <pasis.ua@gmail.com>
; version: alpha5
; license: GPL

; предполагается что во время работы функций es == ds
; иначе, нужно раскомментировать соответствующие строки кода (учтите, что в таком
; случае код должен выполняться в ring0)
; другое решение пробдемы заключается в использовании string.c (doc/string.c)

format ELF

  public fillzero
  public strcpy
  public strcmp
  public strend
  public intstr

;==============================================================================

macro func name
{
  align 4
  name:
}

;==============================================================================

section '.text' executable

;//////////////////////////////////////////////////////////////////////////////
; void fillzero(void *, int count)
; заполнить count байт значением 0 начиная с адреса
func fillzero

    mov   edx, edi

    mov   edi, [esp + 4]
    mov   ecx, [esp + 8]
;    mov   ax, ds
;    mov   es, ax
    xor   eax, eax
    cld
    rep   stosb

    mov   edi, edx

   ret

;//////////////////////////////////////////////////////////////////////////////
; void strcpy(char *dest, char *src)
; скопировать строку src в dest
func strcpy

    mov   edx, esi		; сохранение

    mov   ebx, [esp + 4]
    mov   esi, [esp + 8]

.cycle:

    lodsb
    mov   byte [ebx], al
    inc   ebx

    test  al, al
    jnz   .cycle

    mov   esi, edx		; восстановление

   ret

;//////////////////////////////////////////////////////////////////////////////
; int strcmp(char *, char *)
func strcmp

    mov   edx, esi		; сохраняем

    mov   esi, [esp + 4]
    mov   ebx, [esp + 8]

.cycle:

    lodsb
    mov   cl, byte [ebx]
    inc   ebx

    cmp   al, cl
    jnz   .quit

    test  al, al
    jnz   .cycle

.quit:

    mov   esi, edx		; восстанавливаем

    sub   al, cl		; ответ
    cbw				;+ переводим в тип int
    cwde			;+

   ret

;//////////////////////////////////////////////////////////////////////////////
; char *strend(char *)
; возвращает указатель на конец строки
func strend

    mov   ebx, [esp + 4]

.cycle:

    mov   al, byte [ebx]
    test  al, al
    jz    .quit

    inc   ebx
    jmp   .cycle

.quit:

    xchg  eax, ebx

   ret

;//////////////////////////////////////////////////////////////////////////////
; void intstr(uint, char *)
; переводит чило (целое, в 16-ричной системе счисления) в строку
func intstr

    mov   eax, [esp + 4]
    mov   ebx, [esp + 8]

    mov   word [ebx], 7830h	; добавляем "0x" (hex)
    add   ebx, 2

    mov   ecx, 8		; кол-во цифр

.cycle:

    mov   edx, eax
    bswap edx			; переворачиваем байты в edx
    shr   dl, 4			; берем только 4 старшие бита (нужная цифра)
    and   edx, 0fh		; оставляем только цифру

    mov   dl, [edx + numbs]	; в dl теперь ascii-код
    mov   byte [ebx], dl

    shl   eax, 4		; умножаем на 16 (убираем старшую цифру)

    inc   ebx			; смещаем указатель на строку

    loop  .cycle

    mov   byte [ebx], 0		; конец строки

   ret

;//////////////////////////////////////////////////////////////////////////////

section '.data' writable

numbs db '0123456789ABCDEF'	;для перевода числа в строку
