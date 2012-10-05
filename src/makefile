
###############################################################################

CC		= gcc
LD		= ld
AR		= ar
RANLIB		= ranlib

CFLAGS		= -g
ARFLAGS		= rc

###############################################################################

TMP		= .lib

OBJSNAMES	= core.o string.o
STATIC		= $(TMP)/libdisasm.a

TARGET		= $(TMP)/disasm

OBJS		= $(addprefix $(TMP)/, $(OBJSNAMES))

###############################################################################

.PHONY: all static sample clean isdir

all: isdir static sample

sample:

		if [ -f main.c ]; then $(CC) $(CFLAGS) -L.lib -o $(TARGET) main.c -ldisasm; fi


static: $(OBJS)

		$(AR) $(ARFLAGS) $(STATIC) $(OBJS)
		$(RANLIB) $(STATIC)

###############################################################################

$(TMP)/%.o: %.c %.asm

		$(CC) -c $(CFLAGS) -o $@.c $*.c
		fasm $*.asm $@.asm
		$(LD) -r -o $@ $@.c $@.asm

$(TMP)/%.o: %.c

		$(CC) -c $(CFLAGS) -o $@ $<

$(TMP)/%.o: %.asm

		fasm $< $@

###############################################################################

isdir:

		@if [ ! -d $(TMP) ]; then ( mkdir $(TMP) ); fi

clean:

		rm -f $(OBJS) $(TARGET) $(STATIC)
		rmdir $(TMP)
