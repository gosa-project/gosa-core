#!/bin/sh

ORIG=`pwd`
TEMPDIR="/tmp/gosa-locale"

echo
echo "Creating temporary directory..."
[ -d $TEMPDIR ] && rm -rf $TEMPDIR
mkdir $TEMPDIR

echo "Creating copy of GOsa..."
tar c . | tar x -C $TEMPDIR

echo -n "Converting .tpl files: "
pushd . &> /dev/null
cd $TEMPDIR

for template in $(find -name '*.tpl'); do
  echo -en "\rConverting .tpl files: $(basename $template)                 \r"
  sed -e 's/{t}/<?php $t= _("/g;s!{/t}!");?>!g' $template > $template.new
  mv $template.new $template
done

for class in $(find -name 'class_*.inc'); do
  echo -en "\rConverting class_*.inc files: $(basename $template)                 \r"
  sed -e 's/\($pl[DH][^=]*\)= *"\([^"]*\)";$/\1= _("\2");/g' $class > $class.new
  mv $class.new $class
done
echo -e "\rConverting .tpl files: done                    "

echo "Converting strings from gosa.conf..."
sed -e 's/headline="\([^"]*\)"/<?php $headline=_("\1");?>/g;s/name="\([^"]*\)"/<?php $t= _("\1");?>/g' contrib/gosa.conf > contrib/gosa.conf.new
mv contrib/gosa.conf.new contrib/gosa.conf

echo "Extracting languages..."
rm locale/messages.po
(echo contrib/gosa.conf; find . -name '*.[ctpi][ophn][nlpc]') | xgettext -f - --keyword=must -d Domain -L PHP -n -o locale/messages.po

echo "Merging po files with existing ones:"
error=0
for f in locale/??/LC_MESSAGES; do
  echo -n "* merging $f/messages.po: "
  [ -f $f/messages.po ] && msgmerge $f/messages.po locale/messages.po --output-file=$f/messages.po.new &> /dev/null

  # Do an extra check for dummy dir 'locale/en/LC_MESSAGES'
  if [ $? -ne 0 ]; then
    [ "$f" == "locale/en/LC_MESSAGES" ] && /bin/true
  fi

  if [ $? -eq 0 ]; then
    echo "done";
  else
    echo "failed";
    error=1
  fi

done

echo "Copying new po files, making backups:"
find . -name messages.po | while read f; do

  if [ -f $ORIG/$f ]; then
    mv $ORIG/$f $ORIG/$f.orig
  else
    echo "! skipped $ORIG/$f because of errors during the conversation"
    error=1
    continue
  fi

  echo $f | grep -q "locale/messages.po"
  if [ $? -ne 0 ]; then
    echo "* replaced $ORIG/$f"
    cp $f.new $ORIG/$f
  else
    cp $f $ORIG/$f
  fi

done

rm -rf $TEMPDIR

echo
if [ $error -eq 0 ]; then
  read -p "Do you want to erase the message.po.orig files? (y/n)" -n1 ans

  if [ "$ans" == "y" -o "$ans" == "Y" ]; then
    find $ORIG/ -type f -name 'messages.po.orig' -exec rm -f {} \;
  fi

else
  echo "There were errors during the transition. Please fix!"
  exit 1
fi

cat << EOF

---------------------------------------------------------------------

Now edit all files that have been replaced above (i.e. using kbabel
or gtranslator) and mail the changes to gosa@oss.gonicus.de to be 
included in the next release.

To see the changes you've made in GOsa, run "msgfmt messages.po" on
your freshly edited files and restart your apache after that. Set
the webbrowser to the language you've edited and go back to the
login screen.

---------------------------------------------------------------------
EOF

popd &> /dev/null

# vim:tabstop=2:expandtab:shiftwidth=2:syntax:ruler:
