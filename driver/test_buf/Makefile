KVER = $(shell uname -r)
KDIR = /lib/modules/$(KVER)/build

kbuild:
	make -C $(KDIR) M=`pwd`

clean:
	make -C $(KDIR) M=`pwd` clean
