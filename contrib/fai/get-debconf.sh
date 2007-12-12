#!/bin/bash
# Call with:
# find /path/to/your/debmirror -name \*.deb | xargs ./get-debconf
# Move result to /etc/gosa/fai/server/debconf.d

[ -d /tmp/debconf.d ] && mkdir /tmp/debconf.d
for i in $@; do
    dpkg -e $i /tmp/debconf.d/DEBIAN
    if [ -f /tmp/debconf.d/DEBIAN/templates ]; then
	pp=$(basename $i)
	p=${pp%%_*}
        echo $p has debconf template
	mv /tmp/debconf.d/DEBIAN/templates /tmp/debconf.d/$p.templates
    fi
done


