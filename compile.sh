#!/bin/sh

src=$1
obj=$(echo $src | sed -e 's/\.\([^.]\)*$//')

gcc -Wall $src -o $obj
