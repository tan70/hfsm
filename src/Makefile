# makefile for hfsm sample implementation library.

include ../config.mk

TARGET = lib$(NAME).a

INCS = -I. -I../include
OPT_WARN = -Wall -Werror
OPT_OPTIM = -Og
OPT_DBG = -g
OPT_DEP = -MMD -MP
EXTRA_DEFS =

OPTS = $(OPT_WARN) $(OPT_OPTIM) $(OPT_DBG) $(OPT_DEP)
CFLAGS = $(OPTS) $(INCS)
CPPFLAGS = $(EXTRA_DEFS)
LDFLAGS = -X -r
LIBS =

SRCS = hfsm.c
DEPS = $(SRCS:.c=.d)
OBJS = $(SRCS:.c=.o)

.PHONY: all $(TARGET) clean

%.o: %.c
	$(QCC)$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

all: $(TARGET)

$(TARGET): $(OBJS)
	$(QLD)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	$(QCLEAN)rm -rf $(OBJS) $(DEPS) $(TARGET)

-include $(DEPS)