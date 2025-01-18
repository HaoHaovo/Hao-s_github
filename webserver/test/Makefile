# 编译器和编译选项
CXX = g++
CFLAGS = -std=c++14 -O2 -Wall -g
LDFLAGS = -pthread -lmysqlclient

# 目标文件
TARGET = test

# 使用 wildcard 自动查找所有 .cpp 文件
SRC = $(wildcard ../log/*.cpp) \
      $(wildcard ../pool/*.cpp) \
      $(wildcard ../buffer/*.cpp) \
      ../test/test.cpp
    #   $(wildcard ../timer/*.cpp) \
    #   $(wildcard ../http/*.cpp) \
    #   $(wildcard ../server/*.cpp) \

# 将 .cpp 文件转为 .o 文件
OBJS = $(SRC:.cpp=.o)

# 默认目标
all: $(TARGET)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# 自动生成 .o 文件
%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

# 清理目标文件和可执行文件
clean:
	rm -rf $(OBJS) $(TARGET)

# 打印调试信息，确认 SRC 变量的文件列表
$(info SRC files: $(SRC))
