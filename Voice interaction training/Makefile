#common makefile header
LIB=/usr/local/lib/libjson.a
DIR_INC = ./include
DIR_LIB = ./libs

TARGET	= demo
BIN_TARGET = $(TARGET)

CROSS_COMPILE = 
CFLAGS = -g -Wall -I$(DIR_INC) 

#可根据实际需要修改，选择所需平台SDK，默认为x64或x86平台。
ifdef LINUX64
LDFLAGS := -L$(DIR_LIB)/x64
else
LDFLAGS := -L$(DIR_LIB)/x86 
endif

LDFLAGS += -lmsc -lrt -ldl -lpthread -lasound

OBJECTS := $(patsubst %.cpp %.c,%.o,$(wildcard *.cpp *.c))

$(BIN_TARGET) : $(OBJECTS)
	$(CROSS_COMPILE)g++ $(CFLAGS) $^ -lalut -lopenal -o $@ $(LDFLAGS) $(LIB)

%.o : %.cpp %.c
	$(CROSS_COMPILE)g++ -c $(CFLAGS) $< -lalut -lopenal -o $@
clean:
	@rm -f *.o $(BIN_TARGET)

.PHONY:clean

#common makefile foot
