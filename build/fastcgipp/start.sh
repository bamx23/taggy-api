#!/bin/bash
cd data
make
spawn-fcgi -n -s /tmp/fastcgi/fastcgi-daemon.sock main.fcgi
