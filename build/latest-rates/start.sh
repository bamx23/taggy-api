#!/bin/bash
cd data
make > /var/log/fastcgipp/build.log 2>&1
service nginx start
spawn-fcgi -n -p 8080 ${FCGI_MAIN}
