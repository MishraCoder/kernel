KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build
obj-m += mishra_mutex.o

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(shell pwd) modules
modules_install:
	$(MAKE) INSTALL_MOD_STRIP=1 -C $(KERNEL_SRC) M=$(shell pwd) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean

