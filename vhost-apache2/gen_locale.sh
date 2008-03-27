#!/bin/sh

generate_po() {
  ORIG=`pwd`
  TEMPDIR="/tmp/gosa-locale"
  TRUE=`which true`

  echo
  echo "Creating temporary directory..."
  [ -d $TEMPDIR ] && rm -rf $TEMPDIR
  mkdir $TEMPDIR

  echo "Creating copy of GOsa..."
  tar c . | tar x -C $TEMPDIR

  echo "Converting .tpl files..."
  pushd . &> /dev/null
  cd $TEMPDIR

  for template in $(find . -name '*.tpl'); do
    echo "* converting .tpl files: $(basename $template)"
    sed -e 's/{t}/<?php $t= _("/g;s!{/t}!");?>!g' $template > $template.new
    mv $template.new $template
  done

  for class in $(find . -name 'class_*.inc'); do
    echo "* converting class_*.inc files: $(basename $class)"
    sed -e 's/\($pl[DH][^=]*\)= *"\([^"]*\)";$/\1= _("\2");/g' $class > $class.new
    mv $class.new $class
  done

  echo "Extracting languages..."
  rm locale/messages.po
  find . -name '*.[ctpi][ophn][nlpc]' | xgettext -f - --keyword=must -d Domain -L PHP -n -o locale/messages.po

  echo "Merging po files with existing ones"
  error=0
  for f in locale/??/LC_MESSAGES; do
    echo -n "* merging $f/messages.po: "
    [ -f $f/messages.po ] && msgmerge $f/messages.po locale/messages.po --output-file=$f/messages.po.new &> /dev/null

    # Do an extra check for dummy dir 'locale/en/LC_MESSAGES'
    if [ $? -ne 0 ]; then
      [ "$f" == "locale/en/LC_MESSAGES" ] && $TRUE
    fi

    if [ $? -eq 0 ]; then
      echo "done";
    else
      echo "failed";
      error=1
    fi

  done

  echo "Copying new po files, making backups..."
  find . -name messages.po | while read f; do

    if [ -f $ORIG/$f ]; then
      mv $ORIG/$f $ORIG/$f.orig
    else
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
  error=0
  if [ $error -eq 0 ]; then
    if [ $ASSUME_Y -eq 1 ]; then
      find $ORIG/ -type f -name 'messages.po.orig' -exec rm -f {} \;
    else
      read -p "Do you want to erase the message.po.orig files? (y/n)" -n1 ans

      if [ "$ans" == "y" -o "$ans" == "Y" ]; then
        find $ORIG/ -type f -name 'messages.po.orig' -exec rm -f {} \;
      fi
    fi

  else
    echo "There were errors during the transition. Please fix!"
    exit 1
  fi

cat <<-EOF

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
}

compile_po()
{
  po='messages.po'
  mo='messages.mo'

  echo "Compiling po files..."
  for f in locale/??/LC_MESSAGES; do

    if [ -f $f/$po ]; then
      echo "* compiling $f/$po"
      msgfmt $f/$po -o $f/$mo
    else
      echo "! skipped   $f/$po - does not exist"
      error=1
      continue
    fi

  done
}

#
# MAIN
#
GENERATE=0
COMPILE=0
ASSUME_Y=0
while getopts ":cgyh" opt
do
  case $opt in
    c) COMPILE=1
       ;;
    g) GENERATE=1;
       ;;
    y) ASSUME_Y=1;
       ;;
    h|--help)
       echo "Usage: $(basename $0) [-c] [-g] [-y]"
       echo "       -c compile existing po files into mo files"
       echo "       -g extract strings from GOsa and generate po files"
       echo "       -y assume yes"
       exit 1
       ;;
  esac
done
shift $(($OPTIND - 1))

# Default to generate
if [ $GENERATE -eq 0 -a $COMPILE -eq 0 ]; then
  GENERATE=1
fi

[ $GENERATE -eq 1 ] && generate_po
[ $COMPILE -eq 1 ]  && compile_po

# vim:tabstop=2:expandtab:shiftwidth=2:syntax:ruler:
