CC=gcc
CFLAGS=-Wall -DREENTRANT
LDFLAGS=-lX11 -lpulse -lstdc++

COM=\
	components/datetime \
	components/uptime \
	components/loadavg

all: dwmblocks

.c.o:
	$(CC) -o $@ -c $(CFLAGS) $<

dwmblocks: dwmblocks.o $(COM:=.o)
	$(CC) -o $@ $(COM:=.o) dwmblocks.o $(LDFLAGS)

clean:
	rm -f *.o components/*.o *.gch dwmblocks
