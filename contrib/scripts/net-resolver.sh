#!/bin/sh

# Check for number of parameters
if [ $# -ne 1 ]; then
	echo Usage: $(basename $0) dns-name
	exit 1
fi

# Check for needed commands
for cmd in /usr/bin/host /usr/bin/fping /usr/sbin/arp; do
	if [ ! -x $cmd ]; then
		echo $cmd command not found - aborting
		exit 2
	fi
	eval $(echo ${cmd##*/}=$cmd)
done

mac=""
ip=$(LANG=C $host $1 | grep address | head -n1 | sed 's/^.*[^0-9]\([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*$/\1/g')
if $fping -c1 -r3 -t500 $ip &> /dev/null; then
	mac=$($arp -n | awk "/^$ip/ {print \$3}")
fi
echo "$ip;$mac"
