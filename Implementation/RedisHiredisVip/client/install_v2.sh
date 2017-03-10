#!/bin/bash

#Get dependencis
git clone https://github.com/vipshop/hiredis-vip.git 2>/dev/null
cd hiredis-vip
make
#sudo make install
cd -

#Install src_v2
cd "src_v2"
sudo make
cd -


