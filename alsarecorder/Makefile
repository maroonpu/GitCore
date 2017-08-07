#common makefile header
# LIB=/usr/local/lib/libjson.a
# DIR_INC = ./include
# DIR_LIB = ./libs

TARGET	= demo
BIN_TARGET = $(TARGET)

# CROSS_COMPILE = 
CFLAGS = -g -Wall  

#可根据实际需要修改，选择所需平台SDK，默认为x64或x86平台。
# ifdef LINUX64
# LDFLAGS := -L$(DIR_LIB)/x64
# else
# LDFLAGS := -L$(DIR_LIB)/x86 
# endif

LDFLAGS = -lasound -lm

OBJECTS := $(patsubst %.cpp %.c,%.o,$(wildcard *.cpp *.c))

$(BIN_TARGET) : $(OBJECTS)
	g++ $(CFLAGS) $^ -o $@ $(LDFLAGS) 
%.o : %.cpp %.c
	g++ -c $(CFLAGS) $< -o $@
clean:
	@rm -f *.o $(BIN_TARGET)

.PHONY:clean

#common makefile foot
