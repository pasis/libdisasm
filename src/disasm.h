/******************************************************************************
 *  libdisasm
 * this is part of the libdisasm project
 *
 * author: Dmitry Podgorny <pasis.ua@gmail.com>
 * license: GPL
 *****************************************************************************/

#ifndef _DISASM_H
#define _DISASM_H

#include "types.h"

uint8	*disasm(uint8 *entry, char *str, uint bit, uint offset);

#define USE8		0x00
#define USE16		0x01
#define USE16_ONLY	0x03
#define USE32		0x02
#define USE32_ONLY	0x04

#endif
