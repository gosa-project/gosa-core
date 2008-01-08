#!/bin/sh

# Defaults for Debian
LYX=lyx-qt

# Sanity checks
if [ ! -d doc/guide/user ]; then
	echo "Can't find usable documentation structure - aborting"
	exit 1
fi

# Commands present?
for cmd in $LYX; do
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
	echo "Processing language $lang..."
	cd $lang/lyx-source

	for source in *.lyx; do
		$LYX -e pdf $source; d=../pdf/${source%%\.*}/; pdf=${source%lyx}pdf > /dev/null
		[ -d $d ] || mkdir -p $d && cp $pdf ../pdf/${source%%\.*}/ >/dev/null
	done

	popd &> /dev/null
done

popd &> /dev/null
echo
exit 0
