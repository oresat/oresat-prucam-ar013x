#!/bin/bash

if [ -z $CFC_HOST ]; then
  echo "CFC_HOST defaulting to localhost:5000" >&2
  CFC_HOST=localhost:5000
else
  echo "CFC Host is $CFC_HOST"
fi

OUT_FILE=image_$(date +%s).png
echo "writing image to $OUT_FILE" >&2
curl -sO "$CFC_HOST/$OUT_FILE"
