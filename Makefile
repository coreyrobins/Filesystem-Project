CC = gcc
CFLAGS = -ansi -pedantic-errors -Wall -Werror -g
PROGS = public01 public02 public03 public04 public05 driver

.PHONY: all clean

all: $(PROGS)

driver: driver.o filesystem.o memory-checking.o
	$(CC) -o driver driver.o filesystem.o memory-checking.o

filesystem.o: filesystem.c filesystem.h file-system-internals.h
	$(CC) $(CFLAGS) -c filesystem.c

driver.o: driver.c filesystem.h file-system-internals.h memory-checking.h
	$(CC) $(CFLAGS) -c driver.c

public01.o: public01.c filesystem.h file-system-internals.h memory-checking.h
	$(CC) $(CFLAGS) -c public01.c

public02.o: public02.c filesystem.h file-system-internals.h memory-checking.h
	$(CC) $(CFLAGS) -c public02.c

public03.o: public03.c filesystem.h file-system-internals.h memory-checking.h
	$(CC) $(CFLAGS) -c public03.c

public04.o: public04.c filesystem.h file-system-internals.h memory-checking.h
	$(CC) $(CFLAGS) -c public04.c

public05.o: public05.c filesystem.h file-system-internals.h memory-checking.h
	$(CC) $(CFLAGS) -c public05.c

public01: driver.o filesystem.o public01.o memory-checking.o
	$(CC) -o public01 public01.o filesystem.o memory-checking.o

public02: driver.o filesystem.o public02.o memory-checking.o
	$(CC) -o public02 public02.o filesystem.o memory-checking.o

public03: driver.o filesystem.o public03.o memory-checking.o
	$(CC) -o public03 public03.o filesystem.o memory-checking.o

public04: driver.o filesystem.o public04.o memory-checking.o
	$(CC) -o public04 public04.o filesystem.o memory-checking.o

public05: driver.o filesystem.o public05.o memory-checking.o
	$(CC) -o public05 public05.o filesystem.o memory-checking.o

test%: test%.o filesystem.o
	$(CC) $(CFLAGS) -o $@ $< filesystem.o

test%.o: test%.c filesystem.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f public01.o public02.o public03.o public04.o public05.o driver.o filesystem.o $(PROGS)
