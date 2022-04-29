#!/bin/sh
cd ${0%/*} || exit 1    # Run from this directory

set -e

userName=`whoami`

currentDir=`pwd`
sendingDir="$WM_PROJECT_USER_DIR"

nProcs=2
if [ $# -ne 0 ]
  then nProcs=$1;
fi

mkdir -p $sendingDir


# Copy new files --------------------------------------------------------------
folderApp="applications/solvers/compressible/rhoCentralReactingFoam"

mkdir -p $sendingDir/$folderApp
cp -r $currentDir/$folderApp $sendingDir/`dirname $folderApp`

cp -r $currentDir/run $sendingDir/


# Compile new solver ----------------------------------------------------------
cd $sendingDir/applications/solvers/compressible/rhoCentralReactingFoam
wclean
wmake -j$nProcs


# Re-set to the initial directory ---------------------------------------------
cd $currentDir


echo "|---"
echo "|--- rhoCentralReactingFoam solver version $WM_PROJECT_VERSION installed successfully. Hope you'll enjoy it, $userName!"
echo "|---"

