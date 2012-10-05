/******************************************************************************
 *  Демонстрационная оболочка
 * this is part of the libdisasm project
 *
 * author: Dmitry Podgorny <pasis.ua@gmail.com>
 * version: alpha5
 * license: GPL
 *****************************************************************************/

#include "disasm.h"
#include "string.h"

#include <stdio.h>
#include <stdlib.h>

void usage(char *s) {

	printf("Usage: %s [OPTIONS]\n\nOPTIONS:\n  -i filename\n  -o filename\n  --use16\n  --use32\n  --org offset\n  --help\n", s);

}

int main(int argc, char **argv) {

	char	s[64];		// строка (для вывода кода)
	uint8	*code,		// для загрузки программы
		*index,		// указатель на текущую команду
		*tmp;		// для временного хранения указателя
	int	len,		// размер файла
		ifindex = 0,	// индекс имени для чтения
		ofindex = 0,	// индекс имени для записи
		i;
	uint	bit = USE32,
		offset = 0;

	FILE	*f, *of = NULL;

	//=====================================================================

	i = 1;		// индекс в argv
	while ( i < argc ) {

		if ( !strcmp(argv[i], "--help") ) {
			usage(argv[0]);
			return 0;
		} else

		if ( !strcmp(argv[i], "-i") ) {
			if ( i == argc - 1 ) {
				usage(argv[0]);
				return 2;
			}
			ifindex = ++i;
		} else

		if ( !strcmp(argv[i], "-o") ) {
			if ( i == argc - 1 ) {
				usage(argv[0]);
				return 2;
			}
			ofindex = ++i;
		} else

		if ( !strcmp(argv[i], "--use16") )
			bit = USE16;
		else
		if ( !strcmp(argv[i], "--use32") )
			bit = USE32;
		else

		if ( !strcmp(argv[i], "--org") ) {
			if ( i == argc - 1 ) {
				usage(argv[0]);
				return 2;
			}
			offset = atoi(argv[++i]);
		}

		else {
			usage(argv[0]);
			return 3;
		}

		i++;

	}

	//=====================================================================

	if ( ( f = fopen(argv[ifindex], "rb") ) == NULL ) {
		fprintf(stderr, "error while opening file \"%s\"!\n", argv[ifindex]);
		return 2;
	}

	if ( ofindex )
		if ( ( of = freopen(argv[ofindex], "w", stdout) ) == NULL ) {
			fprintf(stderr, "error while creating file \"%s\"!\n", argv[ofindex]);
			fclose(f);
			return 2;
		}

	fseek(f, 0, SEEK_END);
	len = ftell(f);				// размер файла
	fseek(f, 0, SEEK_SET);

	index = code = (uint8 *) malloc(len + 8);// с запасом )))

	fread(code, 1, len, f);
	fclose(f);

	//=====================================================================

	switch ( bit ) {
	case USE16:
		printf(" use16\n");
		break;
	case USE32:
	default:
		printf(" use32\n");
		break;
	}

	printf(" org 0x%x\n\n", offset);

	while ( (uint) index < (uint) code + len ) {

		tmp = disasm(index, s, bit, offset + ( (uint) index - (uint) (code) ));
		if ( !tmp ) {
			fprintf(stderr, "error: unknown opcode!\nstring: 0x%x 0x%x 0x%x 0x%x 0x%x\n", *index++, *index++, *index++, *index++, *index);
			return 1;
		}
		printf("%s\n", s);
		index = tmp;
	}

	//=====================================================================

	free(code);
	if ( of != NULL )
		fclose(of);

	return 0;

}
