#!/bin/bash

dpath="../REDIS/2017-03-23/TC300/1/"

fstat=$dpath"stats.csv"
rm -f $fstat

filename="perf_data"
#iface="ens3"
iface="eth1"
disk_="dev8-0"

# Rx_KBps , Tx_KBps
sar -f $dpath$filename -n DEV | grep $iface | tail -n 1 | awk '{print $5 "," $6}' >> $fstat

# CPU utilization %
sar -f $dpath$filename -u | tail -n 1 | awk '{print 100-$8}' >> $fstat

# Memory utilization %
sar -f $dpath$filename -r | tail -n 1 | awk '{print $4}' >> $fstat

# Disk rd_KBps , wr_KBps , utilization % [each sector is of 512 Bytes each conversion : $4*512/1024]
sar -f $dpath$filename -d | awk '$1=="Average:" && $2=="'$disk_'" {print $4/2 "," $5/2 "," $10}' >> $fstat
