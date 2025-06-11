CC=gcc
CFLAGS=-std=c99 -Wextra -Wall -Werror -pedantic 

ECHO = 
ifeq ($(VERBOSE),no)
	ECHO=@
endif

ifeq ($(DEBUG),no)
	CFLAGS += -O3 -DNDEBUG
else
	CFLAGS += -g
endif

LIB=liblab2.a
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all: $(LIB)


$(LIB): $(OBJ)
	$(ECHO)ar rcs $@ $^

%.o: %.c
	$(ECHO)$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper doc

clean:
	$(ECHO)rm -rf *.o

mrproper: clean
	$(ECHO)rm -rf $(LIB) documentation/html

doc: stack.h token.h queue.h
	$(ECHO)doxygen documentation/liblab2
	
