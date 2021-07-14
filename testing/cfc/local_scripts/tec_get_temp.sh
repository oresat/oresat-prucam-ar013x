#!/bin/bash

if [ -z $CFC_HOST ]; then
  echo "CFC_HOST defaulting to localhost:5000" >&2
  CFC_HOST=localhost:5000
else
  echo "CFC Host is $CFC_HOST"
fi

curl -s "$CFC_HOST/tec/temperature"
