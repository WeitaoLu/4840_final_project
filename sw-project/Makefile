#include /root/keyboard_versionone/lab2/Makefile
ifneq (${KERNELRELEASE},)

# KERNELRELEASE defined: we are being compiled as part of the Kernel
        obj-m := vga_ball.o

else

# We are being compiled as a module: use the Kernel build system

	KERNEL_SOURCE := /usr/src/linux-headers-$(shell uname -r)
        PWD := $(shell pwd)

default: module hello

hello: hello.o usbkeyboard.o
	cc $(CFLAGS) -o hello hello.o usbkeyboard.o -lusb-1.0 -pthread -lm

hello.o: hello.c usbkeyboard.h
usbkeyboard.o: usbkeyboard.c usbkeyboard.h

module:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules

clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
	${RM} hello

TARFILES = Makefile README vga_ball.h vga_ball.c hello.c usbkeyboard.h usbkeyboard.c
TARFILE = lab3-sw.tar.gz
.PHONY : tar
tar : $(TARFILE)

$(TARFILE) : $(TARFILES)
	tar zcfC $(TARFILE) .. $(TARFILES:%=lab3-sw/%)

endif 
