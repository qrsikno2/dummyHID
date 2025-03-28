obj-m += dummyhid.o
all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules  # ✅ 用 Tab 缩进
clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean    # ✅ 用 Tab 缩进
