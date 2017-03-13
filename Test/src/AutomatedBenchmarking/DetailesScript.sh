echo "CPU count, CPU util, NW rx(KBps), NW tx(KBps), NW util, Mem Size, Mem util, Disk util, Page Faults"

#CPU Count
cpuc=`sar -f abc -u | awk 'NR==1{print substr($9,2)}'`

#CPU util
cpuu=`sar -f abc -u | tail -n-1 | awk '{print (100-$8)}'`

#NW
nwc=`sar -f abc -n DEV | tail -n 3 | awk '$2=="eth1" {print $5","$6","$10}'`
nws=`sar -f abc -n DEV | tail -n 3 | awk '$2=="ens3" {print $5","$6","$10}'`

#Mem
mem=`sar -f abc -r | tail -n 1 | awk '{print $2+$3","$4}'`

#Disk
disk=`sar -f abc -d | tail -n 4 | awk '$1=="Average:" && $2=="dev8-0"{print $10}'`

echo $cpuc","$cpuu","$nwc$nws","$mem","$disk
