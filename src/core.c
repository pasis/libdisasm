/******************************************************************************
 *  Core Module
 * this is part of the libdisasm project
 *
 * author: Dmitry Podgorny <pasis.ua@gmail.com>
 * version: alpha5
 * license: GPL
 *****************************************************************************/

#include "core.h"
#include "disasm.h"

#include "codes.h"
#include "tables.h"

#include "types.h"
#include "string.h"

///////////////////////////////////////////////////////////////////////////////
// скопировать в строку str необходимый регистр
void get_reg(struct command *cmd, struct depend *dep, uint depend, uint bit, char *str) {

	uint	reg;

	if ( bit == USE16_ONLY )
		bit = USE16;
	else if ( bit != USE8 )
		bit = dep->bit_op;

	if ( depend & ( D_RB | D_RD ) )
		reg = (uint) cmd->cmd.reg;
	else if ( depend & D_MODRM )
		reg = (uint) cmd->modRM.reg;

	strcpy(str, (char *) regs[bit * 8 + reg]);

}

///////////////////////////////////////////////////////////////////////////////
// скопировать в строку str необходимый регистр
void get_rm(struct command *cmd, struct depend *dep, uint depend, uint bit, char *str) {

	if ( bit == USE32_ONLY )
		bit = USE32;
	else if ( bit == USE16_ONLY )
		bit = USE16;
	else if ( bit != USE8 )
		bit = dep->bit_op;

	if ( cmd->modRM.mod == 0x03 ) {
	// если оба операнда - регистры
		strcpy(str, (char *) regs[bit * 8 + (uint) cmd->modRM.RM]);
	}
	else {

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		strcpy(str, (char *) mems[bit]);
		str = strend(str);
		*str++ = '[';

		// если есть префикс замены сегментного регистра, то добавим его
		if ( dep->seg ) {
			strcpy(str, (char *) regs_seg[dep->seg]);
			str = strend(str);
			*str++ = ':';
		}

		// если есть префикс 0x66, то изменяется и способ представления, а не только разрядность
		if ( dep->bit_addr == USE32 ) {
		//=============================================================

			if ( cmd->modRM.RM == 0x04 ) {
			// SIB ************************************************

				strcpy(str, (char *) regs[USE32 * 8 + cmd->sib.base]);
				str = strend(str);
				if ( cmd->sib.index != 0x04 ) {
				// если есть индексный регистр
					*str++ = ' ';
					*str++ = '+';
					*str++ = ' ';
					strcpy(str, (char *) regs_sib[cmd->sib.scale * 8 + cmd->sib.index]);
					str = strend(str);
				}

			}
			else if ( cmd->modRM.RM == 0x05 && cmd->modRM.mod == 0x00 ) {
			// смещение *******************************************
				intstr(cmd->offset, str);
				str = strend(str);
			}
			else {
			// регистр (+ смещение) *******************************

				strcpy(str, (char *) regs[USE32 * 8 + cmd->modRM.RM]);
				str = strend(str);

				//! FIXME: пока выводим 8-битные смещения как 32-х
				if ( cmd->modRM.mod > 0x00 ) {
					*str++ = ' ';	//! заменить
					*str++ = '+';
					*str++ = ' ';
					intstr(cmd->offset, str);
					str = strend(str);
				}

			}

		//-------------------------------------------------------------
		}
		else {

			if ( cmd->modRM.RM == 0x06 && cmd->modRM.mod == 0x00 ) {
			// смещение *******************************************
				//! FIXME: пока выводим как 32-битные
				intstr(cmd->offset, str);
				str = strend(str);
			} else {

				strcpy(str, (char *) mem16[cmd->modRM.RM]);
				str = strend(str);

				//! FIXME: пока выводим все смещения как 32-битные
				if ( cmd->modRM.mod > 0x00 ) {
					*str++ = ' ';
					*str++ = '+';
					*str++ = ' ';
					intstr(cmd->offset, str);
					str = strend(str);
				}

			}

		//=============================================================
		}

		*str++ = ']';
		*str = 0;
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	}

}

