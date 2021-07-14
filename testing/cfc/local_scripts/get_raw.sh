#!/bin/bash

if [ -z $CFC_HOST ]; then
  echo "CFC_HOST defaulting to localhost:5000" >&2
  CFC_HOST=localhost:5000
else
  echo "CFC Host is $CFC_HOST"
fi

OUT_FILE=image_$(date +%s).bin
echo "writing buffer to $OUT_FILE" >&2
curl -s "$CFC_HOST/$OUT_FILE?raw=true" > $OUT_FILE
