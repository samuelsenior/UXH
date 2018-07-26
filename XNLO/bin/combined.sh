#! /bin/bash
. /opt/intel/bin/compilervars.sh intel64
export LD_LIBRARY_PATH=/usr/local/openmpi/openmpi-3.0.0/lib:$LD_LIBRARY_PATH

#./400nm.sh
#./800nm.sh
#./1700nm.sh

./400nm_RALBlueLightSource.sh

./800nm_oldRALLaser.sh
./800nm_ourLaser.sh

./1030nm_Pharos_RusselsLaser.sh

./1700nm_newRALLaser.sh
