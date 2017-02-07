#if [ $# -lt 2 ]
#then
#	echo "IPAddress and Port number required as parameter"
#else
       sport=$3
	sdirp=serverinstance
       for i in `seq 1 $1`
do
sdir=$sdirp"_$i"
	rm -r $sdir
	mkdir -p $sdir
	port=$((i+sport))
#echo $port
	sed -e "s/IPADDRESS/$2/g" -e "s/PORTNUM/$port/g" redis.conf > $sdir/redis.conf
	#cd $sdir
	#../redis-3.2.4/src/redis-server ./redis.conf
done
#fi

