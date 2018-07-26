#! /bin/bash
# . /opt/intel/bin/compilervars.sh intel64
# export LD_LIBRARY_PATH=/usr/local/openmpi/openmpi-3.0.0/lib:$LD_LIBRARY_PATH

mpiexec ./UPPE-v1-3-0 -cf ./config.txt
