CC=gcc
CFLAGS=-std=c99 -Wextra -Wall -Werror -pedantic
LDFLAGS=-lm

ECHO = @
ifeq ($(VERBOSE),yes)
	ECHO=
endif

ifeq ($(DEBUG),no)
	CFLAGS += -O3 -DNDEBUG
	LDFLAGS +=
else
	CFLAGS += -g 
	LDFLAGS +=
endif

EXEC=skiplisttest
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all:
ifeq ($(DEBUG),yes)
	@echo "Generating in debug mode"
else
	@echo "Generating in release mode"
endif
	@$(MAKE) $(EXEC)

$(EXEC): $(OBJ)
	$(ECHO)$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(ECHO)$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	$(ECHO)rm -rf *.o

mrproper: clean
	$(ECHO)rm -rf $(EXEC) documentation/html

doc: rng.h skiplist.h
	$(ECHO)doxygen documentation/TP4


tests : $(EXEC)
	$(ECHO)$(BASH) ../Test/test_script.sh $(EXEC)

rng.o : rng.h
skiplist.o : skiplist.h rng.h
skiplisttest.o : skiplist.h rng.h
doc : rng.h skiplist.h
