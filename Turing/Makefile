CC = g++

LIB=/usr/local/lib/libjson.a

CFLAGS = -Wall -O2

TARGET = demo

# 表示获取当前目录下的cpp文件集，放在变量SRCS中
SRCS := $(wildcard *.cpp)         

# 表示将对应的cpp文件名转换为o文件名，放在变量OBJS中
OBJS :=$(patsubst %cpp, %o, $(SRCS))        

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)  

%.o:%.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGET)


# $@ 表示目标文件
# $^  表示所有依赖项
# $<  表示第一个文件


