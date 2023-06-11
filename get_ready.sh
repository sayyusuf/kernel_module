#!/bin/bash

apt-get update  -y&& \
apt-get install build-essential kmod  -y && \
apt-cache search linux-headers-`uname -r` &&  \
sudo apt-get install -y linux-headers-`uname -r`
