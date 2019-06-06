CFLAGS=`sdl2-config --cflags` -Wall -Wextra
LDFLAGS=`sdl2-config --libs` -lm

BINS=hpp fhp

all: ${BINS}

hpp: lga.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DHPP $^ $(LDFLAGS) -o $@

fhp: lga.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -DFHP $^ $(LDFLAGS) -o $@

.PHONY: all clean

clean:
	-rm -f ${BINS}
