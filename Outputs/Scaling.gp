set term png
set output "LevelDB_Write_ops.png"


set xlabel "Number of CPUs"
set ylabel "Write Throughput (ops)"
datafile = "LevelDB.csv"
set xtics 1

plot datafile using 1:2 with lines title "Scaling"
pause -1
