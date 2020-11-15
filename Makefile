obj-m+=sort.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	$(CC) testsort.c -o testsort
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm teststort

load:
	insmod sort.ko

rm:
	rmmod sort.ko	