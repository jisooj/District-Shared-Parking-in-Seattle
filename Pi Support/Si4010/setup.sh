#!/bin/bash
echo Setting up C2 Programmer...
sudo insmod c2_gpio.ko
sudo mknod /dev/c2_bus c 243 0
sudo chmod 777 /dev/c2_bus
./siprog -d c2drv:///dev/c2_bus reset identify