///////////////////////////////////////////////////////////////////////////////
//
void get_imm(struct command *cmd, struct depend *dep, uint depend, uint bit, char *str) {

	//! TODO: разрядность и тп...

	if ( bit != USE8 )
		bit = dep->bit_op;

	/*strcpy(str, (char *) mems[bit]);
	str = strend(str);
	*str++ = ' ';*/

	intstr(cmd->var, str);

}

///////////////////////////////////////////////////////////////////////////////
//
void get_rel(struct command *cmd, struct depend *dep, uint depend, uint bit, char *str) {

	//! TODO: разрядность и тп...
	uint	count = 2;	// кол-во байт на команду
	uint	offset = cmd->offset + count;

	if ( bit != USE8 ) {
		bit = dep->bit_addr;
		if ( bit == USE32 )
			count = 5;
		else
			count = 3;
	}

	*str++ = '$';	//! это нужно заменить...
	*str++ = ' ';
	if ( (int) offset < 0 ) {
		*str++ = '-';
		offset = -offset;
	}
	else
		*str++ = '+';
	*str++ = ' ';

	intstr( offset, str);

}

///////////////////////////////////////////////////////////////////////////////
//
void get_ptr(struct command *cmd, struct depend *dep, uint depend, uint bit, char *str) {

	//! TODO: разрядность и тп...
	intstr(cmd->seg, str);
	str = strend(str);
	*str++ = ':';
	intstr(cmd->offset, str);

}

///////////////////////////////////////////////////////////////////////////////
//
void get_sreg(struct command *cmd, struct depend *dep, uint depend, uint bit, char *str) {

	strcpy(str, (char *) regs_seg[cmd->modRM.reg + 1]);

}

///////////////////////////////////////////////////////////////////////////////
//
void get_cr(struct command *cmd, struct depend *dep, uint depend, uint bit, char *str) {

	*str++ = 'c';
	*str++ = 'r';
	*str++ = '0' + cmd->modRM.reg;
	*str = 0;

}

///////////////////////////////////////////////////////////////////////////////
//
void get_dr(struct command *cmd, struct depend *dep, uint depend, uint bit, char *str) {

	*str++ = 'd';
	*str++ = 'r';
	*str++ = '0' + cmd->modRM.reg;
	*str = 0;

}

///////////////////////////////////////////////////////////////////////////////
// поиск опкода в таблице, если находит, то возвращает номер, иначе -1

int findopcode(uint code, uint8 *entry) {

	int		index = 0,
			op;
	uint8		tmp;
	struct modRM	modRM;

	entry++;		// следующий байт за опкодом

	while ( *opcodes[index].name ) {
		op = opcodes[index].code;

		if ( opcodes[index].depend & D_MODRM_RM ) {
			tmp = (uint8) ( ( opcodes[index].depend & D_DIGIT ) >> SIZE_D_DIGIT );
			modRM.modRM = *entry;
			if ( modRM.reg != tmp ) {
				index++;
				continue;
			}
		}

		// если регистр добавляется к опкоду...
		if ( opcodes[index].depend & ( D_RB | D_RD ) ) {
			if ( op == ( code & 0xfffffff8 ) )
				return index;
		} else
		// в остальных случаях...
		if ( op == code )
			return index;

		index++;
	}

	return -1;

}

///////////////////////////////////////////////////////////////////////////////

