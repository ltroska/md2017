#!/usr/bin/gnuplot --persist
set style data lines
set size ratio -1
plot ARG1 u 1:2 t "pot. energy", \
	ARG1 u 1:3 t "kin. energy", \
	ARG1 u 1:4 t "total energy"