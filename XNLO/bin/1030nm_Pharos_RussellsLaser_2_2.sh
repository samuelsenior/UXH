#! /bin/bash
# . /opt/intel/bin/compilervars.sh intel64
# export LD_LIBRARY_PATH=/usr/local/openmpi/openmpi-3.0.0/lib:$LD_LIBRARY_PATH

mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/22.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/24.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/26.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/28.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/30.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/32.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/34.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/36.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/38.0W.txt
mpiexec ./XNLO-v1-1-0 -cf ./1030nm_Pharos_RussellsLaser_2/280fs/40.0W.txt
