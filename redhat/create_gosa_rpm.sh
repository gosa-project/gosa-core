#!/bin/bash -x

templatefile=gosa_2_6_plugins.templ
configfile=gosa-plugin-${1}.spec

echo "Getting plugin parameter..."

name=`awk /name/ $1/plugin.dsc | awk '{ print $3 }'`
description=`awk /description/ $1/plugin.dsc | awk -F '"' '{ print $2 }'`
version=`awk /version/ $1/plugin.dsc | awk '{ print $3 }'`
author=`awk /author/ $1/plugin.dsc | awk -F '"' '{ print $2 }'`
homepage=`awk /homepage/ $1/plugin.dsc | awk '{ print $3 }'`
depends=`awk /depends/ $1/plugin.dsc | awk '{ print $3 }'`
conflicts=`awk /conflicts/ $1/plugin.dsc | awk '{ print $3 }'`
provides=`awk /provides/ $1/plugin.dsc | awk '{ print $3 }'`
release=0

cp -f $templatefile $configfile

echo "Creating Spec file..."

sed -i "s/@@NAME@@/$name/g" $configfile  
sed -i "s/@@DESCRIPTION@@/$description/g" $configfile  
sed -i "s/@@VERSION@@/$version/g" $configfile  
sed -i "s/@@AUTHOR@@/$author/g" $configfile  
sed -i "s/@@HOMEPAGE@@/"$homepage"/g" $configfile  
sed -i "s/@@DEPENDS@@/$depends/g" $configfile  
sed -i "s/@@CONFLICTS@@/$conflicts/g" $configfile  
sed -i "s/@@PROVIDES@@/$provides/g" $configfile  
sed -i "s/@@RELEASE@@/$release/g" $configfile  

# Leere Zeilen wegräumen
sed -i "s/@@.*@@/not_defined/g" $configfile

mkdir -p /usr/src/redhat/SOURCES
mkdir -p /usr/src/redhat/SPECS

mv $configfile /usr/src/redhat/SPECS
cp -r ./$1 /tmp/$1-${version}
cd /tmp/
mv $1-${version} gosa-plugin-${1}-${version}
tar cvjf gosa-plugin-${1}-${version}.tar.bz2 ./gosa-plugin-${1}-${version}
mv -f gosa-plugin-${1}-${version}.tar.bz2 /usr/src/redhat/SOURCES


