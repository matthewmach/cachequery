TARGET = mod_cachequery

KERNEL := /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

SOURCES := $(addprefix src/, main.c msrdrv.c msrdrv-aarch64.c parser.c cache.c config.c histogram.c lists.c)
$(TARGET)-objs += $(SOURCES:.c=.o)

# fix error with new gcc version
ccflags-y += -I/usr/lib/gcc/aarch64-linux-gnu/12/include
ccflags-y += -Wall

obj-m := $(TARGET).o

# list of supported cpus: i7-4790 i5-6500 i7-8550u
cpu ?= bcm2712
EXTRA_CFLAGS += -DCPU=$(cpu)

all:
	make -C $(KERNEL) M=$(PWD) modules
	cp *.ko $(PWD)/out

qemu:
	make -C ../../linux/obj/linux-x86-basic M=$(PWD) modules
	cp *.ko $(PWD)/out

clean:
	make -C $(KERNEL) M=$(PWD) clean

install:
	sudo rmmod mod_cachequery || true
	sudo insmod mod_cachequery.ko
	sudo chown -R $(USER) /sys/kernel/cachequery/

uninstall:
	sudo rmmod mod_cachequery
