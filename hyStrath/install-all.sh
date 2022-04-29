#!/bin/bash
cd ${0%/*} || exit 1    # Run from this directory

set -e

userName=`whoami`

nProcs=2
if [ $# -ne 0 ]
  then nProcs=$1;
fi

while true;
do
  read -p "Hello $userName, do you want to intall also the CFD-MagnetoHydroDynamics module? [Y/n] `echo $'\n> '`" yn
  case $yn in
    [Yy]*)
	break
	;;
    [Nn]*)
	break
	;;
    *)
	echo "'$yn' is not valid. Please answer yes or no."
	;;
  esac
done


# CFD Module ----------------
./install-CFD.sh $nProcs

# CFD-MHD Module ------------
if [[ "$yn" == [Yy]* ]]; then
  ./install-MHD.sh $nProcs
fi

