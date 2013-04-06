#!/bin/sh
cd "$(dirname "$0")"
java -cp ./lib/*:./lib/pi4j-core-0.0.5.jar com.neophob.udp2i2c.Runner $@
