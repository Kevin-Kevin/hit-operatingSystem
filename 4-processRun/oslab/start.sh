#!/bin/sh
cd linux-0.11
make all
cd ..
sudo umount hdc
./run
