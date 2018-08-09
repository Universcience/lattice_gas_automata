CFLAGS=`sdl2-config --cflags` -Wall -Wextra 
LDFLAGS=`sdl2-config --libs` -lm

BINS=hpp_model fhp_model

.PHONY: all clean

all: ${BINS}

clean:
	-rm -f ${BINS}
