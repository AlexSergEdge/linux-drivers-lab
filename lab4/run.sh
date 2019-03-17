echo ---Processes before run:---
lsmod | head -5
echo ---Load module:---
sudo insmod hello.ko
echo ---Processes after loading:---
lsmod | head -5
echo ---Remove module---
sudo rmmod hello
echo ---Processes after removing:---
lsmod | head -5
echo ---System messages:---
dmesg | tail -30
