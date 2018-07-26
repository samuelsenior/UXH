#! /bin/bash
# . /opt/intel/bin/compilervars.sh intel64
# export LD_LIBRARY_PATH=/usr/local/openmpi/openmpi-3.0.0/lib:$LD_LIBRARY_PATH

mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/62.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/64.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/66.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/68.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/70.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/72.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/74.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/76.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/78.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/80.0W.txt
