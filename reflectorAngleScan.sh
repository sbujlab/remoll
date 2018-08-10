#!/bin/bash

for ((ANGLE=10; ANGLE<=30; ANGLE++)); do
  PREVANGLE=$(($ANGLE-1))
  
  echo $ANGLE

  #Change macro to output the right file name
  LASTFILE=remollout_refAngle${PREVANGLE}.root
  NEWFILE=remollout_refAngle${ANGLE}.root
  echo $LASTFILE

  sed -i "s/$LASTFILE/$NEWFILE/g" ./macros/reflectorAngleScan.mac

  #Update mollerMother to read correct detector geometry
  OLDGEOFILE=detector_refAngle${PREVANGLE}.gdml
  NEWGEOFILE=detector_refAngle${ANGLE}.gdml

  sed -i "s/$OLDGEOFILE/$NEWGEOFILE/g" ./geometry_optimize/mollerMother.gdml

  ./build/remoll -t 8 macros/reflectorAngleScan.mac 

done

sed -i "s/$NEWFILE/remollout_refAngle9.root/g" ./macros/reflectorAngleScan.mac
sed -i "s/$NEWGEOFILE/detector_refAngle9.gdml/g" ./geometry_optimize/mollerMother.gdml
