/******************************************************************************
 *  String Module
 * this is part of the libdisasm project
 *
 * author: Dmitry Podgorny <pasis.ua@gmail.com>
 * license: GPL
 *****************************************************************************/

#ifndef STRING_H
#define STRING_H

#include "types.h"
#include <string.h>

// заполняет count байт массива значением 0
void	fillzero(void *, int count);
// возвращает указатель на конец строки (на 0-й символ)
char	*strend(char *);
// переводит число в строку (в 16-ричной системе)
void	intstr(uint, char *);

#endif
