## 项目结构
### 根目录：
-	makefile 整个项目编译的入口脚本
-	config.mk 配置脚本，被makefile包含
-	common.mk 核心的编译脚本，定义makefile的编译规则，依赖规则，各个子目录用的该脚本实现编译

### 每个子目录：
-	都有一个makefile，包含common.mk



---------------------------
## 内存泄露的检查工具 valgrind
### 一， 内存泄露检查
### 1，memcheck目的
	- 使用未初始化内存
	- 使用已经释放的内存
	- 使用超过分配空间的内存
	- 申请的内存是否释放
	- malloc/free new/delete配对使用
	- memcpy内存拷贝函数的元指针和目标指针重叠
	- 对堆栈的非法访问

### 2，示例：
	格式： valgrind --tool=memcheck 开关 可执行程序
		--tool=memcheck： 使用memcheck工具
		--leak-check=full: 完全full检查内存泄露
		--show-reachabl=yes: 显示内存泄露的地点
		--trace-children=yes: 是否跟入子进程
		--log-file=log.txt: 将调试信息输出到log.txt,不在屏幕显示
	建议：valgrind --tool=memcheck --leak-check=full --show-reachabl=yes ./nginx