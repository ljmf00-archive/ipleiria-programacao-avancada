# I love tuna |>().

# Libraries
LIBS=#-lm -pthread

# Compiler flags
CFLAGS=-Wall -Wextra -ggdb -std=c11 -pedantic -D_POSIX_C_SOURCE=200809L #-pg

# Linker flags
LDFLAGS=-pthread

# Indentation flags
# IFLAGS=-br -brs -brf -npsl -ce -cli4 -bli4 -nut
IFLAGS=-linux -brs -brf -br

# Name of the executable
PROGRAM=ping_pong

PROGRAM_OPT=args

PROGRAM_OBJS=main.o

.PHONY: clean all docs indent debug

all: $(PROGRAM)

# activate DEBUG, defining the SHOW_DEBUG macro
debug: CFLAGS += -D SHOW_DEBUG -g
debug: $(PROGRAM)

OPTIMIZE_FLAGS=-O3
optimize: CFLAGS += $(OPTIMIZE_FLAGS)
optimize: LDFLAGS += $(OPTIMIZE_FLAGS)
optimize: $(PROGRAM)

$(PROGRAM): $(PROGRAM_OBJS)
	$(CC) -o $@ $(PROGRAM_OBJS) $(LIBS) $(LDFLAGS)

# Dependencies
main.o: main.c

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o core.* *~ $(PROGRAM) *.bak

docs: Doxyfile
	doxygen Doxyfile

Doxyfile:
	doxygen -g Doxyfile

depend:
	$(CC) -MM *.c

indent:
	indent $(IFLAGS) *.c *.h

pmccabe:
	pmccabe -v *.c

cppcheck:
	cppcheck --enable=all --verbose *.c *.h