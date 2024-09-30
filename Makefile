# Makefile for ttynull kernel module

obj-m += ttynull.o

# 获取当前内核的构建目录
KDIR := /lib/modules/$(shell uname -r)/build

# 获取当前目录路径
PWD := $(shell pwd)

# 默认目标：编译模块
all:
	make -C $(KDIR) M=$(PWD) modules

# 清理编译生成的文件
clean:
	make -C $(KDIR) M=$(PWD) clean

# 安装模块（需要超级用户权限）
install:
	sudo insmod ttynull.ko

# 卸载模块（需要超级用户权限）
uninstall:
	sudo rmmod ttynull

# 查看模块信息
info:
	modinfo ttynull.ko
