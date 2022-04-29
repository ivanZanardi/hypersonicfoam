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

[ ! -d "$sendingDir" ] && mkdir -p "$sendingDir"


# Copy new files --------------------------------------------------------------
foldersSrc="mhdModel"
foldersApp="solvers/compressible/hy2MhdFoam"
foldersRun="hy2MhdFoam"

for folder in $foldersSrc
do
  mkdir -p $sendingDir/src/$folder
  cp -r $currentDir/src/$folder $sendingDir/src/`dirname $folder`
done

for folder in $foldersApp
do
  mkdir -p $sendingDir/applications/$folder
  cp -r $currentDir/applications/$folder $sendingDir/applications/`dirname $folder`
done

for folder in $foldersRun
do
  mkdir -p $sendingDir/run/$folder
  cp -r $currentDir/run/$folder $sendingDir/run/`dirname $folder`
done


# Compile new libraries -------------------------------------------------------
cd $sendingDir/src/mhdModel
wclean libso
wmake -j$nProcs libso


# Compile new executables -----------------------------------------------------
#---- solver ----
cd $sendingDir/applications/solvers/compressible/hy2MhdFoam
wclean
wmake -j$nProcs


# Re-set to the initial directory ---------------------------------------------
cd $currentDir


echo "|---"
echo "|--- Hypersonic hybrid CFD-MHD module $WM_PROJECT_VERSION installed successfully. Hope you'll enjoy it, $userName!"
echo "|---"

