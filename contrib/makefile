CC = g++
CPPFLAGS += $(shell pkg-config --cflags libpulse)
LDFLAGS += $(shell pkg-config --libs libpulse)

pulse_monitor: pulse_monitor.o
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm  -f *.o pulse_monitor
