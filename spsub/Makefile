# %Z%%M% Version %I% by IKEDA Mikio %E%
#
CFLAGS       = -g -O
TARGET	      = spsub
LFLAGS        = -lm
CC            = gcc

HDRS	      = ana.h

OBJS	      = spsub.o \
		rfft.o \
		irfft.o \
		cfftall.o \
		getfirst.o \
		rdframe.o \
		multirr.o \
		xd_realloc.o

all:		$(TARGET)

$(TARGET):	$(OBJS)
		@$(CC) -o $(TARGET) $(OBJS) $(LFLAGS)

clean:;		@/bin/rm -f $(TARGET) $(OBJS)

$(OBJS):	$(HDRS)
