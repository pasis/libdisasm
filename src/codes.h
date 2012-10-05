/******************************************************************************
 *  Constants
 * this is part of the libdisasm project
 *
 * author: Dmitry Podgorny <pasis.ua@gmail.com>
 * version: alpha5
 * license: GPL
 *****************************************************************************/

#ifndef CODES_H
#define CODES_H

#define EAX	0x00
#define ECX	0x01
#define EDX	0x02
#define EBX	0x03
#define ESP	0x04
#define EBP	0x05
#define ESI	0x06
#define EDI	0x07
#define AX	0x00
#define CX	0x01
#define DX	0x02
#define BX	0x03
#define SP	0x04
#define BP	0x05
#define SI	0x06
#define DI	0x07
#define AL	0x00
#define CL	0x01
#define DL	0x02
#define BL	0x03
#define AH	0x04
#define CH	0x05
#define DH	0x06
#define BH	0x07

#define CS	0x2e
#define SS	0x36
#define DS	0x3e
#define ES	0x26
#define FS	0x64
#define GS	0x65

// for "depend"
#define D_MODRM		0x0001
#define D_MODRM_RM	0x0002
#define D_CB		0x0004
#define D_CD		0x0008
#define D_CP		0x0010
#define D_IB		0x0020
#define D_ID		0x0040
#define D_RB		0x0080
#define D_RD		0x0100
#define D_I		0x0200
#define D_0		0x00000
#define D_1		0x10000
#define D_2		0x20000
#define D_3		0x30000
#define D_4		0x40000
#define D_5		0x50000
#define D_6		0x60000
#define D_7		0x70000
#define D_DIGIT		0x70000
#define SIZE_D_DIGIT	16

// for operands
#define O_R8		"r8"
#define O_R16		"r16"
#define O_R32		"r32"
#define O_RM8		"r/m8"
#define O_RM16		"r/m16"
#define O_RM32		"r/m32"
#define O_RM32ONLY	"r/m32!"
#define O_IMM8		"imm8"
#define O_IMM16		"imm16"
#define O_IMM32		"imm32"
#define O_REL8		"rel8"
#define O_REL32		"rel32"
#define O_PTR32		"ptr16:32"
#define O_SREG		"sreg"
#define O_CR		"cr"
#define O_DR		"dr"

//! moved to disasm.h ...
// #define USE8		0x00
// #define USE16	0x01
// #define USE16_ONLY	0x03
// #define USE32	0x02

#endif
