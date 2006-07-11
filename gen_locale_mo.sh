#!/bin/sh
po='messages.po'
mo='messages.mo'

echo "Compiling po files"
for f in locale/??/LC_MESSAGES; do

  if [ -f $f/$po ]; then
    echo " compiling $f/$po"
    msgfmt $f/$po -o $f/$mo
  else
    echo "! skipped $f/$po because of errors during the conversation"
    error=1
    continue
  fi

done

