#!/bin/sh
cd linux-0.11
make clean
make all
cd ..
sudo umount hdc
./run
