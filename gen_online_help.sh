#!/bin/sh

# Defaults for Debian
CNV=latex2html
LYX=lyx-qt

# Sanity checks
if [ ! -d doc/guide/user ]; then
	echo "Can't find usable documentation structure - aborting"
	exit 1
fi

# Commands present?
for cmd in $CNV $LYX; do
	if ! which $cmd &> /dev/null; then
		echo "Can't find executable for '$cmd' - aborting"
		exit 2
	fi
done

pushd . &> /dev/null
cd doc/guide/user

for lang in *; do
	[ ! -d "$lang" ] && continue

	pushd . &> /dev/null
	echo -n "Processing language $lang..."
	cd $lang/lyx-source

	for source in *.lyx; do
		$LYX -e latex $source; d=../html/${source%%\.*}/
		[ -d $d ] || mkdir -p $d && rm -r $d/*
		$CNV -no_navigation -dir $d ${source%%\.*}.tex
	done

	popd &> /dev/null
done

popd &> /dev/null

exit 0
