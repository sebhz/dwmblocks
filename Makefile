CC      = gcc
CFLAGS  = -pedantic -Wall -Os
LDFLAGS = -lX11 -lm

COMPONENT_OBJS=\
	components/datetime.o \
	components/uptime.o \
	components/loadavg.o \
	components/battery.o \
	components/sun.o \
	components/util.o

all: options dwmblocks

options:
	@echo dwmblocks build options:
	@echo "CFLAGS  = ${CFLAGS}"
	@echo "LDFLAGS = ${LDFLAGS}"
	@echo "CC      = ${CC}"

dwmblocks: dwmblocks.o $(COMPONENT_OBJS) components/libmeeus.a
	$(CC) -o $@ $^ $(LDFLAGS)

indent:
	indent -braces-on-if-lines --no-tabs --indent-level4 *.c *.h components/*.c

clean:
	rm -f *.o components/*.o  dwmblocks

.PHONY: all options clean
