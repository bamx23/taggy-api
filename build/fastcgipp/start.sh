#!/bin/bash
cd data
make > /var/log/fastcgipp/build.log 2>&1
spawn-fcgi -n -p 8023 ${FCGI_MAIN}
