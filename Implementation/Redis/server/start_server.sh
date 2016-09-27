if [ $# -lt 1 ]
then
	echo "Port number required as parameter"
else
	sdir=serverinstance
	mkdir -p $sdir
	sed -e "s/PORTNUM/$1/g" redis.conf > $sdir/redis.conf
	cd $sdir
	../redis-3.2.4/src/redis-server ./redis.conf
fi
