#! /bin/bash
# . /opt/intel/bin/compilervars.sh intel64
# export LD_LIBRARY_PATH=/usr/local/openmpi/openmpi-3.0.0/lib:$LD_LIBRARY_PATH

mpiexec ./XNLO-v1-1-0 -cf ./higherResolution/800nm_ourLaser/50fs/0.5W.txt
mpiexec ./XNLO-v1-1-0 -cf ./higherResolution/800nm_ourLaser/50fs/1.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./higherResolution/800nm_ourLaser/50fs/1.5W.txt
mpiexec ./XNLO-v1-1-0 -cf ./higherResolution/800nm_ourLaser/50fs/2.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./higherResolution/800nm_ourLaser/50fs/2.5W.txt

mpiexec ./XNLO-v1-1-0 -cf ./higherResolution/800nm_ourLaser/35fs/0.5W.txt
mpiexec ./XNLO-v1-1-0 -cf ./higherResolution/800nm_ourLaser/35fs/1.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./higherResolution/800nm_ourLaser/35fs/1.5W.txt
mpiexec ./XNLO-v1-1-0 -cf ./higherResolution/800nm_ourLaser/35fs/2.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./higherResolution/800nm_ourLaser/35fs/2.5W.txt
mpiexec ./XNLO-v1-1-0 -cf ./higherResolution/800nm_ourLaser/35fs/3.0W.txt
