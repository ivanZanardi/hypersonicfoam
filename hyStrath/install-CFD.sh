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
foldersSrc="thermophysicalModels TurbulenceModels hTCModels finiteVolume fvOptions functionObjects/forces functionObjects/field"
foldersApp="solvers/compressible/hy2Foam utilities/mesh/generation/makeAxialMesh utilities/mesh/generation/blockMeshDG"
foldersRun="hy2Foam"
filesInFolder="applications src src/functionObjects"

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

for folder in $filesInFolder
do
  find $currentDir/$folder/ -maxdepth 1 -type f | xargs cp -t $sendingDir/$folder
done

cp -r $currentDir/compilers/* $sendingDir/

# Shatalov's model ---
while true;
do
  read -p "Hello $userName, do you want to intall also Shatalov's model for oxygen flows? [Y/n] `echo $'\n> '`" yn
  case $yn in
    [Yy]*)
	cd $sendingDir/src/thermophysicalModels/strath/strathSpecie/reaction
	rm -rf Reactions && mv Reactions.Shat Reactions
	cd $sendingDir/src/thermophysicalModels/strath/strathChemistryModel
	rm -rf chemistryModel && mv chemistryModel.Shat chemistryModel
	break
	;;
    [Nn]*)
	rm -rf $sendingDir/src/thermophysicalModels/strath/strathSpecie/reaction/Reactions.Shat
	rm -rf $sendingDir/src/thermophysicalModels/strath/strathChemistryModel/chemistryModel.Shat
	break
	;;
    *)
	echo "'$yn' is not valid. Please answer yes or no."
	;;
  esac
done


# Compile new libraries -------------------------------------------------------
cd $sendingDir/src/thermophysicalModels/strath
wclean all
./Allwmake -j$nProcs

cd $sendingDir/src/TurbulenceModels
wclean all
./Allwmake -j$nProcs

cd $sendingDir/src/thermophysicalModels/strath
./AllwmakeBis -j$nProcs

cd $sendingDir/src/hTCModels
wclean libso
wmake -j$nProcs libso

cd $sendingDir/src/finiteVolume
wclean libso
wmake -j$nProcs libso

cd $sendingDir/src/functionObjects/forces
wclean libso
cd $sendingDir/src/functionObjects/field
wclean libso
cd $sendingDir/src/functionObjects
./Allwmake -j$nProcs

cd $sendingDir/src/fvOptions
wclean libso
wmake -j$nProcs libso


# Compile new executables ------------------------------------------------------
#---- solvers ----
cd $sendingDir/applications/solvers/compressible/hy2Foam
./Allwclean
./Allwmake -j$nProcs

#---- utilities ----
cd $sendingDir/applications/utilities/mesh/generation/makeAxialMesh
wclean
wmake -j$nProcs

cd $sendingDir/applications/utilities/mesh/generation/blockMeshDG
wclean all
./Allwmake -j$nProcs


# Re-set to the initial directory ---------------------------------------------
cd $currentDir


echo "|---"
echo "|--- Hypersonic CFD module $WM_PROJECT_VERSION installed successfully. Hope you'll enjoy it, $userName!"
echo "|---"

