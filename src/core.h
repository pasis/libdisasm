/******************************************************************************
 *  Core Module
 * this is part of the libdisasm project
 *
 * author: Dmitry Podgorny <pasis.ua@gmail.com>
 * version: alpha5
 * license: GPL
 *****************************************************************************/

#ifndef CORE_H
#define CORE_H

#include "types.h"

///////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)

//=============================================================================

struct cmd {

	union {

		uint8	cmd;

		struct {
		uint8	size		:1;	// размер
		uint8	direction	:1;	// направление
		uint8	cmd6		:6;	// код операции
		};

		struct {
		uint8	reg		:3;	// регистр
		uint8	cmd6		:5;	// код операции
		};

		struct {
		uint8	inversion	:1;	// инверсия
		uint8	condition	:3;	// условие
		uint8	cmd4		:4;	// код операции
		};

	};

};

//=============================================================================

struct modRM {

	union {

		uint8	modRM;

		struct {
		uint8	RM		:3;	// R/M
		uint8	reg		:3;
		uint8	mod		:2;
		};

	};

};

//=============================================================================

struct sib {

	union {

		uint8	sib;

		struct {
		uint8	base		:3;
		uint8	index		:3;
		uint8	scale		:2;
		};

	};

};

//=============================================================================

#pragma pack(pop)

//=============================================================================

struct command {

	uint		prefix;
	struct cmd	cmd;
	struct modRM	modRM;
	struct sib	sib;
	uint		var;
	uint		offset;
	uint		seg;

};

///////////////////////////////////////////////////////////////////////////////

struct depend {

	uint		use;		// разрядность
	uint		bit_op;		// разрядность операнда
	uint		bit_addr;	// разрядность адреса

	int		db66;		// префикс изменения операнда
	int		db67;		// префикс изменения адреса
	int		seg;		//+ префиксы переопределения сегментных
					//+ регистров (может быть только 1)

};

//=============================================================================

struct opcode {

	uint		code;
	uint		depend;
	char		name[32];

};

///////////////////////////////////////////////////////////////////////////////

#endif
