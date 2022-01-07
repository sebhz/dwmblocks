CC     ?= cc
CFLAGS  = -pedantic -Wall -Wno-deprecated-declarations -Os
LDFLAGS = -lX11

COM=\
	components/datetime \
	components/uptime \
	components/loadavg

all: options dwmblocks

options:
	@echo dwmblocks build options:
	@echo "CFLAGS  = ${CFLAGS}"
	@echo "LDFLAGS = ${LDFLAGS}"
	@echo "CC      = ${CC}"

.c.o:
	$(CC) -o $@ -c $(CFLAGS) $<

dwmblocks: dwmblocks.o $(COM:=.o)
	$(CC) -o $@ $(COM:=.o) dwmblocks.o $(LDFLAGS)

indent:
	indent -braces-on-if-lines --no-tabs --indent-level4 *.c *.h components/*.c

clean:
	rm -f *.o components/*.o *.gch dwmblocks

.PHONY: all options clean
