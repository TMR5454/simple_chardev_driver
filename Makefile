obj-m := chardrv.o

KERNEL_SRC := /usr/lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

clean clobber:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean

