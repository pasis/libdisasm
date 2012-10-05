/******************************************************************************
 *  String Module
 * this is part of the libdisasm project
 *
 * author: Dmitry Podgorny <pasis.ua@gmail.com>
 * version: alpha5
 * license: GPL
 *****************************************************************************/

#include "string.h"
#include "types.h"

///////////////////////////////////////////////////////////////////////////////

void fillzero(void *dest, int count) {

	for(; count > 0; count--) {
		*( (uint8 *) dest++ ) = 0;
	}

}

///////////////////////////////////////////////////////////////////////////////

void strcpy(char *dest, char *src) {

	while ( *dest++ = *src++ );

}

///////////////////////////////////////////////////////////////////////////////

int strcmp(char *s1, char *s2) {

	while ( *s1 == *s2 && *s1 ) {
		s1++;
		s2++;
	}

	return *s1 - *s2;

}

///////////////////////////////////////////////////////////////////////////////

char *strend(char *s) {

	while ( *s )
		s++;

	return s;

}

///////////////////////////////////////////////////////////////////////////////

char	numb[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void intstr(uint num, char *s) {

	int	i;

	*s++ = '0';
	*s++ = 'x';

	for(i = 0; i < 8; i++) {
		*s++ = numb[ num >> 28 ];
		num = num << 4;
	}

	*s = 0;

}
