#! /bin/bash
# . /opt/intel/bin/compilervars.sh intel64
# export LD_LIBRARY_PATH=/usr/local/openmpi/openmpi-3.0.0/lib:$LD_LIBRARY_PATH

mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/2.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/4.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/6.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/8.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/10.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/12.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/14.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/16.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/18.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/20.0W.txt
