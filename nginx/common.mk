#.PHONY:all clean

ifeq ($(DEBUG), true)
CC = g++ -std=c++11 -g
VERSION = debug
else
CC = g++ -std=c++11
VERSION = release
endif

# $(wildcard *.c) 表示扫描当前目录下所有的.c文件
SRCS = $(wildcard *.cpp)

# OBJS = nginx.o ngx_conf.o, 把.c替换为.o
OBJS = $(SRCS:.cpp=.o)

# 把字符串中的.c替换为.d
DEPS = $(SRCS:.cpp=.d)

# 指定BIN文件的位置
BIN := $(addprefix $(BUILD_ROOT)/,$(BIN))

# 指定统一的生成文件，方便连接
LINK_OBJ_DIR = $(BUILD_ROOT)/app/link
DEP_DIR = $(BUILD_ROOT)/app/dep

# -p 递归创建目录，如果不存在就创建目录
$(shell mkdir -p $(LINK_OBJ_DIR))
$(shell mkdir -p $(DEP_DIR))

# nginx.o ngx_conf.o 转为带完整路径的.o /mnt/hgfs/D/Git_Repo/linux/app/link/nginx.o
OBJS := $(addprefix $(LINK_OBJ_DIR)/,$(OBJS))
DEPS := $(addprefix $(DEP_DIR)/,$(DEPS))

# 找到所有的.o编译处理
LINK_OBJ = $(wildcard $(LINK_OBJ_DIR)/*.o)
# 可能app下的.o还没有构建出来
LINK_OBJ += $(OBJS)

all:$(DEPS) $(OBJS) $(BIN)

ifneq ("$(wildcard $(DEPS))","")
include $(DEPS)
endif

# $(warning xxxx1)
# -----------------1 begin-----------------------------
# 生成可执行文件
# $@： 目标
# $^： 所有目标依赖
$(BIN):$(LINK_OBJ)
	@echo "------------------------ build $(VERSION) mode -------------------------!!!"
	$(CC) -o $@ $^
# -----------------1 end-----------------------------

# -----------------2 begin-----------------------------
# %.o:%.c 生成.o的目标文件
$(LINK_OBJ_DIR)/%.o:%.cpp
	$(CC) -I$(INCLUDE_PATH) -o $@ -c $(filter %cpp,$^)
# -----------------2 end-----------------------------

# -----------------3 begin-----------------------------
# 用 gcc -MM c程序文件名 可以将.o依赖的.h文件保存到.d文件中
$(DEP_DIR)/%.d:%.cpp
	echo -n $(LINK_OBJ_DIR)/ > $@
	gcc -I$(INCLUDE_PATH) -MM $^ >> $@
# -----------------3 end-----------------------------
