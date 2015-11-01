#!/bin/bash
cd data
make
fastcgi-daemon2 --config="$CONF_FILE"