uint8 *disasm(uint8 *entry, char *str, uint bit, uint offset) {

	struct command	cmd;
	struct depend	dep;
	int		index,
			i, j;		// counters
	int		bool,		// временная переменная...
			bit_addr,	// разрядность адреса (учитывая префиксы)
			bit_op;		// разрядность операндов (учитывая префиксы)
	char		symb,
			s[32];		// временная строка для парсинга

	fillzero(&cmd, sizeof(struct command) );
	fillzero(&dep, sizeof(struct depend) );

	dep.use = bit;		// 16 или 32-битный режим

	//=====================================================================
	/* проверяем префиксы */

	bool = TRUE;		// считаем пока что префикс есть...

	while ( bool ) {

		//-------------------------------------------------------------
		switch ( *entry ) {

		case 0x26:
			dep.seg = 1;	// es
			break;
		case 0x2e:
			dep.seg = 2;	// cs
			break;
		case 0x36:
			dep.seg = 3;	// ss
			break;
		case 0x3e:
			dep.seg = 4;	// ds
			break;
		case 0x64:
			dep.seg = 5;	// fs
			break;
		case 0x65:
			dep.seg = 6;	// gs
			break;

		case 0x66:
			dep.db66 = TRUE;
			break;

		case 0x67:
			dep.db67 = TRUE;
			break;

//!		case ***:
		case 0xd4:
		case 0xd5:
		case 0xf2:
		case 0xf3:
		case 0x0f:
			cmd.prefix = *entry;
			break;

		default:
			bool = FALSE;	// это не префикс...

		}
		//-------------------------------------------------------------

		if ( bool )		// если это был префикс
			entry++;	// то переходим к следующему байту

	}// end while

	//=====================================================================
	// ищем опкод в таблице и тп...

	cmd.cmd.cmd = *entry;

	index = findopcode( cmd.prefix * 0x100 + (uint) cmd.cmd.cmd, entry );
	if ( index == -1 )
		return 0;

	//---------------------------------------------------------------------
	// таблица опкодов для 32-битного режима, потому рассмотрим команды с
	// префиксами 0x66 (например stosw и stosd)
	// в таблице опкодов: 0xPP66CC, где PP - префикс, CC - опкод, 66 - префикс 0x66
	if ( ( bit == USE16 ) && ( i = findopcode( cmd.prefix * 0x10000 + 0x6600 + (uint) cmd.cmd.cmd, entry ) != -1 ) )
		index = i;	// если 16-битный режим...

	if ( ( dep.db66 ) && ( (opcodes[index].depend & (D_RD | D_MODRM | D_MODRM_RM)) == 0 ) ) {

		if ( ( i = findopcode( cmd.prefix * 0x10000 + 0x6600 + (uint) cmd.cmd.cmd, entry ) ) == -1 ) {
			strcpy(str, "db     0x66");
			return entry;
		}

		// в i номер опкода в таблице с префиксом 0x66...
		if ( bit == USE32 )
			index = i;
		else if ( bit == USE16 )
			index = findopcode( (uint) cmd.cmd.cmd, entry );

	}
	//---------------------------------------------------------------------

	if ( ( dep.db67 ) && ( (opcodes[index].depend & (D_CD | D_CP | D_MODRM | D_MODRM_RM)) == 0 ) ) {
		strcpy(str, "db     0x67");
		return entry;
	}

	entry++;	// переходим к следующему байту, если команда принята :)

	//=====================================================================
	// вычислим разрядность операндов и адреса с учетом префиксов

	if ( dep.db66 )
		switch ( bit ) {
		case USE32:
			bit_op = USE16;
			break;
		case USE16:
			bit_op = USE32;
			break;
		}
	else
		bit_op = bit;

	if ( dep.db67 )
		switch ( bit ) {
		case USE32:
			bit_addr = USE16;
			break;
		case USE16:
			bit_addr = USE32;
			break;
		}
	else
		bit_addr = bit;

	dep.bit_op = bit_op;
	dep.bit_addr = bit_addr;

	//=====================================================================
	// смотрим что следует за опкодом в памяти и читаем это

	if ( opcodes[index].depend & ( D_MODRM | D_MODRM_RM ) ) {

		cmd.modRM.modRM = *entry++;

		// проверяем, следует ли байт SIB...
		if ( cmd.modRM.RM == 0x04 && bit_addr == USE32 ) {
			if ( cmd.modRM.mod != 0x03 )	// в случае mod = 11b, 100b - это esp, а не признак SIB
				cmd.sib.sib = *entry++;
		}
		// смотрим, следует ли смещение....
		else
			switch ( cmd.modRM.mod ) {
			//-----------------------------------------------------
			case 0x01:
				cmd.offset = (uint) ( (char) *entry++ );
				break;
			case 0x00:
				if ( !( ( bit_addr == USE32 && cmd.modRM.RM == 0x05 ) || ( bit_addr == USE16 && cmd.modRM.RM == 0x06 ) ) )
					break;
				// иначе продолжаем...
			case 0x02:
				if ( bit_addr == USE32 ) {
					cmd.offset = *( (uint *) entry);
					entry += 4;
				}
				else {
					cmd.offset = (uint) ( (int) *( (short int *) entry) );
					entry += 2;
				}
			//-----------------------------------------------------
			}

	}

	if ( opcodes[index].depend & D_IB )
		cmd.var = (uint) *entry++;

	if ( opcodes[index].depend & D_CB )
		cmd.offset = (uint) ( (int) ( (char) *entry++ ) );

	if ( opcodes[index].depend & D_ID && bit_op == USE32 ) {
		cmd.var = *( (uint *) entry);
		entry += 4;
	}
	else if ( opcodes[index].depend & D_ID && bit_op == USE16 ) {
		cmd.var = (uint) *( (uint16 *) entry);
		entry += 2;
	}

	if ( opcodes[index].depend & D_CD && bit_addr == USE32 ) {
		cmd.offset = *( (uint *) entry);
		entry += 4;
	}
	else if ( opcodes[index].depend & D_CD && bit_addr == USE16 ) {
		cmd.offset = (uint) ( (int) *( (short int *) entry) );
		entry += 2;
	}

	if ( opcodes[index].depend & D_CP ) {

		if ( bit_addr == USE32 ) {
			cmd.offset = *( (uint *) entry);
			entry += 4;
		}
		else if ( bit_addr == USE16 ) {
			cmd.offset = (uint) *( (uint16 *) entry);
			entry += 2;
		}

		cmd.seg = (uint) *( (uint16 *) entry);
		entry += 2;

	}

	//=====================================================================
	// опкод в таблице уже найден... начинаем парсить строку команды

	i = 0;

	do {

		symb = opcodes[index].name[i++];	// сразу и увеличиваем счетчик

		if ( symb == '{' ) {
		//=============================================================
		// нужно заменить "{X}" на соответствуещие значения

			j = 0;		// указатель внутри строки s
			while ( ( symb = opcodes[index].name[i++] ) != '}' )
				s[j++] = symb;
			s[j] = 0;	// конец строки, в s строка между "{" и "}"


			if ( !strcmp(s, O_R8) )
				get_reg(&cmd, &dep, opcodes[index].depend, USE8, s);
			else if ( !strcmp(s, O_R32) )
				get_reg(&cmd, &dep, opcodes[index].depend, bit, s);
			else if ( !strcmp(s, O_R16) )
				get_reg(&cmd, &dep, opcodes[index].depend, USE16_ONLY, s);
			else if ( !strcmp(s, O_RM8) )
				get_rm(&cmd, &dep, opcodes[index].depend, USE8, s);
			else if ( !strcmp(s, O_RM16) )
				get_rm(&cmd, &dep, opcodes[index].depend, USE16_ONLY, s);
			else if ( !strcmp(s, O_RM32) )
				get_rm(&cmd, &dep, opcodes[index].depend, bit, s);
			else if ( !strcmp(s, O_RM32ONLY) )
				get_rm(&cmd, &dep, opcodes[index].depend, USE32_ONLY, s);
			else if ( !strcmp(s, O_IMM8) )
				get_imm(&cmd, &dep, opcodes[index].depend, USE8, s);
			else if ( !strcmp(s, O_IMM32) )
				get_imm(&cmd, &dep, opcodes[index].depend, bit, s);
			else if ( !strcmp(s, O_REL8) )
				get_rel(&cmd, &dep, opcodes[index].depend, USE8, s);
			else if ( !strcmp(s, O_REL32) )
				get_rel(&cmd, &dep, opcodes[index].depend, bit, s);
			else if ( !strcmp(s, O_PTR32) )
				get_ptr(&cmd, &dep, opcodes[index].depend, bit, s);
			else if ( !strcmp(s, O_SREG) )
				get_sreg(&cmd, &dep, opcodes[index].depend, bit, s);
			else if ( !strcmp(s, O_CR) )
				get_cr(&cmd, &dep, opcodes[index].depend, bit, s);
			else if ( !strcmp(s, O_DR) )
				get_dr(&cmd, &dep, opcodes[index].depend, bit, s);

			j = 0;
			do {
				*str++ = s[j++];
			} while ( s[j] );

		//=============================================================
		}
		else
			*str++ = symb;

	} while ( symb );

	return entry;

}
