obj-m := my_device.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Tell the kernel build system which compiler to use
# CC := gcc-11

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

