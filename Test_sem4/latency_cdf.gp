#set term 
#set size


set datafile separator ","

set xlabel "Latency (in usec)"
set ylabel "CDF (Request Count)"

datafile = "abc"
#skipfile=system("awk -F, '(NR>5){print (FNR-5)\",\" $2}' abc")
skipfile="<awk -F, '(NR>5){print (FNR-5)\",\" $2}' abc"


plot skipfile using 2:1 with lines title "CDF"
pause -1
