set term png
set output "leveldb_writeonly.png"
#set size


set datafile separator ","


datafile = "lwriteonly"
set xlabel "Latency (in ms)"
set ylabel "CDF (Request Count)"
set ytics 0.1
set xtics 2
set xrange [0:20]
skipfile="<awk -F, '(NR>5){print (FNR-5)\",\" $2}' ".datafile
stats skipfile using 1 name "A" nooutput

plot skipfile using ($2/1000):($1/A_max) with lines title "CDF"
#pause -1
