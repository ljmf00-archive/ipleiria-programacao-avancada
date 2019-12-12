# I love tuna |>().

# Libraries
LIBS=#-lm -pthread

# Compiler flags
CFLAGS=-Wall -Wextra -ggdb -std=c11 -pedantic -D_POSIX_C_SOURCE=200809L #-pg

# Linker flags
LDFLAGS=#-pg

# Indentation flags
# IFLAGS=-br -brs -brf -npsl -ce -cli4 -bli4 -nut
IFLAGS=-linux -brs -brf -br

# Name of the executable
SERVER_PROGRAM=cipher_svr
CLIENT_PROGRAM=cipher_clnt

SERVER_PROGRAM_OPT=server_args
CLIENT_PROGRAM_OPT=client_args

SERVER_PROGRAM_OBJS=server.o $(SERVER_PROGRAM_OPT).o #common.o
CLIENT_PROGRAM_OBJS=client.o $(CLIENT_PROGRAM_OPT).o #common.o

.PHONY: clean all docs indent debug

all: $(SERVER_PROGRAM) $(CLIENT_PROGRAM)

# activate DEBUG, defining the SHOW_DEBUG macro
debug: CFLAGS += -D SHOW_DEBUG -g
debug: $(SERVER_PROGRAM) $(CLIENT_PROGRAM)

OPTIMIZE_FLAGS=-O3
optimize: CFLAGS += $(OPTIMIZE_FLAGS)
optimize: LDFLAGS += $(OPTIMIZE_FLAGS)
optimize: $(SERVER_PROGRAM) $(CLIENT_PROGRAM)

$(SERVER_PROGRAM): $(SERVER_PROGRAM_OBJS)
	$(CC) -o $@ $(SERVER_PROGRAM_OBJS) $(LIBS) $(LDFLAGS)

$(CLIENT_PROGRAM): $(CLIENT_PROGRAM_OBJS)
	$(CC) -o $@ $(CLIENT_PROGRAM_OBJS) $(LIBS) $(LDFLAGS)

# Dependencies
server.o: server.c $(SERVER_PROGRAM_OPT).h common.h
$(SERVER_PROGRAM_OPT).o: $(SERVER_PROGRAM_OPT).c $(SERVER_PROGRAM_OPT).h

$(SERVER_PROGRAM_OPT).o: $(SERVER_PROGRAM_OPT).c $(SERVER_PROGRAM_OPT).h
	$(CC) -ggdb -std=c11 -pedantic -c $<

client.o: client.c $(CLIENT_PROGRAM_OPT).h common.h
$(CLIENT_PROGRAM_OPT).o: $(CLIENT_PROGRAM_OPT).c $(CLIENT_PROGRAM_OPT).h

$(CLIENT_PROGRAM_OPT).o: $(CLIENT_PROGRAM_OPT).c $(CLIENT_PROGRAM_OPT).h
	$(CC) -ggdb -std=c11 -pedantic -c $<

.c.o:
	$(CC) $(CFLAGS) -c $<

$(SERVER_PROGRAM_OPT).c $(SERVER_PROGRAM_OPT).h: $(SERVER_PROGRAM_OPT).ggo
	gengetopt < $(SERVER_PROGRAM_OPT).ggo --file-name=$(SERVER_PROGRAM_OPT)

$(CLIENT_PROGRAM_OPT).c $(CLIENT_PROGRAM_OPT).h: $(CLIENT_PROGRAM_OPT).ggo
	gengetopt < $(CLIENT_PROGRAM_OPT).ggo --file-name=$(CLIENT_PROGRAM_OPT)

clean:
	rm -f *.o core.* *~ *.bak $(SERVER_PROGRAM) $(SERVER_PROGRAM_OPT).h \
	$(SERVER_PROGRAM_OPT).c $(CLIENT_PROGRAM) $(CLIENT_PROGRAM_OPT).h \
	$(CLIENT_PROGRAM_OPT).c

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