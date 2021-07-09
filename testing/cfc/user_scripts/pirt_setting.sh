#!/bin/bash

SETTING=$1
VAL=$2

if [ -z $SETTING ]; then
  echo "setting must be specified"
  exit 1
fi

if [ -z $VAL ]; then
  echo "value must vbe specified"
  exit 1
fi

if [ -z $CFC_HOST ]; then
  echo "CFC_HOST defaulting to localhost:5000" >&2
  CFC_HOST=localhost:5000
else
  echo "CFC Host is $CFC_HOST"
fi

curl -s -X PUT "$CFC_HOST/pirt/setting/$SETTING?value=$VAL"
