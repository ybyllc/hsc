# 1. 变量定义
CXX = g++
CXXFLAGS = -Wall -std=c++11 -g
# -g 是为了以后用 gdb 调试用的，习惯加上

TARGET = my_app

# 2. 自动查找当前目录下所有的 .cpp 文件
SRCS := 2.cpp
# SRCS := $(wildcard *.cpp)

# 3. 字符串替换，把 .cpp 替换成 .o
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

# 4. 依赖文件（.d 文件，用来记录头文件依赖）
DEPS := $(OBJS:.o=.d)

# 5. 默认目标
all: $(TARGET)

# 6. 链接规则 ($^ 表示所有依赖项，$@ 表示目标项)
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 7. 编译规则（核心魔法在这里）
# -MMD -MP：让 g++ 在编译的同时，自动生成 .d 依赖文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# 8. 自动引入生成的 .d 依赖文件
# 如果文件存在就引入，不存在就忽略（- 的作用），第一次make时不会报错
-include $(DEPS)

# 9. 清理
clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

.PHONY: all clean
