#!/usr/bin/python3
import sys

# get the hex and strip the '0x'
raw = sys.argv[1].replace("0x", "")

b = bytes.fromhex(raw)

if len(b) != 2:
    raise Exception("invalid input: " + sys.argv[1])

print("BYTES: ", hex(b[0]), hex(b[1]))

i = (b[0] << 7) | b[1]

out = i.to_bytes(2, 'little')



print("OUT: ", hex(out[0]), hex(out[1]))

