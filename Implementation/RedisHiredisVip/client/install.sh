#!/bin/bash
git clone https://github.com/vipshop/hiredis-vip.git
cd hiredis-vip
make
sudo make install
LD_LIBRARY_PATH=/usr/local/lib
export $LD_LIBRARY_PATH

