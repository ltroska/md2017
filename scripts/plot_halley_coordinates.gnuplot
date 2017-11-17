#!/usr/bin/gnuplot --persist
set style data lines
set size ratio -1
plot ARG1 u 2:3 t "Sonne", \
	ARG1 u 4:5 t "Erde", \
	ARG1 u 6:7 t "Jupiter", \
	ARG1 u 8:9 t "Halley"
