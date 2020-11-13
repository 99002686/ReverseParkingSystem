obj-m = usbCam.o
KVERSION = $(shell uname -r)
build:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
