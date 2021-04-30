obj-m := chardrv.o
KO := $(obj-m:%.o=%.ko)

TARGET := $(KO) testprog

KERNEL_SRC := /usr/lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all: $(TARGET)

$(KO):
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

testprog:

clean clobber:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean
	rm -f testprog

test:
	@echo "**** exec test program!! ****"
	sudo rm -f /dev/simplechar
	sudo rmmod $(KO)|:
	sudo insmod $(KO)
	sudo mknod /dev/simplechar c `grep simple_char /proc/devices | awk '{print $$1}'` 1
	sudo chmod a+rw /dev/simplechar
	./testprog /dev/simplechar
	sudo rm -f /dev/simplechar
	sudo rmmod $(KO)
	@echo "**** finish test successfully!!!! ****"

auto:
	$(MAKE) clean
	$(MAKE)
	$(MAKE) testprog
	$(MAKE) test

