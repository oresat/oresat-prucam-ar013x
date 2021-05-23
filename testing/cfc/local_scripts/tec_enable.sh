#!/bin/bash

TEMP=$1

if [ -z $TEMP ]; then
  echo "temp must be specified"
  exit 1
fi

if [ -z $CFC_HOST ]; then
  echo "CFC_HOST defaulting to localhost:5000" >&2
  CFC_HOST=localhost:5000
else
  echo "CFC Host is $CFC_HOST"
fi

curl -s -X PUT "$CFC_HOST/tec/enable?temperature=$TEMP"
