#!/bin/sh 

###############################################################################
#
# FILE: create_mhd_pattern.sh
# AUTHOR: Rômulo Pinho 05/08/2011
#
# Similar to create_mhd_4D.sh, but receives a pattern as input.
# 
# Example:
# create_mhd_pattern.sh "<path>/all_my_phases_start_like_this_"
#
###############################################################################

write_mhd_4D()
{
  cat $orig | sed "s/NDims = .*/NDims = 4/
		  s/TransformMatrix = .*/TransformMatrix = 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1/
		  /Offset/ s/.*/& 0/
		  /CenterOfRotation/ s/.*/& 0/
		  s/AnatomicalOrientation = .*/AnatomicalOrientation = ????/
		  /ElementSpacing/ s/.*/& 1/
		  /DimSize/ s/.*/& $nbph/
		  s/ElementDataFile = .*/ElementDataFile = LIST/" > "$dirname/$file_name_4D"
  
  for ph in $listph
  do
    phase=`basename $ph`
    echo "$phase" >> "$dirname/$file_name_4D"
  done
}

#################################################
# create_mhd_4D 	argument : repertoire	#
#################################################
if [ $# -lt 1 ]
then
  echo "Usage: $0 PREFIX [SUFFIX]"
  exit 1
fi

dirname=`dirname $1`
prefix=`basename $1`
suffix=$2

nbph=0
list_phase_file=`ls -1 $dirname/$prefix*[0-9]*$suffix.mhd`
for ph in $list_phase_file
do
  if [ -e `echo $ph | sed 's/mhd/raw/'` ]
  then
    listph="$listph $ph"
    nbph=$(( nbph + 1 ))
  fi
done

nb_phase_file=`ls -1 $dirname/$prefix*[0-9]*$suffix.mhd | wc -l`
if [ $nb_phase_file = 0 ]
then
  echo "Error: no phase found"
  exit 1
fi

orig=`echo $list_phase_file | cut -f 1 -d ' '`
listph=`echo $listph | sed 's:\.mhd:\.raw:g'`

file_name_4D=`echo "${prefix}4D${suffix}.mhd"`

write_mhd_4D
echo "$dirname/$file_name_4D"
