#!/bin/bash
# version.sh tag

HG=$(which hg)

if [[ -n $HG ]] ; then 
	hg history -r tip | grep changeset: | awk {'print "#define LSLP_CHANGESET " "\""$2"\""'} > changeset.h 
	if [[ $# -gt 0 ]] ; then
		hg tag -r tip "$1"
		echo "#define LSLP_TAG $1" >> changeset.h
	else 
		hg history -r tip | grep tag: | awk {'print "#define LSLP_TAG " "\""$2"\""'} >> changeset.h 
	fi
fi

