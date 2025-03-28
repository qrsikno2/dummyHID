# dummyHID
## Usage

0. you may need to install some packages:

In archlinux: 
```bash
sudo pacman -S gcc make linux-headers
```

1. run the following code:
```bash
bear -- make 
sudo cp ./dummyhid.ko /lib/modules/$(uname -r)/kernel/drivers/hid/
sudo rmmod dummyhid
sudo modprobe dummyhid
```

2. run `sudo dmesg -w` to see the output of the kernel module.

3. remove your device and plug it again.
