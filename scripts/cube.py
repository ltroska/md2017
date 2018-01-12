#!/usr/bin/env python
# vim: et sts=4 ts=4 sw=4 ai

import sys, getopt, csv, os, math, re, tempfile, shutil, exceptions

def usage():
    print "Usage:"
    print sys.argv[0], " experimentname\n"
    print "Options:\n\t-s,--sigma\tsigma [=1]"
    print "\t-e,--epsilon\tepsilon [=120]"
    print "\t-d,--deltat\tdelta t [=0.001]"
    print "\t--end\tend_t[=10]"
    print "\t-n i   \tobject of i*i*i particles [i=8]"
    print "\t-m     \tmass of particles [=1]"
    print "\t-L     \tlength of the domain [=9.4]"

def main(argv):
    try:
        opts, args = getopt.getopt(argv, "he:v:d:s:L:n:", ["help","epsilon=","eps=","deltat=","delta_t=","n=","m=","end="])
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)
    if len(args) != 1:
        print "error: no experiment name given", args
        usage()
        sys.exit(2)
    # default values
    sigma = 1
    epsilon = 1
    delta_t = 0.001
    t_end = 3
    n = 8
    m = 1
    L = 9.4
    for o, a in opts:
        print o
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        if o in ("-s", "--sigma"):
            sigma = float(a)
        if o in ("-e", "--epsilon", "--eps"):
            epsilon = float(a)
        if o in ("-d", "--deltat", "--delta_t"):
            delta_t = float(a)
        if o in ("-n","--n"):
            n = int(a)
        if o == "-m":
            m = float(a)
        if o == "--end":
            t_end = float(a)

    # first argument is the experiment name
    name = args[0]

    # calc derived values
    rcut = sigma*2.5
    #gridstep = pow(2.,1./6.)*sigma
    gridstep = L / n
    lx = L
    ly = L
    lz = L

    # write parameter file
    f = open(name+'.parameter', 'w+')
    f.write("name %s\n" % name)
    f.write("delta_t %g\n" % delta_t)
    f.write("t_end %g\n" % t_end)
    f.write("epsilon %g\n" % epsilon)
    f.write("sigma %g\n" % sigma)
    f.write("cell_r_cut %g\n" % rcut)
    f.write("length %g %g %g\n" % (lx,ly,lz) )
    f.write("upper_border periodic periodic periodic\n")
    f.write("lower_border periodic periodic periodic\n")
    f.write("set_start_temperature %g\n" % 15)
    f.write("thermostat_step_interval %g\n" % 50)
    f.write("thermostat_target_temperature %g\n" % 15)
    f.write("random_seed %g\n" % -1)
    f.close()
    print name+'.parameter done.'
    # write particle file
    count = 0
    # open particle file
    f = open(name+'.particle', 'w+')
    # write object1
    for i in range(0,n):
        x = 0 + i * gridstep;
        for j in range(0,n):
            y = 0 + j * gridstep;
            for k in range(0,n):
                z = 0 + k * gridstep;
                f.write("%i %g %g %g %g %g %g %g\n" % (count,m,x,y,z,0,0,0))
                count+=1
    print name+'.particle done.'
    # close particle file
    f.close()

if __name__ == "__main__":
    main(sys.argv[1:])
